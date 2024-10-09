/**
  * Copyright 2024 Snap, Inc.
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
import java.util.regex.Pattern
import java.util.regex.Matcher

class ComposerGenerator(spec: Spec) extends Generator(spec) {

  val cppMarshal = new CppMarshal(spec)

  def jsClassNameAsCppType(jsClass: String): String = {
    val nameParts = jsClass.split("""\.""")
    val ns = nameParts.dropRight(1).mkString(".")
    val cls = nameParts.takeRight(1).last
    return Seq(ns, cls).map( e=> s"""djinni::composer::CTS{"$e"}""").mkString(", ")
  }

  class ComposerRefs(name: String, cppPrefixOverride: Option[String]=None) {
    var hpp = mutable.TreeSet[String]()
    var cpp = mutable.TreeSet[String]()
    var interfaces = mutable.TreeSet[String]()

    val cppPrefix = cppPrefixOverride.getOrElse(spec.composerIncludeCppPrefix)
    hpp.add("#include " + q(cppPrefix + spec.cppFileIdentStyle(name) + "." + spec.cppHeaderExt))
    hpp.add("#include " + q(spec.composerBaseLibIncludePrefix + "djinni_composer.hpp"))
    spec.cppNnHeader match {
      case Some(nnHdr) => hpp.add("#include " + nnHdr)
      case _ =>
    }

    def find(ty: TypeRef) { find(ty.resolved) }
    def find(tm: MExpr) {
      tm.args.foreach(find)
      find(tm.base)
    }
    def find(m: Meta) = {
      for(r <- references(m, name)) r match {
        case ImportRef(arg) => cpp.add("#include " + arg)
        case _ =>
      }
      for(i <- dependentInterfaces(m)) {
        interfaces.add(helperClass(i))
      }
    }
  }

  private def composerFilenameStyle(name: String): String = {
    return spec.composerFileIdentStyle(name)
  }
  private def helperNamespace(): String = {
    return spec.composerNamespace;
  }

  private def helperClass(name: String) = spec.composerClassIdentStyle(name)
  private def helperClass(tm: MExpr): String = helperName(tm) + helperTemplates(tm)
  def helperName(tm: MExpr): String = tm.base match {
    case d: MDef => withNs(Some(helperNamespace()), helperClass(d.name))
    case e: MExtern => e.composer.translator
    case o => withNs(Some("djinni::composer"), o match {
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
      case MProtobuf(_,_,_) => "Protobuf"
      case MArray => "Array"
      case d: MDef => throw new AssertionError("unreachable")
      case e: MExtern => throw new AssertionError("unreachable")
      case p: MParam => throw new AssertionError("not applicable")
      case MVoid => "Void"
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
      case MMap =>
        assert(tm.args.size == 2)
        f
      case MProtobuf(name, _, ProtobufMessage(cpp,_,_,Some(ts),_)) =>
        assert(tm.args.size == 0)
        val tsname = if (ts.ns.isEmpty) name else ts.ns + "." + name
        s"""<${withNs(Some(cpp.ns), name)}, ${jsClassNameAsCppType(tsname)}>"""
      case MArray =>
        assert(tm.args.size == 1)
        s"""<${helperClass(tm.args.head)}>"""
      case _ => f
    }
  }

  def include(ident: String) = q(spec.composerIncludePrefix + composerFilenameStyle(ident) + "." + spec.cppHeaderExt)

  def references(m: Meta, exclude: String = ""): Seq[SymbolReference] = m match {
    case d: MDef => List(ImportRef(include(d.name)))
    case e: MExtern => List(ImportRef(resolveExtComposerHdr(e.composer.header)))
    case _ => List()
  }

  def resolveExtComposerHdr(path: String) = {
    path.replaceAll("\\$", spec.composerBaseLibIncludePrefix);
  }

  def dependentInterfaces(m: Meta): Seq[MExpr] = m match {
    case d: MDef => if (d.defType == DInterface) List(MExpr(d, List())) else List()
    case e: MExtern => if (e.defType == DInterface) List(MExpr(e, List())) else List()
    case _ => List()
  }

  private def stubRetSchema(m: Interface.Method): String = {
    return if (m.ret.isEmpty) "ValueSchema::voidType()" else s"${schemaOrReference(m.ret.get)}"
  }
  private def exceptionHandlingTraits(m: Interface.Method): String = {
    return if (m.ret.isEmpty) "void" else helperClass(m.ret.get.resolved)
  }

  private def stubParamName(name: String): String = s"c_${idCpp.local(name)}"

  private def schemaTypeNameForInterface(ident: Ident): String = s"_djinni_interface_${ident.name}"
  private def schemaTypeNameForStaticInterface(ident: Ident): String = s"_djinni_interface_${ident.name}_statics"
  private def schemaTypeNameForRecord(ident: Ident): String = s"_djinni_record_${ident.name}"

  private def schemaOrReference(ty: TypeRef): String = {
    val m = ty.resolved
    if (isInterface(m))
      return s"${helperClass(m)}::schemaRef()"
    else
      return s"${helperClass(m)}::schema()"
  }

  override def generateEnum(origin: String, ident: Ident, doc: Doc, e: Enum) {
    val refs = new ComposerRefs(ident.name)
    val cls = cppMarshal.fqTypename(ident, e)
    val helper = helperClass(ident)
    //TODO: support namespace?
    // val fullyQualifiedName = withWasmNamespace(idJs.ty(ident))
    val fullyQualifiedName = idJs.ty(ident)
    writeHppFileGeneric(spec.composerOutFolder.get, helperNamespace(), composerFilenameStyle)(ident.name, origin, refs.hpp, Nil, (w => {
      w.wl(s"using $helper = ::djinni::composer::Enum<$cls>;")
    }), (w => {}))
  }

  override def generateRecord(origin: String, ident: Ident, doc: Doc, params: Seq[TypeParam], r: Record) {
    val refs = new ComposerRefs(ident.name)
    r.fields.foreach(f => refs.find(f.ty))
    r.consts.foreach(c => refs.find(c.ty))

    val cls = withNs(Some(spec.cppNamespace), idCpp.ty(ident.name))
    val helper = helperClass(ident)

    writeHppFileGeneric(spec.composerOutFolder.get, helperNamespace(), composerFilenameStyle)(ident.name, origin, refs.hpp, Nil, (w => {
      w.wl(s"struct $helper").bracedSemi {
        w.wl(s"using CppType = $cls;")
        w.wl("using ComposerType = Composer:: Value;")
        w.wl(s"using Boxed = $helper;")
        w.wl
        w.wl("static CppType toCpp(const ComposerType& v);")
        w.wl("static ComposerType fromCpp(const CppType& c);")
        w.wl
        w.wl("static const Composer::ValueSchema& schema() noexcept;")
      }
    }), (w => {}))
    writeCppFileGeneric(spec.composerOutFolder.get, helperNamespace(), composerFilenameStyle, spec.composerIncludePrefix) (ident.name, origin, refs.cpp, (w => {
      w.wl("using namespace Composer;")
      w.wl
      w.w(s"auto $helper::toCpp(const ComposerType& v) -> CppType").braced {
        w.wl("auto o = v.getTypedObjectRef();")
        var nextIdx = 0;
        writeAlignedCall(w, "return {", r.fields, "}", f => {
          val idx = nextIdx
          nextIdx += 1
          s"${helperClass(f.ty.resolved)}::Boxed::toCpp(o->getProperty(${idx}))"
        })
        w.wl(";")
      }
      w.w(s"auto $helper::fromCpp(const CppType& c) -> ComposerType").braced {
        w.wl("auto o = ValueTypedObject::make(schema().getClassRef(),").bracedEnd(");"){
          for (f <- r.fields) {
            w.wl(s"${helperClass(f.ty.resolved)}::Boxed::fromCpp(${cppMarshal.maybeMove("c." + idCpp.field(f.ident), f.ty)}),")
          }
        }
        w.wl("return Value(o);")
      }
      w.w(s"const ValueSchema& $helper::schema() noexcept").braced {
        //FIXME: _djinnin_record_namespace?
        w.wl(s"""static auto schema = ValueSchema::cls(STRING_LITERAL("${schemaTypeNameForRecord(ident)}"), false,""").bracedEnd(");") {
          for (f <- r.fields) {
            w.wl(s"""ClassPropertySchema(STRING_LITERAL("${idJs.field(f.ident)}"), ${schemaOrReference(f.ty)}),""")
          }
        }
        w.wl("return schema;")
      }
      // FIXME: constants?
      // if (!spec.wasmOmitConstants && !r.consts.isEmpty) {
      //   generateWasmConstants(w, ident, r.consts);
      // }
    }))
  }

  override def generateInterface(origin: String, ident: Ident, doc: Doc, typeParams: Seq[TypeParam], i: Interface) {
    val refs = new ComposerRefs(ident.name)
    i.consts.foreach(c => refs.find(c.ty))
    i.methods.foreach(m => {
      m.params.foreach(p => refs.find(p.ty))
      m.ret.foreach(refs.find)
    })
    val cls = withNs(Some(spec.cppNamespace), idCpp.ty(ident))
    val helper = helperClass(ident)
    writeHppFileGeneric(spec.composerOutFolder.get, helperNamespace(), composerFilenameStyle)(ident.name, origin, refs.hpp, Nil, (w => {
      w.w(s"struct $helper : ::djinni::composer::JsInterface<$cls, $helper>").bracedSemi {
        w.wl("static void registerSchema(bool resolve) noexcept;")
        w.wl("static const Composer::ValueSchema& schemaRef() noexcept;")
        w.wl("static const Composer::ValueSchema& schema() noexcept;")

        // cpp marshal helper
        if (i.ext.cpp) {
          w.wl("static Composer::Ref<Composer::ValueTypedProxyObject> toComposer(const CppOptType& c);")
        }

        // js proxy
        if (i.ext.js) {
          w.w(s"struct ComposerProxy: $cls, ::djinni::composer::ComposerProxyBase").bracedSemi {
            w.wl("ComposerProxy(Composer::Ref<Composer::ValueTypedProxyObject> js) : ComposerProxyBase(js) {}")
            for (m <- i.methods.filter(m => !m.static)) {
              w.w(s"${cppMarshal.fqReturnType(m.ret)} ${idCpp.method(m.ident)}(")
              w.w(m.params.map(p => {
                s"${cppMarshal.fqParamType(p.ty)} ${idCpp.local(p.ident)}"
              }).mkString(","))
              val constModifier = if (m.const) " const" else ""
              w.wl(s")$constModifier override;")
            }
          }
        }

        //static methods
        val staticMethods = i.methods.filter(m => m.static)
        if (!staticMethods.isEmpty) {
          w.wl("static void djinniInitStaticMethods(Composer::Ref<Composer::ValueMap> m) noexcept;")
        }

        //TODO ???
      //   // init consts
      //   if (!spec.wasmOmitConstants && !i.consts.isEmpty) {
      //     w.wl("static void staticInitializeConstants();");
      //   }
      }
    }), (w => {}))

    writeCppFileGeneric(spec.composerOutFolder.get, helperNamespace(), composerFilenameStyle, spec.composerIncludePrefix)(ident.name, origin, refs.cpp, (w => {
      w.wl("using namespace Composer;")
      w.wl("using namespace std::placeholders;")
      w.wl
      w.wl(s"""static STRING_CONST(schemaName, "${schemaTypeNameForInterface(ident)}");""")
      w.wl
      if (i.ext.cpp) {
        // method trampolines
        w.wl("namespace shim {")
        for (m <- i.methods.filter(m => !m.static || m.lang.js)) {
          val selfRef = if (m.static) "" else s"const ${helper}::CppType& self, "
          w.w(s"static Value ${idCpp.method(m.ident)}(${selfRef}const ValueFunctionCallContext& callContext)").braced {
            var idx = 0;
            for (p <- m.params) {
              w.wl(s"const Value& ${stubParamName(p.ident)} = callContext.getParameter($idx);")
              idx += 1
            }
            if (!m.ret.isEmpty) w.w("auto r = ")
            if (m.static) w.w(s"$cls::") else w.w("self->")
            writeAlignedCall(w, s"""${idCpp.method(m.ident)}(""", m.params, ")", p => {
              s"${helperClass(p.ty.resolved)}::toCpp(${stubParamName(p.ident)})"
            })
            w.wl(";")
            if (m.ret.isEmpty) {
              w.wl("return Value::undefined();")
            } else {
              val r = m.ret.get
              w.wl(s"return ${helperClass(r.resolved)}::fromCpp(${cppMarshal.maybeMove("r", r)});")
            }
          }
        }
        w.wl("}")
        // instance method object factory method
        w.w(s"Ref<ValueTypedProxyObject> $helper::toComposer(const CppOptType& c)").braced{
          w.w("auto o = ValueTypedObject::make(schema().getClassRef(),").bracedEnd(");") {
            for (m <- i.methods.filter(m => !m.static)) {
              w.wl(s"""djinni::composer::tsFunc<${exceptionHandlingTraits(m)}>(std::bind(shim::${idCpp.method(m.ident)}, c, _1)),""")
            }
          }
          w.wl("return makeShared<djinni::composer::DjinniCppProxyObject<CppType::element_type>>(o, c);")
        }
      }
      //value schema
      w.w(s"static const ValueSchema& unresolvedSchema() noexcept").braced {
        w.wl(s"static auto schema = ValueSchema::cls(schemaName(), true,").bracedEnd(");") {
          for (m <- i.methods.filter(m => !m.static)) {
            w.w(s"""ClassPropertySchema(STRING_LITERAL("${idJs.method(m.ident)}"), ValueSchema::function(${stubRetSchema(m)},""").bracedEnd(")),") {
              for (p <- m.params) {
                w.wl(s"${schemaOrReference(p.ty)},")
              }
            }
          }
        }
        w.wl("return schema;")
      }
      w.w(s"void $helper::registerSchema(bool resolve) noexcept").braced {
        //register dependent interfaces (params and return that are interfaces but not this one)
        for (t <- refs.interfaces.filter(t => t != withNs(Some(helperNamespace), helperClass(ident)))) {
          w.wl(s"${t}::registerSchema(resolve);")
        }
        w.wl("static bool flag[2] = {false, false};")
        w.wl("if (std::exchange(flag[resolve ? 1 : 0], true) == false) { djinni::composer::registerSchemaImpl(unresolvedSchema(), resolve); }")
      }
      // type reference
      w.w(s"const ValueSchema& $helper::schemaRef() noexcept").braced {
        w.wl("static auto ref = ValueSchema::typeReference(ValueSchemaTypeReference::named(schemaName()));")
        w.wl("return ref;")
      }
      w.w(s"const ValueSchema& $helper::schema() noexcept").braced {
        w.wl(s"static auto schema = djinni::composer::getResolvedSchema<$helper>(schemaName());")
        w.wl("return schema;")
      }
      // js proxy
      if (i.ext.js) {
        var idx = 0
        for (m <- i.methods.filter(m => !m.static)) {
          w.w(s"${cppMarshal.fqReturnType(m.ret)} $helper::ComposerProxy::${idCpp.method(m.ident)}(")
          w.w(m.params.map(p => {
            s"${cppMarshal.fqParamType(p.ty)} ${idCpp.local(p.ident)}"
          }).mkString(","))
          val constModifier = if (m.const) " const" else ""
          w.w(s")$constModifier").braced {
            if (!m.ret.isEmpty) {
              w.w("auto ret = ")
            }
            w.w(s"callJsMethod(${idx},").bracedEnd(");") {
              for (p <- m.params) { 
                w.wl(s"${helperClass(p.ty.resolved)}::fromCpp(${cppMarshal.maybeMove(idCpp.local(p.ident), p.ty)}),")
              }
            }
            idx += 1
            if (!m.ret.isEmpty) {
              w.wl(s"return ${helperClass(m.ret.get.resolved)}::toCpp(ret);")
            }
          }
        }
      }
      // static methods
      val staticMethods = i.methods.filter(m => m.static && m.lang.js)
      if (!staticMethods.isEmpty) {
        // object for static methods
        w.wl(s"void ${helper}::djinniInitStaticMethods(Ref<ValueMap> m) noexcept").braced {
          w.wl(s"""auto unresolvedStaticSchema = ValueSchema::cls(STRING_LITERAL("${schemaTypeNameForStaticInterface(ident)}"), false,""").bracedEnd(");") {
            for (m <- staticMethods) {
              w.w(s"""ClassPropertySchema(STRING_LITERAL("${idJs.method(m.ident)}"), ValueSchema::function(${stubRetSchema(m)},""").bracedEnd(")),") {
                for (p <- m.params) {
                  w.wl(s"${schemaOrReference(p.ty)},")
                }
              }
            }
          }
          w.wl(s"auto staticSchema = djinni::composer::resolveSchema(unresolvedStaticSchema, [] { registerSchema(false); registerSchema(true);} );")
          w.w(s"""(*m)[STRING_LITERAL("${idJs.ty(ident)}")] = Value(ValueTypedObject::make(staticSchema.getClassRef(),""").bracedEnd("));") {
            for (m <- staticMethods) {
              w.wl(s"""djinni::composer::tsFunc<${exceptionHandlingTraits(m)}>(shim::${idCpp.method(m.ident)}),""")
            }
          }
        }
      }
    }))
  }
}
