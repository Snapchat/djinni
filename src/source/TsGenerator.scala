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

import java.io._
import djinni.ast.Record.DerivingType
import djinni.ast._
import djinni.generatorTools._
import djinni.meta._
import djinni.writer.IndentWriter
import scala.collection.mutable.ListBuffer
import scala.collection.mutable.TreeSet

class TsGenerator(spec: Spec) extends Generator(spec) {
  private def tsRetType(m: Interface.Method): String = {
    return if (m.ret.isEmpty) "void" else toTsType(m.ret.get.resolved)
  }

  private def tsPrimitiveType(p: MPrimitive): String = p._idlName match {
    case "bool" => "boolean"
    case "i64" => "bigint"
    case _ => "number"
  }

  private def tsArrayType(tm: MExpr): String = tm.base match {
    case p: MPrimitive => p._idlName match {
      case "i8" => "Int8Array"
      case "i16" => "Int16Array"
      case "i32" => "Int32Array"
      case "i64" => "BigInt64Array"
      case "f32" => "Float32Array"
      case "f64" => "Float64Array"
      case _ => "Array<" + tsPrimitiveType(p) + ">"
    }
    case _ => "Array<" + toTsType(tm) + ">"
  }

  // return the base type if tm is optional otherwise None
  private def optionalBase(tm: MExpr) : Option[MExpr] = {
    tm.base match {
      case MOptional => Some(tm.args.head)
      case _ => None
    }
  }

  private def removeOptional(tm: MExpr) : MExpr = {
    tm.base match {
      case MOptional => tm.args.head
      case _ => tm
    }
  }

  def toTsType(tm: MExpr): String = {
    def args(tm: MExpr) = if (tm.args.isEmpty) "" else tm.args.map(f).mkString("<", ", ", ">")
    def f(tm: MExpr): String = {
      tm.base match {
        case MOptional =>
          assert(tm.args.size == 1)
          val arg = tm.args.head
          arg.base match {
            case MOptional => throw new AssertionError("nested optional?")
            case m => f(arg) + " | undefined"
          }
        case MArray => tsArrayType(tm.args.head)
        case e: MExtern => if (e.ts.generic) e.ts.typename + args(tm) else e.ts.typename
        case p: MProtobuf => p.name
        case o =>
          val base = o match {
            case p: MPrimitive => tsPrimitiveType(p)
            case MString => "string"
            case MDate => "Date"
            case MBinary => "Uint8Array"
            case MOptional => throw new AssertionError("optional should have been special cased")
            case MList => "Array"
            case MSet => "Set"
            case MMap => "Map"
            case MArray => throw new AssertionError("array should have been special cased")
            case d: MDef => idJs.ty(d.name)
            case e: MExtern => throw new AssertionError("unreachable")
            case e: MProtobuf => throw new AssertionError("unreachable")
            case p: MParam => idJs.typeParam(p.name)
          }
          base + args(tm)
      }
    }
    f(tm)
  }

  case class TsSymbolRef(sym: String, module: String)
  def references(m: Meta): Seq[TsSymbolRef] = m match {
    case e: MExtern => List(TsSymbolRef(idJs.ty(e.name), e.ts.module))
    case MProtobuf(name, _, ProtobufMessage(_,_,_,Some(ts))) => List(TsSymbolRef(name, ts.module))
    case _ => List()
  }
  class TsRefs() {
    var imports = scala.collection.mutable.Map[String, TreeSet[String]]()

    def find(ty: TypeRef) { find(ty.resolved) }
    def find(tm: MExpr) {
      tm.args.foreach(find)
      find(tm.base)
    }
    def find(m: Meta) = for(r <- references(m)) r match {
      case TsSymbolRef(sym, module) => {
        var syms = imports.getOrElseUpdate(module, TreeSet[String]())
        syms += (sym)
      }
      case _ =>
    }
  }

