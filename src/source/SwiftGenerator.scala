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

class SwiftGenerator(spec: Spec) extends Generator(spec) {

  val marshal = new SwiftMarshal(spec)

  def writeSwiftFile(ident: String, origin: String, refs: Iterable[String], f: IndentWriter => Unit) {
    createFile(spec.swiftOutFolder.get, idSwift.ty(ident) + ".swift", (w: IndentWriter) => {
      w.wl("// AUTOGENERATED FILE - DO NOT MODIFY!")
      w.wl("// This file was generated by Djinni from " + origin)
      w.wl
      if (refs.nonEmpty) {
        refs.foreach(s => w.wl(s"import $s"))
        w.wl
      }
      f(w)
    })
  }
  def writeSwiftPrivateFile(ident: String, origin: String, refs: Iterable[String], f: IndentWriter => Unit) {
    createFile(spec.swiftOutFolder.get, idSwift.ty(ident) + "+Private.swift", (w: IndentWriter) => {
      w.wl("// AUTOGENERATED FILE - DO NOT MODIFY!")
      w.wl("// This file was generated by Djinni from " + origin)
      w.wl
      if (refs.nonEmpty) {
        refs.foreach(s => w.wl(s"import $s"))
        w.wl
      }
      f(w)
    })
  }

  def writeFlagNone(w: IndentWriter, e: Enum, ident: IdentConverter, t: String) {
    for (o <- e.options.find(_.specialFlag == Some(Enum.SpecialFlag.NoFlags))) {
      writeDoc(w, o.doc)
      w.wl(s"public static let ${ident(o.ident.name)}: $t = []")
    }
  }
  def writeFlags(w: IndentWriter, e: Enum, ident: IdentConverter, t: String) {
    var shift = 0
    for (o <- normalEnumOptions(e)) {
      writeDoc(w, o.doc)
      w.wl(s"public static let ${ident(o.ident.name)} = $t(rawValue: 1 << $shift)")
      shift += 1
    }
  }
  def writeFlagAll(w: IndentWriter, e: Enum, ident: IdentConverter, t: String) {
    for (
      o <- e.options.find(_.specialFlag.contains(Enum.SpecialFlag.AllFlags))
    ) {
      writeDoc(w, o.doc)
      val all = normalEnumOptions(e)
          .map{case o => "."+ident(o.ident.name)}
          .mkString(", ")
      w.w(s"public static let ${ident(o.ident.name)}: $t = [${all}]")
    }
  }

  override def generateEnum(origin: String, ident: Ident, doc: Doc, e: Enum) {
    writeSwiftFile(ident, origin, List[String](), w => {
      val t = marshal.typename(ident, e)
      if (e.flags) {
        w.w(s"public struct $t: OptionSet").braced {
          w.wl("public let rawValue: Int32")
          w.wl("public init(rawValue: Int32) { self.rawValue = rawValue }")
          writeFlagNone(w, e, idSwift.enum, t)
          writeFlags(w, e, idSwift.enum, t)
          writeFlagAll(w, e, idSwift.enum, t)
        }
      } else {
        w.w(s"public enum ${marshal.typename(ident, e)}: Int32").braced {
          writeEnumOptions(w, e, idSwift.enum, "=", "case ", "")
        }
      }
    })
    writeSwiftPrivateFile(ident, origin, List[String]("DjinniSupport", spec.swiftModule), w => {
      w.wl(s"public typealias ${marshal.typename(ident, e)}Marshaller = DjinniSupport.EnumMarshaller<${marshal.typename(ident, e)}>")
    })
  }

  // return the base type if tm is optional otherwise None
  private def optionalBase(tm: MExpr) : Option[MExpr] = {
    tm.base match {
      case MOptional => Some(tm.args.head)
      case _ => None
    }
  }

  private def swiftMethodName(ident: String) = {
    val reservedNames = Array("init", "deinit")
    val name = idSwift.method(ident)
    if (reservedNames.contains(name)) {
      "_" + name
    } else {
      name
    }
  }

