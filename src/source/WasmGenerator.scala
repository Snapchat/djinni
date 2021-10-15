/**
  * Copyright 2021 Snap, Inc.
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *    http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */

package djinni

import djinni.ast.Record.DerivingType
import djinni.ast._
import djinni.generatorTools._
import djinni.meta._
import djinni.writer.IndentWriter

import scala.collection.mutable

class WasmGenerator(spec: Spec) extends Generator(spec) {

  val cppMarshal = new CppMarshal(spec)

  private def wasmFilenameStyle(name: String): String = {
    return spec.jniFileIdentStyle(name)
  }
  private def helperNamespace(): String = {
    return spec.jniNamespace;
  }

  private def helperClass(name: String) = spec.jniClassIdentStyle(name)
  private def helperClass(tm: MExpr): String = helperName(tm) + helperTemplates(tm)
  def helperName(tm: MExpr): String = tm.base match {
    case d: MDef => withNs(Some(helperNamespace()), helperClass(d.name))
    case e: MExtern => e.wasm.translator
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
        val tsname = if (p.body.ts.ns.isEmpty) p.name else p.body.ts.ns + "." + p.name
        s"""<${withNs(Some(p.body.cpp.ns), p.name)}, ${jsClassNameAsCppType(tsname)}>"""
      case MArray =>
        assert(tm.args.size == 1)
        s"""<${helperClass(tm.args.head)}>"""
      case _ => f
    }
  }

  def wasmType(tm: MExpr): String = tm.base match {
    case p: MPrimitive => p.cName
    case MString => if (spec.cppUseWideStrings) "std::wstring" else "std::string"
    case d: MDef => d.defType match {
      case DEnum => "int32_t"
      case _ => "em::val"
    }
    case e: MExtern => e.wasm.typename
    case _ => "em::val"
  }
  def wasmType(t: TypeRef): String = wasmType(t.resolved)

  private def stubRetType(m: Interface.Method): String = {
    return if (m.ret.isEmpty) "void" else wasmType(m.ret.get)
  }
  private def stubParamType(t: TypeRef): String = t.resolved.base match {
    case p: MPrimitive => p.cName
    case MString => if (spec.cppUseWideStrings) "const std::wstring&" else "const std::string&"
    case d: MDef => d.defType match {
      case DEnum => "int32_t"
      case _ => "const em::val&"
    }
    case e: MExtern => e.defType match {
      case DEnum => e.wasm.typename
      case _ => "const " + e.wasm.typename + "&"
    }
    case _ => "const em::val&"
  }

  private def stubParamName(name: String): String = s"w_${idCpp.local(name)}"

  def jsClassNameAsCppType(jsClass: String): String = {
    val classNameChars = jsClass.toList.map(c => s"'$c'")
    s"""::djinni::JsClassName<${classNameChars.mkString(",")}>"""
  }

  def include(ident: String) = q(spec.wasmIncludePrefix + wasmFilenameStyle(ident) + "." + spec.cppHeaderExt)

  def references(m: Meta, exclude: String = ""): Seq[SymbolReference] = m match {
    case d: MDef => List(ImportRef(include(d.name)))
    case e: MExtern => List(ImportRef(e.wasm.header))
    case _ => List()
  }

  class WasmRefs(name: String, cppPrefixOverride: Option[String]=None) {
    var hpp = mutable.TreeSet[String]()
    var cpp = mutable.TreeSet[String]()

    val cppPrefix = cppPrefixOverride.getOrElse(spec.wasmIncludeCppPrefix)
    hpp.add("#include " + q(cppPrefix + spec.cppFileIdentStyle(name) + "." + spec.cppHeaderExt))
    hpp.add("#include " + q(spec.wasmBaseLibIncludePrefix + "djinni_wasm.hpp"))
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

  private def generateWasmConstants(w: IndentWriter, ident: Ident, consts: Seq[Const]) {
    val nsprefix = spec.cppNamespace.replaceAll("::", "_")
    val helper = helperClass(ident)
    var dependentTypes = mutable.TreeSet[String]()
    def writeJsConst(w: IndentWriter, ty: TypeRef, v: Any): Unit = v match {
      case l: Long if wasmType(ty).equalsIgnoreCase("int64_t") => w.w(s"""BigInt("${l.toString}")""")
      case l: Long => w.w(l.toString)
      case d: Double => w.w(d.toString)
      case b: Boolean => w.w(if (b) "true" else "false")
      case s: String => w.w(s)
      case e: EnumValue => {
        w.w(s"Module.${idJs.ty(ty.expr.ident)}.${idJs.enum(e)}")
        dependentTypes.add(helperClass(ty.expr.ident))
      }
      case v: ConstRef => {
        w.w(s"Module.${idJs.ty(ident)}.${idJs.const(v)}")
      }
      case z: Map[_, _] => { // Value is record
        val recordMdef = ty.resolved.base.asInstanceOf[MDef]
        val record = recordMdef.body.asInstanceOf[Record]
        val vMap = z.asInstanceOf[Map[String, Any]]
        w.w("").braced {
          // Use exact sequence
          val skipFirst = SkipFirst()
          for (f <- record.fields) {
            skipFirst {w.wl(",")}
            w.w(s"${idJs.field(f.ident)}: ")
            writeJsConst(w, f.ty, vMap.apply(f.ident.name))
          }
          w.wl
        }
      }
    }
    w.wl
    w.w(s"namespace").braced {
      w.wl(s"EM_JS(void, djinni_init_${nsprefix}_${ident.name}_consts, (), {").nested {
        w.w(s"if (!('${idJs.ty(ident)}' in Module))").braced {
          w.wl(s"Module.${idJs.ty(ident)} = {};")
        }
        for (c <- consts) {
          w.w(s"Module.${idJs.ty(ident)}.${idJs.const(c.ident)} = ")
          writeJsConst(w, c.ty, c.value)
          w.wl(";")
        }
      }
      w.wl("})")
    }
    w.w(s"void $helper::staticInitialize()").braced {
      w.wl("static std::once_flag initOnce;")
      w.wl(s"std::call_once(initOnce, djinni_init_${nsprefix}_${ident.name}_consts);")
    }
    w.wl
    w.w(s"EMSCRIPTEN_BINDINGS(${nsprefix}_${ident.name}_consts)").braced {
      for (d <- dependentTypes) {
        if (d != helper)
          w.wl(s"$d::staticInitialize();");
      }
      w.wl(s"$helper::staticInitialize();");
    }
  }

  //------------------------------------------------------------------------------

  override def generateEnum(origin: String, ident: Ident, doc: Doc, e: Enum) {
    val nsprefix = spec.cppNamespace.replaceAll("::", "_")
    val refs = new WasmRefs(ident.name)
    refs.cpp.add("#include <mutex>")
    val cls = cppMarshal.fqTypename(ident, e)
    val helper = helperClass(ident)
    writeHppFileGeneric(spec.wasmOutFolder.get, helperNamespace(), wasmFilenameStyle)(ident.name, origin, refs.hpp, Nil, (w => {
      w.w(s"struct $helper: ::djinni::WasmEnum<$cls>").bracedSemi{
        w.wl("static void staticInitialize();");
      }
    }), (w => {}))
    writeCppFileGeneric(spec.wasmOutFolder.get, helperNamespace(), wasmFilenameStyle, spec.wasmIncludePrefix)(ident.name, origin, refs.cpp, (w => {
      w.w(s"namespace").braced {
        w.wl(s"EM_JS(void, djinni_init_${nsprefix}_${ident.name}, (), {").nested {
          w.w(s"Module.${idJs.ty(ident)} = ").braced {
            writeEnumOptionNone(w, e, idJs.enum, ":")
            writeEnumOptions(w, e, idJs.enum, ":")
            writeEnumOptionAll(w, e, idJs.enum, ":")
          }
        }
        w.wl("})")
      }
      w.wl
      w.w(s"void $helper::staticInitialize()").braced {
        w.wl("static std::once_flag initOnce;")
        w.wl(s"std::call_once(initOnce, djinni_init_${nsprefix}_${ident.name});")
      }
      w.wl
      w.w(s"EMSCRIPTEN_BINDINGS(${nsprefix}_${ident.name})").braced {
        w.wl(s"$helper::staticInitialize();")
      }
    }))
  }

  override def generateInterface(origin: String, ident: Ident, doc: Doc, typeParams: Seq[TypeParam], i: Interface) {
    val refs = new WasmRefs(ident.name)
    i.consts.foreach(c => refs.find(c.ty))
    i.methods.foreach(m => {
      m.params.foreach(p => refs.find(p.ty))
      m.ret.foreach(refs.find)
    })

    val cls = withNs(Some(spec.cppNamespace), idCpp.ty(ident))
    val helper = helperClass(ident)

    writeHppFileGeneric(spec.wasmOutFolder.get, helperNamespace(), wasmFilenameStyle)(ident.name, origin, refs.hpp, Nil, (w => {
      w.w(s"struct $helper : ::djinni::JsInterface<$cls, $helper>").bracedSemi {
        // types
        w.wl(s"using CppType = std::shared_ptr<$cls>;")
        w.wl(s"using CppOptType = std::shared_ptr<$cls>;")
        w.wl("using JsType = em::val;")
        w.wl(s"using Boxed = $helper;")
        w.wl
        // mashalling
        w.wl("static CppType toCpp(JsType j) { return _fromJs(j); }")
        w.wl("static JsType fromCppOpt(const CppOptType& c) { return {_toJs(c)}; }")
        w.wl("static JsType fromCpp(const CppType& c) { return fromCppOpt(c); }")
        w.wl
        // method list
        if (i.ext.cpp) {
          w.wl("static em::val cppProxyMethods();")
        }
        w.wl
        // stubs
        if (i.ext.cpp) {
          for (m <- i.methods) {
            val selfRef = if (m.static) "" else if (m.params.isEmpty) "const CppType& self" else "const CppType& self, "
            w.w(s"static ${stubRetType(m)} ${idCpp.method(m.ident)}(${selfRef}")
            w.w(m.params.map(p => {
              s"${stubParamType(p.ty)} ${stubParamName(idCpp.local(p.ident))}"
            }).mkString(","))
            w.wl(");")
          }
          w.wl
        }
        // js proxy
        if (i.ext.js) {
          w.w(s"struct JsProxy: ::djinni::JsProxyBase, $cls, ::djinni::InstanceTracker<JsProxy>").bracedSemi {
            w.wl("JsProxy(const em::val& v) : JsProxyBase(v) {}")
            for (m <- i.methods) {
              if (!m.static) {
                w.w(s"${cppMarshal.fqReturnType(m.ret)} ${idCpp.method(m.ident)}(")
                w.w(m.params.map(p => {
                  s"${cppMarshal.fqParamType(p.ty)} ${idCpp.local(p.ident)}"
                }).mkString(","))
                val constModifier = if (m.const) " const" else ""
                w.wl(s")$constModifier override;")
              }
            }
          }
        }
        // init consts
        if (!i.consts.isEmpty) {
          w.wl("static void staticInitialize();");
        }
      }
    }), (w => {}))

    writeCppFileGeneric(spec.wasmOutFolder.get, helperNamespace(), wasmFilenameStyle, spec.wasmIncludePrefix)(ident.name, origin, refs.cpp, (w => {
      // method list
      if (i.ext.cpp) {
        w.w(s"em::val $helper::cppProxyMethods()").braced {
          w.w("static const em::val methods = em::val::array(std::vector<std::string>").bracedEnd(");") {
            for (m <- i.methods) {
              w.wl(s""""${idJs.method(m.ident)}",""")
            }
          }
          w.wl("return methods;")
        }
      }
      w.wl
      // stub methods
      if (i.ext.cpp) {
        for (m <- i.methods) {
          val selfRef = if (m.static) "" else if (m.params.isEmpty) "const CppType& self" else "const CppType& self, "
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
        w.wl
      }
      // js proxy methods
      if (i.ext.js) {
        for (m <- i.methods) {
          if (!m.static) {
            val constModifier = if (m.const) " const" else ""
            w.w(s"${cppMarshal.fqReturnType(m.ret)} ${helper}::JsProxy::${idCpp.method(m.ident)}(")
            w.w(m.params.map(p => {
              s"${cppMarshal.fqParamType(p.ty)} ${idCpp.local(p.ident)}"
            }).mkString(","))
            w.w(s")$constModifier").braced {
              val methodName = q(idJs.method(m.ident.name)) + (if (m.params.isEmpty) "" else ", ")
              writeAlignedCall(w, s"auto ret = callMethod($methodName", m.params, s")", p => {
                s"${helperClass(p.ty.resolved)}::fromCpp(${idCpp.local(p.ident)})"
              })
              w.wl(";")
              w.wl("checkError(ret);")
              stubRetType(m) match {
                case "void" =>
                case "em::val" => w.wl(s"return ${helperClass(m.ret.get.resolved)}::toCpp(ret);")
                case _ => w.wl(s"return ${helperClass(m.ret.get.resolved)}::toCpp(ret.as<${stubRetType(m)}>());")
              }
            }
            w.wl
          }
        }
      }
      // embind
      w.w(s"EMSCRIPTEN_BINDINGS(${ident.name})").braced {
        w.wl(s"""em::class_<$cls>("${idJs.ty(ident.name)}")""").nested {
          w.wl(s""".smart_ptr<std::shared_ptr<$cls>>("${idJs.ty(ident.name)}")""")
          w.wl(s""".function("${idJs.method("native_destroy")}", &$helper::nativeDestroy)""")
          if (i.ext.cpp) {
            for (m <- i.methods) {
              val funcType = if (m.static) "class_function" else "function"
              w.wl(s""".$funcType("${idJs.method(m.ident.name)}", $helper::${idCpp.method(m.ident)})""")
            }
          }
          w.wl(";")
        }
      }
      // constants
      if (!i.consts.isEmpty) {
        generateWasmConstants(w, ident, i.consts);
      }
    }))
  }

  override def generateRecord(origin: String, ident: Ident, doc: Doc, params: Seq[TypeParam], r: Record) {
    val refs = new WasmRefs(ident.name)
    r.fields.foreach(f => refs.find(f.ty))
    r.consts.foreach(c => refs.find(c.ty))

    val cls = withNs(Some(spec.cppNamespace), idCpp.ty(ident.name))
    val helper = helperClass(ident)

    // TODO: consts in records

    writeHppFileGeneric(spec.wasmOutFolder.get, helperNamespace(), wasmFilenameStyle)(ident.name, origin, refs.hpp, Nil, (w => {
      w.wl(s"struct $helper").bracedSemi {
        w.wl(s"using CppType = $cls;")
        w.wl("using JsType = em::val;")
        w.wl(s"using Boxed = $helper;")
        w.wl
        w.wl("static CppType toCpp(const JsType& j);")
        w.wl("static JsType fromCpp(const CppType& c);")
        // init consts
        if (!r.consts.isEmpty) {
          w.wl("static void staticInitialize();");
        }
      }
    }), (w => {}))

    writeCppFileGeneric(spec.wasmOutFolder.get, helperNamespace(), wasmFilenameStyle, spec.wasmIncludePrefix)(ident.name, origin, refs.cpp, (w => {
      w.w(s"auto $helper::toCpp(const JsType& j) -> CppType").braced {
        writeAlignedCall(w, "return {", r.fields, "}", f => {
          s"""${helperClass(f.ty.resolved)}::Boxed::toCpp(j["${idJs.field(f.ident.name)}"])"""
        })
        w.wl(";")
      }
      w.w(s"auto $helper::fromCpp(const CppType& c) -> JsType").braced {
        w.wl("em::val js = em::val::object();")
        for (f <- r.fields) {
          w.wl(s"""js.set("${idJs.field(f.ident.name)}", ${helperClass(f.ty.resolved)}::Boxed::fromCpp(c.${idCpp.field(f.ident)}));""")
        }
        w.wl("return js;")
      }
      // constants
      if (!r.consts.isEmpty) {
        generateWasmConstants(w, ident, r.consts);
      }
    }))
  }
}
