package djinni

import generatorTools.{DeclRef, ImportRef, Spec, q}

import djinni.ast.{Ident, TypeDef, TypeRef}
import djinni.meta.{MExpr, MOptional, MPrimitive, Meta}
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
    spec.cNamespace + str + "_ref"
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

  private def isEnum(expr: MExpr): Boolean = {
    expr.base match {
      case meta.MDef(name, numParams, defType, body) => {
        body match {
          case ast.Enum(options, flags) => true
          case _ => false
        }
      }
      case _ => false
    }
  }

  private def makeNestedTranslator(inner: CTypeTranslator, typename: String, translator: String): CTypeTranslator = {
    new CTypeTranslator(typename,
      (p) => {
        val innerToCpp = inner.toCppTranslatorFn("value")
        s"${translator}::toCpp(${p}, [](auto value) { return ${innerToCpp}; })"
      },
      (p) => {
        val innerFromCpp = inner.fromCppTranslatorFn("value")
        s"$translator::fromCpp(${p}, [](auto value) { return ${innerFromCpp}; })"
      }
    )
  }

  private def resolveOptional(expr: MExpr, asBoxed: Boolean, parent: MExpr): CTypeTranslator = {
    if (isEnum(expr)) {
      return resolve(expr, true)
    }
    val resolved = resolve(expr, asBoxed)
    val primitive = getPrimitiveOrNull(expr)

    val cppOptionalTemplate = cppMarshal.fqTypename(parent)

    if (primitive != null && !asBoxed) {
      val typename = s"djinni_optional_${resolved.typename}"
      new CTypeTranslator(typename,
        (p) => s"::djinni::c_api::Optional::toCppPrimitive<${cppOptionalTemplate}, ${typename}>(${resolved.toCppTranslatorFn(p)})",
        (p) => s"::djinni::c_api::Optional::fromCppPrimitive<${cppOptionalTemplate}, ${typename}>(${resolved.fromCppTranslatorFn(p)})"
      )
    } else {
      new CTypeTranslator(resolved.typename,
        (p) => {
          val innerToCpp = resolved.toCppTranslatorFn("value")
          s"::djinni::c_api::Optional::toCpp<${cppOptionalTemplate}>(${p}, [](auto value) { return ${innerToCpp}; })"
        },
        (p) => {
          val innerFromCpp = resolved.fromCppTranslatorFn("value")
          s"::djinni::c_api::Optional::fromCpp<${cppOptionalTemplate}>(${p}, [](auto value) { return ${innerFromCpp}; })"
        }
      )
    }
  }

  private def resolveListLike(expr: MExpr, translator: String): CTypeTranslator = {
    val resolved = resolve(expr, true)

    val cppType = cppMarshal.fqTypename(expr)
    makeNestedTranslator(resolved, "djinni_array_ref", translator + s"<${cppType}>")
  }

  private def resolveMap(keyExpr: MExpr, valueExpr: MExpr): CTypeTranslator = {
    val resolvedKey = resolve(keyExpr, true)
    val resolvedValue = resolve(valueExpr, true)
    val cppTypeKey = cppMarshal.fqTypename(keyExpr)
    val cppTypeValue = cppMarshal.fqTypename(valueExpr)

    new CTypeTranslator("djinni_keyval_array_ref",
      (p) => {
        val innerToCppKey = resolvedKey.toCppTranslatorFn("key")
        val innerToCppValue = resolvedValue.toCppTranslatorFn("value")
        s"::djinni::c_api::Map<${cppTypeKey}, ${cppTypeValue}>::toCpp(${p}, [](auto key, auto value) { return std::make_pair(${innerToCppKey}, ${innerToCppValue}); })"
      },
      (p) => {
        val innerFromCppKey = resolvedKey.fromCppTranslatorFn("key")
        val innerFromCppValue = resolvedValue.fromCppTranslatorFn("value")
        s"::djinni::c_api::Map<${cppTypeKey}, ${cppTypeValue}>::fromCpp(${p}, [](auto key, auto value) { return std::make_pair(${innerFromCppKey}, ${innerFromCppValue}); })"
      })

  }

  private def resolvePrimitive(cName: String, asBoxed: Boolean): CTypeTranslator = {
    if (asBoxed) {
      new CTypeTranslator("djinni_number_ref", (p) => {
        s"::djinni::c_api::Number::toCpp<${cName}>(${p})"
      },
        (p) => {
          s"::djinni::c_api::Number::fromCpp<${cName}>(${p})"
        })
    } else {
      new CTypeTranslator(cName, p => p, p => p)
    }
  }

  private def updatePrivateImports(meta: Meta): Unit = {
    for (r <- cppMarshal.hppReferences(meta, ident.name, false)) r match {
      case ImportRef(arg) => privateImports.add("#include " + arg)
      case _ =>
    }
  }

  private def addPublicImport(path: String) = {
    publicImports.add("#include " + path)
  }

  private def addPublicImportFromDef(name: String) = {
    addPublicImport(q(spec.cIncludePrefix + name + ".h"))
  }

  private def resolveEnum(name: String, cppTypename: String, asBoxed: Boolean): CTypeTranslator = {
    val typename = valueTypeName(name)
    if (asBoxed) {
      val optionalType = s"${spec.cppOptionalTemplate}<${cppTypename}>"
      new CTypeTranslator("djinni_number_ref",
        (p) => s"::djinni::c_api::Enum<${cppTypename}, ${typename}>::toCppBoxed<${optionalType}>(${p})",
        (p) => s"::djinni::c_api::Enum<${cppTypename}, ${typename}>::fromCppBoxed<${optionalType}>(${p})"
      )
    } else {
      new CTypeTranslator(typename,
        (p) => s"::djinni::c_api::Enum<${cppTypename}, ${typename}>::toCpp(${p})",
        (p) => s"::djinni::c_api::Enum<${cppTypename}, ${typename}>::fromCpp(${p})"
      )
    }
  }

  def resolve(expr: MExpr): CTypeTranslator = {
    resolve(expr, false)
  }

  private def resolve(expr: MExpr, asBoxed: Boolean): CTypeTranslator = {
    expr.base match {
      case meta.MParam(name) => {
        throw new AssertionError("Unsupported MParam type")
      }
      case meta.MDef(name, numParams, defType, body) => {
        updatePrivateImports(expr.base)
        addPublicImportFromDef(name)
        val cppTypename = cppMarshal.fqTypename(name, body)
        body match {
          case ast.Enum(options, flags) => resolveEnum(name, cppTypename, asBoxed)
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
        updatePrivateImports(expr.base)
        addPublicImport(c.publicHeader)
        makeTranslator(c.typename, c.translator)
      }
      case meta.MProtobuf(name, numParams, body) => {
        updatePrivateImports(expr.base)
        makeTranslator(
          "djinni_binary_ref",
          s"::djinni::c_api::Protobuf<${body.cpp.ns}>"
        )
      }
      case opaque: meta.MOpaque => {
        opaque match {
          case meta.MPrimitive(_idlName, jName, jniName, cName, jBoxed, jSig, objcName, objcBoxed) => resolvePrimitive(cName, asBoxed)
          case meta.MString => makeTranslator("djinni_string_ref", "::djinni::c_api::String")
          case meta.MDate => makeTranslator("djinni_date_ref", "::djinni::c_api::Date")
          case meta.MBinary => makeTranslator("djinni_binary_ref", "::djinni::c_api::Binary")
          case meta.MOptional => resolveOptional(expr.args.head, asBoxed, expr)
          case meta.MList => resolveListLike(expr.args.head, "::djinni::c_api::List")
          case meta.MSet => resolveListLike(expr.args.head, "::djinni::c_api::Set")
          case meta.MMap => resolveMap(expr.args.head, expr.args(1))
          case meta.MArray => resolveListLike(expr.args.head, "::djinni::c_api::List")
          case meta.MVoid => new CTypeTranslator("void", p => p, p => p)
        }
      }
    }
  }

}
