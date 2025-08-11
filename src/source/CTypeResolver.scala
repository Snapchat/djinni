package djinni

import generatorTools.Spec

import djinni.ast.TypeRef

class CTypeTranslator(val typename: String, val toCppExpr: String, val fromCppExpr: String)

class CTypeResolver(val spec: Spec) {

  val cppMarshal = new CppMarshal(spec)

  private val publicImports = Set.empty[String]
  private var importStatements = List.empty[String]


  def valueTypeName(str: String): String = {
    spec.cNamespace + str
  }

  def ptrTypeName(str: String): String = {
    spec.cNamespace + str + "_ptr"
  }

  def resolve(typeRef: TypeRef): CTypeTranslator = {
    typeRef.resolved.base match {
      case meta.MParam(name) => {
        throw new AssertionError("Unsupported MParam type")
      }
      case meta.MDef(name, numParams, defType, body) => {
        body match {
          case ast.Enum(options, flags) => new CTypeTranslator(valueTypeName(name), "", "")
          case ast.Record(ext, fields, consts, derivingTypes) => new CTypeTranslator(ptrTypeName(name), "", "")
          case ast.Interface(ext, methods, consts) => new CTypeTranslator(ptrTypeName(name), "", "")
          case ast.ProtobufMessage(cpp, java, objc, ts, swift) => throw new AssertionError("Unsupported")
        }
      }
      case meta.MExtern(name, numParams, defType, body, _, _, _, _, _, _, _, _, _, _, c) => {
        new CTypeTranslator(c.typename, "", "")
      }
      case meta.MProtobuf(name, numParams, body) => return new CTypeTranslator("djinni_binary_ref", "", "")
      case opaque: meta.MOpaque => {
        opaque match {
          case meta.MPrimitive(_idlName, jName, jniName, cName, jBoxed, jSig, objcName, objcBoxed) => new CTypeTranslator(cName, "", "")
          case meta.MString => new CTypeTranslator("djinni_string", "djinni_string_to_cpp", "djinni_string_from_cpp")
          case meta.MDate => new CTypeTranslator("djinni_date", "djinni_date_to_cpp", "djinni_date_from_cpp")
          case meta.MBinary => new CTypeTranslator("djinni_binary_ref", "djinni_binary_to_cpp", "djinni_binary_from_cpp")
          case meta.MOptional => new CTypeTranslator("djinni_optional", "", "")
          case meta.MList => new CTypeTranslator("djinni_list", "", "")
          case meta.MSet => new CTypeTranslator("djinni_set", "", "")
          case meta.MMap => new CTypeTranslator("djinni_map", "", "")
          case meta.MArray => new CTypeTranslator("djinni_array", "", "")
          case meta.MVoid => new CTypeTranslator("void", "", "")
        }
      }
    }
  }

}
