package djinni

import djinni.ast.Record.DerivingType
import djinni.ast._
import djinni.generatorTools._
import djinni.meta._
import djinni.writer.IndentWriter

import scala.collection.mutable

class WasmGenerator(spec: Spec) extends Generator(spec) {

  val cppMarshal = new CppMarshal(spec)

  // TODO: use wasm indent style spec
  private def hppFileName(name: String): String = {
    return "Native" + spec.cppFileIdentStyle(name)
  }
  private def cppFileName(name: String): String = {
    return "Native" + spec.cppFileIdentStyle(name)
  }
  private def marshallerName(name: String): String = {
    return "Native" + idCpp.ty(name)
  }
  private def marshallerNamespace(): String = {
    return "djinni_generated"
  }
  private def includePrefix(): String = {
    // TODO: add include path prefix
    return "";
  }

  private def helperClass(name: String) = marshallerName(name)
  private def helperClass(tm: MExpr): String = helperName(tm) + helperTemplates(tm)

  private def helperName(tm: MExpr): String = tm.base match {
    case d: MDef => withNs(Some(spec.jniNamespace), helperClass(d.name))
    case e: MExtern => e.jni.translator
    case o => withNs(Some("djinni"), o match {
      case p: MPrimitive => p.idlName match {
        case "i8" => "I8"
        case "i16" => "I16"
        case "i32" => "I32"
        case "i64" => "I64"
        case "f32" => "F32"
        case "f64" => "F64"
        case "bool" => "Bool"
      }
      case MOptional => "Optional"
      case MBinary => "Binary"
      case MString => if (spec.cppUseWideStrings) "WString" else "String"
      case MDate => "Date"
      case MList => "List"
      case MSet => "Set"
      case MMap => "Map"
      case MOutcome => "Outcome"
      case MProtobuf(_,_,_) => "Protobuf"
      case MArray => "Array"
      case d: MDef => throw new AssertionError("unreachable")
      case e: MExtern => throw new AssertionError("unreachable")
      case p: MParam => throw new AssertionError("not applicable")
    })
  }

  private def helperTemplates(tm: MExpr): String = {
    def f() = if(tm.args.isEmpty) "" else tm.args.map(helperClass).mkString("<", ", ", ">")
    tm.base match {
      case MOptional =>
        assert(tm.args.size == 1)
        val argHelperClass = helperClass(tm.args.head)
        s"<${spec.cppOptionalTemplate}, $argHelperClass>"
      case MList | MSet =>
        assert(tm.args.size == 1)
        f
      case MMap | MOutcome =>
        assert(tm.args.size == 2)
        f
      case p: MProtobuf =>
        assert(tm.args.size == 0)
        val fqJavaProtoClass = p.body.java.pkg.replaceAllLiterally(".", "/") + "$" + p.name
        s"""<${withNs(Some(p.body.cpp.ns), p.name)}>"""
      case MArray =>
        assert(tm.args.size == 1)
        s"""<${helperClass(tm.args.head)}>"""
      case _ => f
    }
  }

  private def wasmType(t: TypeRef): String = t.resolved.base match {
    case p: MPrimitive => p.cName
    case MString => if (spec.cppUseWideStrings) "std::wstring" else "std::string"
    case _ => "em::val"
  }
  private def stubRetType(m: Interface.Method): String = {
    return if (m.ret.isEmpty) "void" else wasmType(m.ret.get)
  }
  private def stubParamType(t: TypeRef): String = t.resolved.base match {
    case p: MPrimitive => p.cName
    case MString => if (spec.cppUseWideStrings) "const std::wstring&" else "const std::string&"
    case _ => "const em::val&"
  }
  private def stubParamName(name: String): String = s"w_${idCpp.local(name)}"

  def include(ident: String) = q(spec.jniIncludePrefix + spec.jniFileIdentStyle(ident) + "." + spec.cppHeaderExt)

  def references(m: Meta, exclude: String = ""): Seq[SymbolReference] = m match {
    case d: MDef => List(ImportRef(include(d.name)))
    case e: MExtern => List(ImportRef(e.jni.header))
    case _ => List()
  }

  class WasmRefs(name: String, cppPrefixOverride: Option[String]=None) {
    var hpp = mutable.TreeSet[String]()
    var cpp = mutable.TreeSet[String]()