  class SwiftRefs(name: String) {
    var swiftImports = mutable.TreeSet[String]()
    var privateImports = mutable.TreeSet[String]("DjinniSupport", "Foundation", spec.swiftxxBaseLibModule, spec.swiftModule, spec.swiftModule + "Cxx")
    swiftImports.add("Foundation")
    def find(ty: TypeRef) { find(ty.resolved) }
    def find(tm: MExpr) {
      tm.args.foreach(find)
      find(tm.base)
    }
    def find(m: Meta) = for(r <- marshal.references(m, name)) r match {
      // don't import empty module name (e.g. types defined in stdlib) or same module as the current one
      case ImportRef(arg) => if (arg.nonEmpty && arg != spec.swiftModule) { swiftImports.add(arg) }
      case PrivateImportRef(arg) => if (arg.nonEmpty && arg != spec.swiftModule) { privateImports.add(arg) }
      case _ =>
    }
  }

  def generateSwiftConstants(w: IndentWriter, consts: Seq[Const]) = {

    def writeSwiftConst(w: IndentWriter, ty: TypeRef, v: Any): Unit = v match {
      case l: Long => w.w(l.toString)
      case d: Double => w.w(d.toString)
      case b: Boolean => w.w(if (b) "true" else "false")
      case s: String => w.w(s)
      case e: EnumValue =>  w.w(s"${marshal.typename(ty)}.${idSwift.enum(e)}")
      case v: ConstRef => w.w(idSwift.const(v))
      case z: Map[_, _] => { // Value is record
        val recordMdef = ty.resolved.base.asInstanceOf[MDef]
        val record = recordMdef.body.asInstanceOf[Record]
        val vMap = z.asInstanceOf[Map[String, Any]]
        w.wl(s"${marshal.typename(ty)}(")
        w.increase()
        // Use exact sequence
        val skipFirst = SkipFirst()
        for (f <- record.fields) {
          skipFirst {w.wl(",")}
          w.w(idSwift.field(f.ident) + ":")
          writeSwiftConst(w, f.ty, vMap.apply(f.ident.name))
        }
        w.w(")")
        w.decrease()
      }
    }

    for (c <- consts) {
      writeDoc(w, c.doc)
      w.w(s"public static let ${idSwift.const(c.ident)}: ${marshal.fieldType(c.ty)} = ")
      writeSwiftConst(w, c.ty, c.value)
      w.wl
    }
  }

  def generateConformance(deriving: Set[Record.DerivingType.Value], prefix: String) = {
    val eq = if (deriving.contains(DerivingType.Eq)) "Equatable" else ""
    val hashable = if (deriving.contains(DerivingType.Hashable)) "Hashable" else ""
    val sendable = if (deriving.contains(DerivingType.Sendable)) "Sendable" else ""
    val codable = if (deriving.contains(DerivingType.Codable)) "Codable" else ""
    val error = if (deriving.contains(DerivingType.Error)) "Error" else ""
    val conformance = Array(eq, hashable, sendable, codable).filter(_ != "")
    if (conformance.nonEmpty) prefix + conformance.mkString(", ") else ""
  }

