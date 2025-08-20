package djinni

import ast.Ident
import generatorTools.{ImportRef, Spec, q}
import meta.{DEnum, MExpr, MPrimitive, Meta}

import scala.collection.mutable

class CTypeTranslator(val typename: String,
                      val isRefType: Boolean,
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

  private def makeTranslator(typename: String, isRefType: Boolean, translator: String): CTypeTranslator = {
    new CTypeTranslator(typename,
      isRefType,
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
      case meta.MDef(_, _, _, body) => {
        body match {
          case ast.Enum(_, _) => true
          case _ => false
        }
      }
      case meta.MExtern(_, _, defType, _, _, _, _, _, _, _, _, _, _, _, _) => {
        defType match {
          case DEnum => true
          case _ => false
        }
      }
      case _ => false
    }
  }

  private def makeNestedTranslator(inner: CTypeTranslator, typename: String, toCppTranslator: String, fromCppTranslator: String): CTypeTranslator = {
    val innerValueParamName = s"std::forward<decltype(value)>(value)"
    new CTypeTranslator(typename,
      true,
      (p) => {
        val innerToCpp = inner.toCppTranslatorFn(innerValueParamName)
        s"${toCppTranslator}(${p}, [](auto&& value) { return ${innerToCpp}; })"
      },
      (p) => {
        val innerFromCpp = inner.fromCppTranslatorFn(innerValueParamName)
        s"$fromCppTranslator(${p}, [](auto&& value) { return ${innerFromCpp}; })"
      }
    )
  }

  private def resolveOptional(expr: MExpr, asBoxed: Boolean, parent: MExpr): CTypeTranslator = {
    val resolved = resolve(expr, asBoxed || isEnum(expr))
    val primitive = getPrimitiveOrNull(expr)

    val cppOptionalTemplate = cppMarshal.fqTypename(parent)

    if (primitive != null && !asBoxed) {
      val typename = s"djinni_optional_${resolved.typename}"
      new CTypeTranslator(typename,
        false,
        (p) => s"::djinni::c_api::Optional::toCppPrimitive<${cppOptionalTemplate}, ${typename}>(${resolved.toCppTranslatorFn(p)})",
        (p) => s"::djinni::c_api::Optional::fromCppPrimitive<${cppOptionalTemplate}, ${typename}>(${resolved.fromCppTranslatorFn(p)})"
      )
    } else {
      val sharedPtr = cppMarshal.bySharedPtr(expr)
      val toCppMethodName = if (sharedPtr) "toSharedPtrCpp" else "toCpp"
      val fromCppMethodName = if (sharedPtr) "fromSharedPtrCpp" else "fromCpp"
      makeNestedTranslator(resolved,
        resolved.typename,
        s"::djinni::c_api::Optional::${toCppMethodName}<${cppOptionalTemplate}>",
        s"::djinni::c_api::Optional::${fromCppMethodName}<${cppOptionalTemplate}>")
    }
  }

  private def resolveListLike(expr: MExpr, translator: String): CTypeTranslator = {
    val resolved = resolve(expr, true)

    val cppType = cppMarshal.fqTypename(expr)
    makeNestedTranslator(resolved, "djinni_array_ref", s"$translator<${cppType}>::toCpp", s"$translator<${cppType}>::fromCpp")
  }

  private def resolveMap(keyExpr: MExpr, valueExpr: MExpr): CTypeTranslator = {
    val resolvedKey = resolve(keyExpr, true)
    val resolvedValue = resolve(valueExpr, true)
    val cppTypeKey = cppMarshal.fqTypename(keyExpr)
    val cppTypeValue = cppMarshal.fqTypename(valueExpr)

    new CTypeTranslator("djinni_keyval_array_ref",
      true,
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
      new CTypeTranslator("djinni_number_ref", true, (p) => {
        s"::djinni::c_api::Number::toCpp<${cName}>(${p})"
      },
        (p) => {
          s"::djinni::c_api::Number::fromCpp<${cName}>(${p})"
        })
    } else {
      new CTypeTranslator(cName, false, p => p, p => p)
    }
  }

  private def updatePrivateImports(meta: Meta): Unit = {
    for (r <- cppMarshal.hppReferences(meta, ident.name, forwardDeclareOnly = false)) r match {
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

  private def resolveEnum(name: String, body: ast.TypeDef, asBoxed: Boolean): CTypeTranslator = {
    val translator = getTranslatorNameForType(name, body)
    val typename = valueTypeName(name)
    doResolveEnum(typename, translator, asBoxed)
  }

  private def doResolveEnum(typename: String, translator: String, asBoxed: Boolean): CTypeTranslator = {
    if (asBoxed) {
      new CTypeTranslator("djinni_number_ref",
        true,
        (p) => s"${translator}::toCppBoxed(${p})",
        (p) => s"${translator}::fromCppBoxed(${p})"
      )
    } else {
      new CTypeTranslator(typename,
        false,
        (p) => s"${translator}::toCpp(${p})",
        (p) => s"${translator}::fromCpp(${p})"
      )
    }
  }

  private def resolveExtern(expr: MExpr, defType: meta.DefType, c: meta.MExtern.C, asBoxed: Boolean): CTypeTranslator = {
    updatePrivateImports(expr.base)
    addPublicImport(cppMarshal.resolveExtCppHdr(c.publicHeader))
    privateImports.add("#include " + cppMarshal.resolveExtCppHdr(c.privateHeader))

    val resolvedTranslator: String = if (expr.args.isEmpty) {
      c.translator
    } else {
      // Resolve to get the includes
      expr.args.foreach(a => resolve(a))
      val templateArgs = expr.args.map(a => cppMarshal.fqTypename(a)).mkString(", ")
      s"${c.translator}<${templateArgs}>"
    }

    val isEnum = defType match {
      case DEnum => true
      case _ => false
    }

    if (isEnum) {
      doResolveEnum(c.typename, resolvedTranslator, asBoxed)
    } else {
      makeTranslator(c.typename, isRefType = true, resolvedTranslator)
    }
  }

  def getTranslatorNameForType(name: String, td: ast.TypeDef): String = {
    val cppTypename = cppMarshal.fqTypename(name, td)
     td match {
      case ast.Enum(_, _) => {
        val typename = valueTypeName(name)
        s"::djinni::c_api::Enum<${cppTypename}, ${typename}>"
      }
      case ast.Record(_, _, _, _) => s"::djinni::c_api::Record<${cppTypename}>"
      case ast.Interface(_, _, _) => s"::djinni::c_api::Interface<${cppTypename}>"
      case ast.ProtobufMessage(_, _, _, _, _) => throw new AssertionError("Unsupported")
    }
  }

  def resolve(expr: MExpr): CTypeTranslator = {
    resolve(expr, asBoxed = false)
  }

  private def resolve(expr: MExpr, asBoxed: Boolean): CTypeTranslator = {
    expr.base match {
      case meta.MParam(_) => {
        throw new AssertionError("Unsupported MParam type")
      }
      case meta.MDef(name, _, _, body) => {
        updatePrivateImports(expr.base)
        addPublicImportFromDef(name)
        body match {
          case ast.Enum(_, _) => resolveEnum(name, body, asBoxed)
          case _ => makeTranslator(
            ptrTypeName(name),
            isRefType = true,
            getTranslatorNameForType(name, body)
          )
        }
      }
      case meta.MExtern(_, _, defType, _, _, _, _, _, _, _, _, _, _, _, c) => resolveExtern(expr, defType, c, asBoxed)
      case meta.MProtobuf(_, _, _) =>
        updatePrivateImports(expr.base)
        makeTranslator(
          "djinni_binary_ref",
          isRefType = true,
          s"::djinni::c_api::Protobuf<${cppMarshal.fqTypename(expr)}>"
        )
      case opaque: meta.MOpaque =>
        opaque match {
          case meta.MPrimitive(_, _, _, cName, _, _, _, _) => resolvePrimitive(cName, asBoxed)
          case meta.MString => makeTranslator("djinni_string_ref", isRefType = true, "::djinni::c_api::String")
          case meta.MDate => makeTranslator("djinni_date_ref", isRefType = true, "::djinni::c_api::Date")
          case meta.MBinary => makeTranslator("djinni_binary_ref", isRefType = true, "::djinni::c_api::Binary")
          case meta.MOptional => resolveOptional(expr.args.head, asBoxed, expr)
          case meta.MList => resolveListLike(expr.args.head, "::djinni::c_api::List")
          case meta.MSet => resolveListLike(expr.args.head, "::djinni::c_api::Set")
          case meta.MMap => resolveMap(expr.args.head, expr.args(1))
          case meta.MArray => resolveListLike(expr.args.head, "::djinni::c_api::List")
          case meta.MVoid => new CTypeTranslator("void", false, p => p, p => p)
        }
    }
  }

}