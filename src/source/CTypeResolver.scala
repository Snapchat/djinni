package djinni

import ast.Ident
import generatorTools.{ImportRef, Spec, q}
import meta.{DEnum, MExpr, MPrimitive, Meta}

import scala.collection.mutable

class CTypeTranslator(val typename: String,
                      val isRefType: Boolean,
                      val translator: String) {

  def toCpp(value: String): String =
    if (translator.nonEmpty) {
      s"${translator}::toCpp(${value})"
    } else {
      value
    }

  def fromCpp(value: String): String =
    if (translator.nonEmpty) {
      s"${translator}::fromCpp(${value})"
    } else {
      value
    }
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

  private def resolveOptional(expr: MExpr, asBoxed: Boolean, parent: MExpr): CTypeTranslator = {
    val resolved = resolve(expr, asBoxed || isEnum(expr))
    val primitive = getPrimitiveOrNull(expr)

    val cppOptionalTemplate = cppMarshal.fqTypename(parent)

    if (primitive != null && !asBoxed) {
      val resolvedTypename = if (resolved.typename.endsWith("_t")) resolved.typename.substring(0, resolved.typename.length - 2) else resolved.typename
      val typename = s"djinni_optional_${resolvedTypename}"

      new CTypeTranslator(
        typename,
        false,
        s"::djinni::c_api::PrimitiveOptionalTranslator<${cppOptionalTemplate}, ${typename}>"
      )
    } else {
      val sharedPtr = cppMarshal.bySharedPtr(expr)

      if (sharedPtr) {
        new CTypeTranslator(resolved.typename, true, s"::djinni::c_api::OptionalPtrTranslator<${resolved.translator}>")
      } else {
        new CTypeTranslator(resolved.typename, true, s"::djinni::c_api::OptionalTranslator<${cppOptionalTemplate}, ${resolved.translator}>")
      }
    }
  }

  private def resolveListLike(expr: MExpr, translator: String): CTypeTranslator = {
    val resolved = resolve(expr, asBoxed = true)

    new CTypeTranslator("djinni_array_ref", true, s"$translator<${resolved.translator}>")
  }

  private def resolveMap(keyExpr: MExpr, valueExpr: MExpr): CTypeTranslator = {
    val resolvedKey = resolve(keyExpr, asBoxed = true)
    val resolvedValue = resolve(valueExpr, asBoxed = true)

    new CTypeTranslator("djinni_keyval_array_ref", true, s"::djinni::c_api::MapTranslator<${resolvedKey.translator}, ${resolvedValue.translator}>")
  }

  private def resolvePrimitive(cName: String, asBoxed: Boolean): CTypeTranslator = {
    if (asBoxed) {
      val translatorName = cName match {
        case "bool" => "BoolTranslator"

        case "uint8_t" => "UInt8Translator"
        case "uint16_t" => "UInt16Translator"
        case "uint32_t" => "UInt32Translator"
        case "uint64_t" => "UInt64Translator"

        case "int8_t" => "Int8Translator"
        case "int16_t" => "Int16Translator"
        case "int32_t" => "Int32Translator"
        case "int64_t" => "Int64Translator"

        case "float" => "FloatTranslator"
        case "double" => "DoubleTranslator"
        case _ => throw new AssertionError(s"Recognized primitive type '${cName}'")
      }

      new CTypeTranslator("djinni_number_ref", true, s"::djinni::c_api::${translatorName}")
    } else {
      new CTypeTranslator(cName, false, "")
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
    val cppTypename = cppMarshal.fqTypename(name, body)
    val typename = valueTypeName(name)
    doResolveEnum(typename, cppTypename, asBoxed)
  }

  private def doResolveEnum(typename: String, cppTypename: String, asBoxed: Boolean): CTypeTranslator = {
    if (asBoxed) {
      new CTypeTranslator("djinni_number_ref", true, s"::djinni::c_api::BoxedEnumTranslator<${cppTypename}>")
    } else {
      new CTypeTranslator(typename, false, s"::djinni::c_api::EnumTranslator<${cppTypename}, ${typename}>")
    }
  }

  private def resolveExtern(expr: MExpr, defType: meta.DefType, c: meta.MExtern.C, asBoxed: Boolean): CTypeTranslator = {
    updatePrivateImports(expr.base)
    addPublicImport(cppMarshal.resolveExtCppHdr(c.publicHeader))
    privateImports.add("#include " + cppMarshal.resolveExtCppHdr(c.privateHeader))

    if (expr.args.nonEmpty) {
      val templateArgs = expr.args.map(a => cppMarshal.fqTypename(a)).mkString(", ")
      val translator = s"${c.translator}<${templateArgs}>"

      if (c.ignoreTypeParams) {
        return new CTypeTranslator(c.typename,
          true,
          translator
        )
      } else {
        val argsTranslators = expr.args.map(a => resolve(a, asBoxed = true).translator).mkString(", ")
        val translator = s"${c.translator}<${argsTranslators}>"

        return new CTypeTranslator(c.typename, true, translator)
      }
    }

    val isEnum = defType match {
      case DEnum => true
      case _ => false
    }

    if (isEnum) {
      doResolveEnum(c.typename, c.translator, asBoxed)
    } else {
      new CTypeTranslator(c.typename, true, c.translator)
    }
  }

  def getTranslatorNameForType(name: String, td: ast.TypeDef): String = {
    val cppTypename = cppMarshal.fqTypename(name, td)
     td match {
      case ast.Enum(_, _) => {
        val typename = valueTypeName(name)
        s"::djinni::c_api::EnumTranslator<${cppTypename}, ${typename}>"
      }
      case ast.Record(_, _, _, _) => s"::djinni::c_api::RecordTranslator<${cppTypename}>"
      case ast.Interface(_, _, _) => s"::djinni::c_api::InterfaceTranslator<${cppTypename}>"
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
          case _ => new CTypeTranslator(ptrTypeName(name), true, getTranslatorNameForType(name, body))
        }
      }
      case meta.MExtern(_, _, defType, _, _, _, _, _, _, _, _, _, _, _, c) => resolveExtern(expr, defType, c, asBoxed)
      case meta.MProtobuf(_, _, _) =>
        updatePrivateImports(expr.base)
        new CTypeTranslator(
          "djinni_binary_ref",
          isRefType = true,
          s"::djinni::c_api::ProtobufTranslator<${cppMarshal.fqTypename(expr)}>"
        )
      case opaque: meta.MOpaque =>
        opaque match {
          case meta.MPrimitive(_, _, _, cName, _, _, _, _) => resolvePrimitive(cName, asBoxed)
          case meta.MString => new CTypeTranslator("djinni_string_ref", isRefType = true, "::djinni::c_api::StringTranslator")
          case meta.MDate => new CTypeTranslator("djinni_date_ref", isRefType = true, "::djinni::c_api::DateTranslator")
          case meta.MBinary => new CTypeTranslator("djinni_binary_ref", isRefType = true, "::djinni::c_api::BinaryTranslator")
          case meta.MOptional => resolveOptional(expr.args.head, asBoxed, expr)
          case meta.MList => resolveListLike(expr.args.head, "::djinni::c_api::ListTranslator")
          case meta.MSet => resolveListLike(expr.args.head, "::djinni::c_api::SetTranslator")
          case meta.MMap => resolveMap(expr.args.head, expr.args(1))
          case meta.MArray => resolveListLike(expr.args.head, "::djinni::c_api::ListTranslator")
          case meta.MVoid => new CTypeTranslator("void", false, "::djinni::c_api::VoidTranslator")
        }
    }
  }

}