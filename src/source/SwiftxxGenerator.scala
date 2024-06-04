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

import java.io._
import djinni.ast.Record.DerivingType
import djinni.ast._
import djinni.generatorTools._
import djinni.meta._
import djinni.writer.IndentWriter
import scala.collection.mutable.ListBuffer
import scala.collection.mutable.TreeSet
import java.util.regex.Pattern
import java.util.regex.Matcher
import scala.collection.mutable

class SwiftxxGenerator(spec: Spec) extends Generator(spec) {
  val cppMarshal = new CppMarshal(spec)
  val marshal = new SwiftxxMarshal(spec)

  class SwiftRefs(name: String) {
    var swiftHpp = mutable.TreeSet[String]()
    var swiftCpp = mutable.TreeSet[String]()
    swiftHpp.add("#include " + q(spec.cppFileIdentStyle(name) + "." + spec.cppHeaderExt))
    swiftHpp.add("#include " + q("djinni_support.hpp"))
    spec.cppNnHeader match {
      case Some(nnHdr) => swiftHpp.add("#include " + nnHdr)
      case _ =>
    }

    def find(ty: TypeRef) { find(ty.resolved) }
    def find(tm: MExpr) {
      tm.args.foreach(find)
      find(tm.base)
    }
    def find(m: Meta) = for(r <- marshal.references(m, name)) r match {
      case ImportRef(arg) => swiftCpp.add("#include " + arg)
      case _ =>
    }
  }

  val writeSwiftCppFile = writeCppFileGeneric(spec.swiftxxOutFolder.get, spec.swiftxxNamespace, spec.swiftxxFileIdentStyle, "") _
  def writeSwiftHppFile(name: String, origin: String, includes: Iterable[String], fwds: Iterable[String], f: IndentWriter => Unit, f2: IndentWriter => Unit = (w => {})) =
    writeHppFileGeneric(spec.swiftxxOutFolder.get, spec.swiftxxNamespace, spec.swiftxxFileIdentStyle)(name, origin, includes, fwds, f, f2)

  override def generateEnum(origin: String, ident: Ident, doc: Doc, e: Enum) {
    val refs = new SwiftRefs(ident.name)
    writeSwiftHppFile(ident, origin, refs.swiftHpp, Nil, w => {
      w.wl(s"using ${spec.swiftxxClassIdentStyle(ident)} = djinni::swift::Enum<${cppMarshal.fqTypename(ident, e)}>;")
    })
    writeSwiftCppFile(ident, origin, List[String](), w => {})
  }

