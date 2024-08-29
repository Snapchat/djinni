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

import djinni.ast._
import djinni.generatorTools._
import djinni.meta._

class SwiftMarshal(spec: Spec) extends Marshal(spec) {
  override def typename(tm: MExpr): String = toSwiftType(tm, None)
  def typename(name: String, ty: TypeDef): String = idSwift.ty(name)

  override def fqTypename(tm: MExpr): String = toSwiftType(tm, Some(spec.swiftModule))
  def fqTypename(name: String, ty: TypeDef): String = withPackage(Some(spec.swiftModule), idSwift.ty(name))

  override def paramType(tm: MExpr): String = toSwiftType(tm, None)
  override def fqParamType(tm: MExpr): String = toSwiftType(tm, Some(spec.swiftModule))

  override def returnType(ret: Option[TypeRef]): String = ret.fold("Void")(ty => toSwiftType(ty.resolved, None))
  override def fqReturnType(ret: Option[TypeRef]): String = ret.fold("Void")(ty => toSwiftType(ty.resolved, Some(spec.swiftModule)))

  override def fieldType(tm: MExpr): String = toSwiftType(tm, None)
  override def fqFieldType(tm: MExpr): String = toSwiftType(tm, Some(spec.swiftModule))

  override def toCpp(tm: MExpr, expr: String): String = s"${helperClass(tm)}.toCpp(${expr})"
  override def fromCpp(tm: MExpr, expr: String): String = s"${helperClass(tm)}.fromCpp(${expr})"

  private def withPackage(packageName: Option[String], t: String) = packageName.fold(t)(_ + "." + t)

  def references(m: Meta, exclude: String = ""): Seq[SymbolReference] = m match {
    case MProtobuf(name, _, ProtobufMessage(_,_,_,_,Some(swift))) => List(ImportRef(swift.module), PrivateImportRef(swift.module))
    case d: MDef => List()
    case e: MExtern => List(ImportRef(e.swift.module), PrivateImportRef(e.swift.translatorModule))
    case _ => List()
  }

  private def toSwiftType(tm: MExpr, packageName: Option[String]): String = {
    def args(tm: MExpr) = if (tm.args.isEmpty) "" else tm.args.map(f(_)).mkString("<", ", ", ">")
    def f(tm: MExpr): String = {
      tm.base match {
        case MOptional =>
          assert(tm.args.size == 1)
          val arg = tm.args.head
          arg.base match {
            case MOptional => throw new AssertionError("nested optional?")
            case m => s"Optional<${f(arg)}>"
          }
        case e: MExtern => e.swift.module + "." + e.swift.typename + (if (e.swift.generic) args(tm) else "")
        case p: MProtobuf => p.body.swift match {
          case Some(o) => o.prefix + p.name
          case None => p.name
        }
        case o =>
          val base = o match {
            case p: MPrimitive => swiftPrimitiveType(p)
            case MString => "String"
            case MDate => "Date"
            case MBinary => "Data"
            case MOptional => throw new AssertionError("optional should have been special cased")
            case MList => "Array"
            case MArray => "Array"
            case MSet => "Set"
            case MMap => "Dictionary"
            case d: MDef => withPackage(packageName, idSwift.ty(d.name))
            case e: MExtern => throw new AssertionError("unreachable")
            case e: MProtobuf => throw new AssertionError("unreachable")
            case p: MParam => idSwift.typeParam(p.name)
            case MVoid => "Void"
          }
          base + args(tm)
      }
    }
    f(tm)
  }

  private def swiftPrimitiveType(p: MPrimitive): String = p._idlName match {
    case "bool" => "Bool"
    case "i64" => "Int64"
    case "i32" => "Int32"
    case "i16" => "Int16"
    case "i8" => "Int8"
    case "f32" => "Float"
    case "f64" => "Double"
    case _ => throw new AssertionError("unknown primitive type: " + p._idlName)
  }

  def helperClass(name: String) = idSwift.ty(name) + "Marshaller" // TODO: make naming configurable
  private def helperClass(tm: MExpr): String = helperName(tm) + helperTemplates(tm)
  def helperName(tm: MExpr): String = tm.base match {
    case d: MDef => helperClass(d.name)
    case e: MExtern => e.swift.translator
    case o => o match {
      case p: MPrimitive => p.idlName match {
        case "i8" => "I8Marshaller"
        case "i16" => "I16Marshaller"
        case "i32" => "I32Marshaller"
        case "i64" => "I64Marshaller"
        case "f32" => "F32Marshaller"
        case "f64" => "F64Marshaller"
        case "bool" => "BoolMarshaller"
      }
      case MOptional => "OptionalMarshaller"
      case MBinary => "BinaryMarshaller"
      case MString => "StringMarshaller"
      case MDate => "DateMarshaller"
      case MList => "ListMarshaller"
      case MSet => "SetMarshaller"
      case MMap => "MapMarshaller"
      case p: MProtobuf => p.body.swift match {
        case Some(o) => "ProtobufMarshaller<" + o.prefix + p.name + ">"
        case None => s"[invalid protobuf: ${p.name}]"
      }
      case MArray => "ArrayMarshaller"
      case d: MDef => throw new AssertionError("unreachable")
      case e: MExtern => throw new AssertionError("unreachable")
      case p: MParam => throw new AssertionError("not applicable")
      case MVoid => "Void_Marshaller"
    }
  }
  private def helperTemplates(tm: MExpr): String = {
    def f() = if(tm.args.isEmpty) "" else tm.args.map(helperClass).mkString("<", ", ", ">")
    tm.base match {
      case MOptional | MList | MSet | MArray =>
        assert(tm.args.size == 1)
        f
      case _ => f
    }
  }
}
