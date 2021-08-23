package djinni

import djinni.ast.Record.DerivingType
import djinni.ast._
import djinni.generatorTools._
import djinni.meta._
import djinni.writer.IndentWriter

import scala.collection.mutable

class WasmGenerator(spec: Spec) extends Generator(spec) {

  override def generateModule(decls: Seq[InternTypeDecl]) {
    println("WasmGenerator.generateModule")
  }

  override def generateEnum(origin: String, ident: Ident, doc: Doc, e: Enum) {
    println("WasmGenerator.generateEnum")
  }

  override def generateInterface(origin: String, ident: Ident, doc: Doc, typeParams: Seq[TypeParam], i: Interface) {
    println("WasmGenerator.generateInterface")
  }

  override def generateRecord(origin: String, ident: Ident, doc: Doc, params: Seq[TypeParam], r: Record) {
    println("WasmGenerator.generateRecord")
  }
}