  override def generateRecord(origin: String, ident: Ident, doc: Doc, params: Seq[TypeParam], r: Record) {
    val refs = new SwiftRefs(ident.name)
    r.fields.foreach(f => refs.find(f.ty))
    writeSwiftFile(ident, origin, refs.swiftImports, w => {
      writeDoc(w, doc)
      w.w(s"public struct ${marshal.typename(ident, r)}${generateConformance(r.derivingTypes, ": ")}").braced {
        generateSwiftConstants(w, r.consts)
        for (f <- r.fields) {
          writeDoc(w, f.doc)
          w.wl(s"public var ${idSwift.field(f.ident)}: ${marshal.fqFieldType(f.ty)}")
        }
        val initParams = r.fields.map(f => s"${idSwift.field(f.ident)}: ${marshal.fqFieldType(f.ty)}").mkString(", ")
        w.wl
        w.wl(s"public init(${initParams})").braced {
          for (f <- r.fields) {
            w.wl(s"self.${idSwift.field(f.ident)} = ${idSwift.field(f.ident)}")
          }
        }
      }
    })
    writeSwiftPrivateFile(ident, origin, refs.privateImports, w => {
      w.w(s"public enum ${marshal.typename(ident, r)}Marshaller: DjinniSupport.Marshaller").braced {
        w.wl(s"public typealias SwiftType = ${marshal.fqTypename(ident, r)}")
        w.w("public static func fromCpp(_ c: djinni.swift.AnyValue) -> SwiftType").braced {
          w.wl("return withUnsafePointer(to: c) { p in").nested {
            for ((f, i) <- r.fields.view.zipWithIndex) {
              val swiftExp = s"djinni.swift.getMember(p, $i)"
              w.wl(s"let ${idSwift.field(f.ident)} = ${marshal.fromCpp(f.ty, swiftExp)}")
            }
            val members = r.fields.map(f => s"${idSwift.field(f.ident)}: ${idSwift.field(f.ident)}").mkString(", ")
            w.wl(s"return SwiftType(${members})")
          }
          w.wl("}")
        }
        w.w("public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue").braced {
          if (r.fields.nonEmpty) {
            w.wl("var ret = djinni.swift.makeCompositeValue()")
            for (f <- r.fields) {
              val swiftExpr = s"s.${idSwift.field(f.ident)}"
              w.wl(s"djinni.swift.addMember(&ret, ${marshal.toCpp(f.ty,swiftExpr )})")
            }
            w.wl("return ret")
          } else {
            w.wl("return djinni.swift.makeCompositeValue()")
          }
        }
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
    writeSwiftFile(ident, origin, refs.swiftImports, w => {
      writeDoc(w, doc)
      w.w(s"public protocol ${marshal.typename(ident, i)}: AnyObject").braced {
        for (m <- i.methods.filter(!_.static)) {
          writeMethodDoc(w, m, idSwift.local)
          w.w(s"func ${swiftMethodName(m.ident)}(")
          // skip label for the first parameter
          if (m.params.nonEmpty) { w.w("_ ") }
          w.w(m.params.map(p => s"${idSwift.local(p.ident)}: ${marshal.fqParamType(p.ty)}").mkString(", "))
          w.wl(s") throws -> ${marshal.fqReturnType(m.ret)}")
        }
      }
    })
    writeSwiftPrivateFile(ident, origin, refs.privateImports, w => {
      writeDoc(w, doc)
      // Define CppProxy class if interface is implemented in C++
      if (i.ext.cpp) {
        w.w(s"final class ${marshal.typename(ident, i)}CppProxy: DjinniSupport.CppProxy, ${marshal.fqTypename(ident, i)}").braced {
          w.wl("init(_ inst: djinni.swift.AnyValue) { super.init(inst:inst) } ")
          for (m <- i.methods.filter(!_.static)) {
            w.w(s"func ${swiftMethodName(m.ident)}(")
            if (m.params.nonEmpty) { w.w("_ ") }
            w.w(m.params.map(p => s"${idSwift.local(p.ident)}: ${marshal.fqParamType(p.ty)}").mkString(", "))
            w.w(s") throws -> ${marshal.fqReturnType(m.ret)}").braced {
              w.wl("var _params = djinni.swift.ParameterList()")
              w.wl("_params.addValue(inst)")
              for (p <- m.params) {
                w.wl(s"_params.addValue(${marshal.toCpp(p.ty, idSwift.local(p.ident))})")
              }
              w.wl(s"var ret = ${spec.swiftxxNamespace}.${marshal.typename(ident, i)}_${idSwift.method(m.ident)}(&_params)")
              w.wl("try handleCppErrors(&ret)")
              if (!m.ret.isEmpty) {
                w.wl(s"return ${marshal.fromCpp(m.ret.get, "ret")}")
              }
            }
          }
        }
      }
      // Define the vtbl for protocol wrapper if interface is implemented in Swift
      val swiftProxyClassName = s"${marshal.typename(ident, i)}SwiftProxy"
      val swiftProxyVtbl = s"${idSwift.local(ident)}Methods"
      if (i.ext.swift) {
        w.wl(s"let $swiftProxyVtbl: Vtbl<${marshal.typename(ident, i)}> = [").nested {
          for (m <- i.methods.filter(!_.static)) {
            w.wl("{ inst, params, ret in").nested {
              for ((p, i) <- m.params.view.zipWithIndex) {
                val pi = s"djinni.swift.getMember(params, $i)"
                w.wl(s"let _${idSwift.local(p.ident)} = ${marshal.fromCpp(p.ty, pi)}")
              }
              val args = m.params.view.zipWithIndex.map{case (p, i) =>
                val label = if (i==0) "" else s"${idSwift.local(p.ident)}: "
                label + s"_${idSwift.local(p.ident)}"
              }.mkString(", ")
              val call = s"inst.${swiftMethodName(m.ident)}(${args})"
              if (m.ret.isEmpty) {
                w.wl("try " + call)
              } else {
                w.wl(s"djinni.swift.setReturnValue(ret, try ${marshal.toCpp(m.ret.get, call)})")
              }
            }
            w.wl("},")
          }
        }
        w.wl("]")
        w.wl
      }
      // Define the marshaller
      w.w(s"public enum ${marshal.typename(ident, i)}Marshaller: DjinniSupport.Marshaller").braced {
        w.wl(s"public typealias SwiftType = ${marshal.fqTypename(ident, i)}")
        w.w("public static func fromCpp(_ c: djinni.swift.AnyValue) -> SwiftType").braced {
          val newProxyBlock = if (i.ext.cpp) {s"{ ${marshal.typename(ident, i)}CppProxy(c) as SwiftType }"} else {"{ fatalError(\"n/a\") }"}
          w.wl(s"return cppInterfaceToSwift(c, ${newProxyBlock})")
        }
        w.w("public static func toCpp(_ s: SwiftType) -> djinni.swift.AnyValue").braced {
          val newProxyBlock = if (i.ext.swift) {s"{ ${spec.swiftxxNamespace}.$swiftProxyClassName.make(ctxPtr(s, ${swiftProxyVtbl}), dispatcherProtocalCall)}"} else {"{ fatalError(\"n/a\") }"}
          w.wl(s"return swiftInterfaceToCpp(s, ${newProxyBlock})")
        }
      }
      // Define static method stubs
      val staticMethods = i.methods.filter(m => m.static && m.lang.swift)
      if (!staticMethods.isEmpty) {
        w.w(s"public class ${marshal.typename(ident, i)}_statics").braced {
          for (m <- staticMethods) {
            w.w(s"public static func ${swiftMethodName(m.ident)}(")
            if (m.params.nonEmpty) { w.w("_ ") }
            w.w(m.params.map(p => s"${idSwift.local(p.ident)}: ${marshal.fqParamType(p.ty)}").mkString(", "))
            w.w(s") throws -> ${marshal.fqReturnType(m.ret)}").braced {
              w.wl("var _params = djinni.swift.ParameterList()")
              for (p <- m.params) {
                w.wl(s"_params.addValue(${marshal.toCpp(p.ty, idSwift.local(p.ident))})")
              }
              w.wl(s"var ret = ${spec.swiftxxNamespace}.${marshal.typename(ident, i)}_${idSwift.method(m.ident)}(&_params)")
              w.wl("try handleCppErrors(&ret)")
              if (!m.ret.isEmpty) {
                w.wl(s"return ${marshal.fromCpp(m.ret.get, "ret")}")
              }
            }
          }
        }
      }
    })
  }
}
