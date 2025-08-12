package djinni

import generatorTools.{DeclRef, ImportRef, Spec, q}

import djinni.ast.{Ident, TypeDef, TypeRef}
import djinni.meta.{MExpr, MPrimitive, Meta}
import djinni.writer.IndentWriter

import scala.collection.mutable

class CTypeTranslator(val typename: String,
                      val toCppTranslatorFn: (String) => String,
                      val fromCppTranslatorFn: (String) => String) {

}

class CTypeResolver(val ident: Ident, val spec: Spec, val cppMarshal: CppMarshal) {

  val publicImports = mutable.TreeSet[String]()
  val privateImports = mutable.TreeSet[String]()

  initSelfInclude()

  private def initSelfInclude(): Unit = {
    val myHeader = q(spec.cppIncludePrefix + spec.cppFileIdentStyle(ident.name) + "." + spec.cppHeaderExt)
    privateImports.add(s"#include $myHeader")
  }

  def valueTypeName(str: String): String = {
    spec.cNamespace + str
  }

  def ptrTypeName(str: String): String = {
    spec.cNamespace + str + "_ptr"
  }

  private def makeTranslator(typename: String, translator: String): CTypeTranslator = {
    new CTypeTranslator(typename,
      (p) => s"${translator}::toCpp(${p})",
      (p) => s"${translator}::fromCpp(${p})"
    )
  }

  private def getPrimitiveOrNull(expr: MExpr): meta.MPrimitive = {
    expr.base match {
      case opaque: meta.MOpaque =>
        opaque match {
          case MPrimitive(_, _, _, _, _, _, _, _) => return opaque.asInstanceOf[MPrimitive]
          case _ =>
        }
      case _ =>
    }
    null
  }

  private def resolveOptional(expr: MExpr): CTypeTranslator = {
    val resolved = resolve(expr)

    val primitive = getPrimitiveOrNull(expr)
    val nestedTypename = if (primitive != null) {
      s"djinni_optional_${resolved.typename}"
    } else {
      resolved.typename
    }

    new CTypeTranslator(nestedTypename,
      (p) => s"djinni::c_api::Optional::toCpp(${resolved.toCppTranslatorFn(p)})",
      (p) => s"djinni::c_api::Optional::fromCpp(${resolved.fromCppTranslatorFn(p)})"
    )
  }

  private def updateImports(meta: Meta): Unit = {
    for (r <- cppMarshal.hppReferences(meta, ident.name, false)) r match {
      case ImportRef(arg) => privateImports.add("#include " + arg)
      case _ =>
    }
  }

  def resolve(expr: MExpr): CTypeTranslator = {
    expr.base match {
      case meta.MParam(name) => {
        throw new AssertionError("Unsupported MParam type")
      }
      case meta.MDef(name, numParams, defType, body) => {
        updateImports(expr.base)
        val cppTypename = cppMarshal.fqTypename(name, body)
        body match {
          case ast.Enum(options, flags) => new CTypeTranslator(valueTypeName(name),
            (p) => s"static_cast<${cppTypename}>(${p})",
            (p) => s"static_cast<${valueTypeName(name)}>(${p})"
          )
          case ast.Record(ext, fields, consts, derivingTypes) => makeTranslator(
            ptrTypeName(name),
            s"::djinni::c_api::Record<${cppTypename}>"
          )
          case ast.Interface(ext, methods, consts) => makeTranslator(
            ptrTypeName(name),
            s"::djinni::c_api::Interface<${cppTypename}>"
          )
          case ast.ProtobufMessage(cpp, java, objc, ts, swift) => throw new AssertionError("Unsupported")
        }
      }
      case meta.MExtern(name, numParams, defType, body, _, _, _, _, _, _, _, _, _, _, c) => {
        updateImports(expr.base)
        makeTranslator(c.typename, c.translator)
      }
      case meta.MProtobuf(name, numParams, body) => {
        updateImports(expr.base)
        makeTranslator(
          "djinni_binary_ref",
          s"::djinni::c_api::Protobuf<${body.cpp.ns}>"
        )
      }
      case opaque: meta.MOpaque => {
        opaque match {
          case meta.MPrimitive(_idlName, jName, jniName, cName, jBoxed, jSig, objcName, objcBoxed) => new CTypeTranslator(cName, p => p, p => p)
          case meta.MString => makeTranslator("djinni_string", "::djinni::c_api::String")
          case meta.MDate => makeTranslator("djinni_date", "::djinni::c_api::Date")
          case meta.MBinary => makeTranslator("djinni_binary_ref", "::djinni::c_api::Binary")
          case meta.MOptional => resolveOptional(expr.args.head)
          case meta.MList => makeTranslator("djinni_list", "::djinni::c_api::List")
          case meta.MSet => makeTranslator("djinni_set", "::djinni::c_api::Set")
          case meta.MMap => makeTranslator("djinni_map", "::djinni::c_api::Map")
          case meta.MArray => makeTranslator("djinni_array", "::djinni::c_api::Array")
          case meta.MVoid => new CTypeTranslator("void", p => p, p => p)
        }
      }
    }
  }

}