  override def generateRecord(origin: String, ident: Ident, doc: Doc, params: Seq[TypeParam], r: Record) {
    val refs = new SwiftRefs(ident.name)
    r.fields.foreach(f => refs.find(f.ty))
    val helper = marshal.helperClass(ident)
    writeSwiftHppFile(ident, origin, refs.swiftHpp, Nil, w => {
      w.w(s"struct $helper").bracedEnd(";") {
        w.wl(s"using CppType = ${cppMarshal.fqTypename(ident, r)};")
        w.wl(s"static djinni::swift::AnyValue fromCpp(const CppType& c);")
        w.wl(s"static CppType toCpp(const djinni::swift::AnyValue& s);")
      }
    })
    writeSwiftCppFile(ident, origin, refs.swiftCpp, w => {
      w.w(s"djinni::swift::AnyValue ${helper}::fromCpp(const ${cppMarshal.fqTypename(ident, r)}& c)").braced {
        w.wl("auto ret = std::make_shared<djinni::swift::CompositeValue>();")
        for (f <- r.fields) {
          val member = s"c.${idCpp.field(f.ident)}"
          w.wl(s"ret->addValue(${marshal.fromCpp(f.ty, member)});")
        }
        w.wl("return {ret};")
      }
      w.w(s"${cppMarshal.fqTypename(ident, r)} ${helper}::toCpp(const djinni::swift::AnyValue& s)").braced {
        w.wl("auto p = std::get<djinni::swift::CompositeValuePtr>(s);")
        val members = r.fields.view.zipWithIndex.map{case (f, i) => {
          val expr = s"p->getValue($i)"
          s"${marshal.toCpp(f.ty, expr)}"
        }}.mkString(", ")
        w.wl(s"return ${cppMarshal.fqTypename(ident, r)}($members);")
      }
    })
  }
  override def generateInterface(origin: String, ident: Ident, doc: Doc, typeParams: Seq[TypeParam], i: Interface) {
    val refs = new SwiftRefs(ident.name)
    i.methods.foreach(m => {
      m.params.foreach(p => refs.find(p.ty))
      m.ret.foreach(refs.find)
    })
    i.consts.foreach(c => {
      refs.find(c.ty)
    })
    val helper = marshal.helperClass(ident)
    val proxy = idSwift.ty(ident) + "SwiftProxy"
    writeSwiftHppFile(ident, origin, refs.swiftHpp, Nil, w => {
      w.wl(s"using ${spec.swiftxxClassIdentStyle(ident)} = djinni::swift::Interface<${cppMarshal.fqTypename(ident, i)}>;")
      if (i.ext.cpp) {
        w.wl
        i.methods.foreach(m => {
          if (m.static || m.lang.swift) {
            w.wl(s"djinni::swift::AnyValue ${idSwift.ty(ident)}_${idSwift.method(m.ident)}(const djinni::swift::ParameterList* params);")
          }
        })
      }
      if (i.ext.swift) {
        w.wl
        w.w(s"class $proxy: public ${cppMarshal.fqTypename(ident, i)}, public djinni::swift::ProtocolWrapper").bracedEnd(";") {
          w.wlOutdent("public:")
          w.wl(s"$proxy(void* instance, djinni::swift::DispatchFunc dispatcher): ProtocolWrapper(instance, dispatcher) {}")
          w.wl("static djinni::swift::AnyValue make(void* instance, djinni::swift::DispatchFunc dispatcher);")
          i.methods.foreach(m => {
            val ret = cppMarshal.fqReturnType(m.ret)
            val params = m.params.map(p => cppMarshal.fqParamType(p.ty) + " " + idCpp.local(p.ident))
            if (!m.static) {
              val constFlag = if (m.const) " const" else ""
              w.wl(s"$ret ${idCpp.method(m.ident)}${params.mkString("(", ", ", ")")}$constFlag override;")
            }
          })
        }
      }
    })
    writeSwiftCppFile(ident, origin, refs.swiftCpp, w => {
      if (i.ext.cpp) {
        i.methods.foreach(m => {
          if (m.static || m.lang.swift) {
            w.w(s"djinni::swift::AnyValue ${idSwift.ty(ident)}_${idSwift.method(m.ident)}(const djinni::swift::ParameterList* params)").braced {
              // get self
              if (!m.static) {
                w.wl(s"auto inst = ${marshal.helperClass(ident)}::toCpp(params->getValue(0));")
              }
              // get args
              val iOffset = if (m.static) 0 else 1
              for ((p, i) <- m.params.view.zipWithIndex) {
                val pi = s"params->getValue(${i + iOffset})"
                w.wl(s"auto _${idCpp.local(p.ident)} = ${marshal.toCpp(p.ty, pi)};")
              }
              // make the call
              if (!m.ret.isEmpty) {
                w.w("auto ret = ")
              }
              if (m.static) {
                w.w(s"${cppMarshal.fqTypename(ident, i)}::")
              } else {
                w.w("inst->")
              }
              val args = m.params.map(p => s"_${idCpp.local(p.ident)}").mkString(", ")
              w.wl(s"${idCpp.method(m.ident)}($args);")
              // return
              if (m.ret.isEmpty) {
                w.wl("return djinni::swift::makeVoidValue();")
              } else {
                w.wl("return " + marshal.fromCpp(m.ret.get, "ret") + ";")
              }
            }
          }
        })
      }
      if (i.ext.swift) {
        w.w(s"djinni::swift::AnyValue ${proxy}::make(void* instance, djinni::swift::DispatchFunc dispatcher)").braced {
          w.wl(s"return {std::make_shared<$proxy>(instance, dispatcher)};")
        }
        for ((m, idx) <- i.methods.filter(!_.static).view.zipWithIndex) {
          val ret = cppMarshal.fqReturnType(m.ret)
          val params = m.params.map(p => cppMarshal.fqParamType(p.ty) + " " + idCpp.local(p.ident))
          val constFlag = if (m.const) " const" else ""
          w.w(s"$ret $proxy::${idCpp.method(m.ident)}${params.mkString("(", ", ", ")")}$constFlag").braced {
            w.wl("djinni::swift::ParameterList params;")
            for (p <- m.params) {
              w.wl(s"params.addValue(${marshal.fromCpp(p.ty, idCpp.local(p.ident))});")
            }
            val call = s"callProtocol($idx, &params)"
            if (m.ret.isEmpty) {
              w.wl(call + ";")
            } else {
              w.wl("return " + marshal.toCpp(m.ret.get, call) + ";")
            }
          }
        }
      }
    })
  }
}