    // TODO: base lib prefix from spec
    // TODO: fix jni*
    val cppPrefix = cppPrefixOverride.getOrElse(spec.jniIncludeCppPrefix)
    hpp.add("#include " + q(cppPrefix + spec.cppFileIdentStyle(name) + "." + spec.cppHeaderExt))
    hpp.add("#include " + q(spec.jniBaseLibIncludePrefix + "djinni_wasm.hpp"))
    spec.cppNnHeader match {
      case Some(nnHdr) => hpp.add("#include " + nnHdr)
      case _ =>
    }

    def find(ty: TypeRef) { find(ty.resolved) }
    def find(tm: MExpr) {
      tm.args.foreach(find)
      find(tm.base)
    }
    def find(m: Meta) = for(r <- references(m, name)) r match {
      case ImportRef(arg) => cpp.add("#include " + arg)
      case _ =>
    }
  }
  //------------------------------------------------------------------------------

  override def generateEnum(origin: String, ident: Ident, doc: Doc, e: Enum) {
  }

  override def generateInterface(origin: String, ident: Ident, doc: Doc, typeParams: Seq[TypeParam], i: Interface) {
    val refs = new WasmRefs(ident.name)

    i.methods.foreach(m => {
      m.params.foreach(p => refs.find(p.ty))
      m.ret.foreach(refs.find)
    })

    val cls = withNs(Some(spec.cppNamespace), idCpp.ty(ident))
    val helper = marshallerName(ident)

    writeHppFileGeneric(spec.wasmOutFolder.get, marshallerNamespace(), spec.cppFileIdentStyle)(hppFileName(ident.name), origin, refs.hpp, Nil, (w => {
      w.w(s"struct $helper : ::djinni::JsInterface<$cls, $helper>").bracedSemi {
        // types
        w.wl(s"using CppType = std::shared_ptr<$cls>;")
        w.wl(s"using CppOptType = std::shared_ptr<$cls>;")
        w.wl("using JsType = em::val;")
        w.wl(s"using Boxed = $helper;")
        w.wl("")
        // mashalling
        w.wl("static CppType toCpp(JsType j) { return _fromJs(j); }")
        w.wl("static JsType fromCppOpt(const CppOptType& c) { return {_toJs(c)}; }")
        w.wl("static JsType fromCpp(const CppType& c) { return fromCppOpt(c); }")
        w.wl("")
        // method list
        w.wl("static em::val cppProxyMethods();")
        w.wl("")
        // stubs
        for (m <- i.methods) {
          val selfRef = if (m.static) "" else "const CppType& self, "
          w.w(s"static ${stubRetType(m)} ${idCpp.method(m.ident)}(${selfRef}")
          w.w(m.params.map(p => {
            s"${stubParamType(p.ty)} ${stubParamName(idCpp.local(p.ident))}"
          }).mkString(","))
          w.wl(");")
        }
        w.wl("")
        // js proxy
        w.w(s"struct JsProxy: ::djinni::JsProxyBase, $cls, ::djinni::InstanceTracker<JsProxy>").bracedSemi {
          w.wl("JsProxy(const em::val& v) : JsProxyBase(v) {}")
          for (m <- i.methods) {
            if (!m.static) {
              w.w(s"${cppMarshal.fqReturnType(m.ret)} ${idCpp.method(m.ident)}(")
              w.w(m.params.map(p => {
                s"${cppMarshal.fqParamType(p.ty)} ${idCpp.local(p.ident)}"
              }).mkString(","))
              w.wl(") override;")
            }
          }
        }
      }
    }), (w => {}))

    writeCppFileGeneric(spec.wasmOutFolder.get, marshallerNamespace(), spec.cppFileIdentStyle, includePrefix())(cppFileName(ident.name), origin, refs.cpp, (w => {
      // method list
      w.w(s"em::val $helper::cppProxyMethods()").braced {
        w.w("static const em::val methods = em::val::array(std::vector<std::string>").bracedEnd(");") {
          for (m <- i.methods) {
            w.wl(s""""${idCpp.method(m.ident)}",""")
          }
        }
        w.wl("return methods;")
      }
      w.wl("")
      // stub methods
      for (m <- i.methods) {
        val selfRef = if (m.static) "" else "const CppType& self, "
        w.w(s"${stubRetType(m)} $helper::${idCpp.method(m.ident)}(${selfRef}")
        w.w(m.params.map(p => {
          s"${stubParamType(p.ty)} ${stubParamName(p.ident)}"
        }).mkString(","))
        w.w(")").braced {
          val retPrefix = if (m.ret.isEmpty) "" else s"${helperClass(m.ret.get.resolved)}::fromCpp("
          val retSuffix = if (m.ret.isEmpty) "" else ")"
          val callPrefix = if (m.static) s"$cls::" else "self->"
          writeAlignedCall(w, s"""return ${retPrefix}${callPrefix}${idCpp.method(m.ident)}(""", m.params, s")${retSuffix}", p => {
            s"${helperClass(p.ty.resolved)}::toCpp(${stubParamName(p.ident)})"
          })
          w.wl(";")
        }
      }
      w.wl("")
      // js proxy methods
      for (m <- i.methods) {
        if (!m.static) {
          w.w(s"${cppMarshal.fqReturnType(m.ret)} ${helper}::JsProxy::${idCpp.method(m.ident)}(")
          w.w(m.params.map(p => {
            s"${cppMarshal.fqParamType(p.ty)} ${idCpp.local(p.ident)}"
          }).mkString(","))
          w.w(")").braced {
            val retPrefix = if (m.ret.isEmpty) "" else s"${helperClass(m.ret.get.resolved)}::toCpp("
            val retSuffix = if (m.ret.isEmpty) "" else ")"
            writeAlignedCall(w, s"""return ${retPrefix}_jsRef().call<${stubRetType(m)}>("${m.ident.name}", """, m.params, s")${retSuffix}", p => {
              s"${helperClass(p.ty.resolved)}::fromCpp(${idCpp.local(p.ident)})"
            })
            w.wl(";")
          }
          w.wl("")
        }
      }
      // embind
      w.w(s"EMSCRIPTEN_BINDINGS(${ident.name})").braced {
        w.wl(s"""em::class_<$cls>("${ident.name}")""").nested {
          w.wl(s""".smart_ptr<std::shared_ptr<$cls>>("${ident.name}")""")
          w.wl(s""".function("nativeDestroy", &$helper::nativeDestroy)""")
          for (m <- i.methods) {
            val funcType = if (m.static) "class_function" else "function"
            w.wl(s""".$funcType("${m.ident.name}", $helper::${idCpp.method(m.ident)})""")
          }
          w.wl(";")
        }
      }
    }))
  }