  private def generateTsConstants(w: IndentWriter, ident: Ident, consts: Seq[Const]) = {
    def writeJsConst(w: IndentWriter, ty: TypeRef, v: Any): Unit = v match {
      case l: Long if (toTsType(removeOptional(ty.resolved)) == "bigint") => w.w(s"""BigInt("${l.toString}")""")
      case l: Long => w.w(l.toString)
      case d: Double => w.w(d.toString)
      case b: Boolean => w.w(if (b) "true" else "false")
      case s: String => w.w(s)
      case e: EnumValue => w.w(s"${idJs.ty(ty.expr.ident)}.${idJs.enum(e)}")
      case v: ConstRef => w.w(s"${idJs.const(v)}")
      case z: Any => { // Value is record
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
    w.w(s"export namespace ${idJs.ty(ident)}").braced {
      for (c <- consts) {
        writeDoc(w, c.doc)
        w.w(s"export const ${idJs.const(c.ident)} = ")
        writeJsConst(w, c.ty, c.value)
        w.wl(";")
      }
    }
  }

  //------------------------------------------------------------------------
  private def generateEnum(origin: String, ident: Ident, doc: Doc, e: Enum, w: IndentWriter) {
    w.wl
    writeDoc(w, doc)
    w.w(s"export enum ${idJs.ty(ident)}").braced {
      writeEnumOptionNone(w, e, idJs.enum, "=")
      writeEnumOptions(w, e, idJs.enum, "=")
      writeEnumOptionAll(w, e, idJs.enum, "=")
    }
  }
  private def generateRecord(origin: String, ident: Ident, doc: Doc, params: Seq[TypeParam], r: Record, w: IndentWriter) {
    w.wl
    writeDoc(w, doc)
    w.w(s"export interface /*record*/ ${idJs.ty(ident)}").braced {
      for (f <- r.fields) {
        writeDoc(w, f.doc)
        optionalBase(f.ty.resolved) match {
          case Some(t) => w.wl(s"${idJs.field(f.ident)}?: ${toTsType(t)};")
          case _ => w.wl(s"${idJs.field(f.ident)}: ${toTsType(f.ty.resolved)};")
        }
      }
    }
    if (!r.consts.isEmpty) {
      generateTsConstants(w, ident, r.consts);
    }
  }
  private def generateInterface(origin: String, ident: Ident, doc: Doc, typeParams: Seq[TypeParam], i: Interface, w: IndentWriter) {
    w.wl
    writeDoc(w, doc)
    w.w(s"export interface ${idJs.ty(ident)}").braced {
      for (m <- i.methods.filter(!_.static)) {
        writeMethodDoc(w, m, idJs.local)
        w.w(s"${idJs.method(m.ident)}(")
        w.w(m.params.map(p => s"${idJs.local(p.ident)}: ${toTsType(p.ty.resolved)}").mkString(", "))
        w.wl(s"): ${tsRetType(m)};")
      }
    }
    if (!i.consts.isEmpty) {
      generateTsConstants(w, ident, i.consts);
    }
    val staticMethods = i.methods.filter(_.static)
    if (!staticMethods.isEmpty) {
      w.w(s"export interface ${idJs.ty(ident)}_statics").braced {
        for (m <- staticMethods) {
          writeMethodDoc(w, m, idJs.local)
          w.w(s"${idJs.method(m.ident)}(")
          w.w(m.params.map(p => s"${idJs.method(p.ident)}: ${toTsType(p.ty.resolved)}").mkString(", "))
          w.wl(s"): ${tsRetType(m)};")
        }
      }
    }
  }
  //--------------------------------------------------------------------------
  override def generate(idl: Seq[TypeDecl]) {
    createFile(spec.tsOutFolder.get, spec.tsModule + ".ts", (w: IndentWriter) => {
      w.wl("// AUTOGENERATED FILE - DO NOT MODIFY!")
      w.wl("// This file generated by Djinni from " + spec.moduleName + ".djinni")
      w.wl
      val decls = idl.collect { case itd: InternTypeDecl => itd }

      // find external references
      val refs = new TsRefs()
      for (td <- decls) td.body match {
        case r: Record => {
          r.fields.foreach(f => refs.find(f.ty))
          r.consts.foreach(c => refs.find(c.ty))
        }
        case i: Interface => {
          i.methods.foreach(m => {
            m.params.foreach(p => refs.find(p.ty))
            m.ret.foreach(refs.find)
          })
          i.consts.foreach(c => refs.find(c.ty))
        }
        case _ =>
      }
      // write external references
      for ((module, syms) <- refs.imports) {
        if (module != "") {
          w.wl(s"""import { ${syms.mkString(", ")} } from "$module"""")
        }
      }

      var interfacesWithStatics = new ListBuffer[String]()
      for (td <- decls) td.body match {
        case e: Enum => generateEnum(td.origin, td.ident, td.doc, e, w)
        case r: Record => generateRecord(td.origin, td.ident, td.doc, td.params, r, w)
        case i: Interface => {
          generateInterface(td.origin, td.ident, td.doc, td.params, i, w)
          if (i.methods.exists(_.static)) {
            interfacesWithStatics += idJs.ty(td.ident.name)
          }
        }
        case _ =>
      }
      // add static factories
      w.wl
      w.w(s"export interface ${idJs.ty(spec.tsModule)}_statics").braced {
        for (i <- interfacesWithStatics.toList) {
          w.wl(i + ": " + i + "_statics;")
        }
      }
    })
  }
  override def generateEnum(origin: String, ident: Ident, doc: Doc, e: Enum) {}
  override def generateRecord(origin: String, ident: Ident, doc: Doc, params: Seq[TypeParam], r: Record) {}
  override def generateInterface(origin: String, ident: Ident, doc: Doc, typeParams: Seq[TypeParam], i: Interface) {}
}
