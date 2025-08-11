package djinni

import generatorTools.Spec

import djinni.ast.{TypeDef, TypeRef}
import djinni.meta.{MExpr, MPrimitive}

class CTypeTranslator(val typename: String,
                      val toCppTranslatorFn: (String) => String,
                      val fromCppTranslatorFn: (String) => String) {

}

class CTypeResolver(val spec: Spec, val cppMarshal: CppMarshal) {

  private val publicImports = Set.empty[String]
  private var importStatements = List.empty[String]


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

  private def resolveOptional(expr: MExpr): CTypeTranslator = {
    val resolved = resolve(expr)
    var nestedTypename = resolved.typename

    expr.base match {
      case opaque: meta.MOpaque =>
        case meta.MPrimitive(_idlName, jName, jniName, cName, jBoxed, jSig, objcName, objcBoxed) => {
          nestedTypename = s"djinni_optional_${resolved.typename}"
        }
        case _ =>
      case _ =>
    }

    return new CTypeTranslator(nestedTypename,
      (p) => s"djinni::c_api::Optional::toCpp(${resolved.toCppTranslatorFn(p)})",
      (p) => s"djinni::c_api::Optional::fromCpp(${resolved.fromCppTranslatorFn(p)})"
    )
  }

  def resolve(expr: MExpr): CTypeTranslator = {
    expr.base match {
      case meta.MParam(name) => {
        throw new AssertionError("Unsupported MParam type")
      }
      case meta.MDef(name, numParams, defType, body) => {
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
            s"::djinni::c_api::Record<${cppTypename}>"
          )
          case ast.ProtobufMessage(cpp, java, objc, ts, swift) => throw new AssertionError("Unsupported")
        }
      }
      case meta.MExtern(name, numParams, defType, body, _, _, _, _, _, _, _, _, _, _, c) => {
        makeTranslator(c.typename, c.translator)
      }
      case meta.MProtobuf(name, numParams, body) => makeTranslator(
        "djinni_binary_ref",
        s"::djinni::c_api::Protobuf<${body.cpp.ns}>"
      )
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