  override def generateRecord(origin: String, ident: Ident, doc: Doc, params: Seq[TypeParam], r: Record) {
    val refs = new WasmRefs(ident.name)

    val cls = withNs(Some(spec.cppNamespace), idCpp.ty(ident.name))
    val helper = marshallerName(ident.name)

    // TODO: consts in records

    writeHppFileGeneric(spec.wasmOutFolder.get, marshallerNamespace(), spec.cppFileIdentStyle)(hppFileName(ident.name), origin, refs.hpp, Nil, (w => {
      w.wl(s"struct $helper").bracedSemi {
        w.wl(s"using CppType = $cls;")
        w.wl("using JsType = em::val;")
        w.wl(s"using Boxed = $helper;")
        w.wl("")
        w.wl("static CppType toCpp(const JsType& j);")
        w.wl("static JsType fromCpp(const CppType& c);")
      }
    }), (w => {}))

    writeCppFileGeneric(spec.wasmOutFolder.get, marshallerNamespace(), spec.cppFileIdentStyle, includePrefix())(cppFileName(ident.name), origin, refs.cpp, (w => {
        w.w(s"auto $helper::toCpp(const JsType& j) -> CppType").braced {
          writeAlignedCall(w, "return {", r.fields, "}", f => {
            s"""${helperClass(f.ty.resolved)}::Boxed::toCpp(j["${f.ident.name}"])"""
          })
          w.wl(";")
        }
        w.w(s"auto $helper::fromCpp(const CppType& c) -> JsType").braced {
          w.wl("em::val js = em::val::object();")
          for (f <- r.fields) {
            w.wl(s"""js.set("${f.ident.name}", ${helperClass(f.ty.resolved)}::Boxed::fromCpp(c.${idCpp.field(f.ident)}));""")
          }
          w.wl("return js;")
        }
    }))
  }
}
