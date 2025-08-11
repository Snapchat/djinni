package djinni

import generatorTools.Spec

import djinni.ast.TypeRef

class CMarshal(spec: Spec) extends Marshal(spec) {

  override def typename(tm: meta.MExpr): String = {
    return fqTypename(tm)
  }

  override def fqTypename(tm: meta.MExpr): String = {
    throw new RuntimeException("Not implemented")
  }

  override def paramType(tm: meta.MExpr): String = {
    return fqParamType(tm)
  }

  override def fqParamType(tm: meta.MExpr): String = {
    throw new RuntimeException("Not implemented")
  }

  override def returnType(ret: Option[TypeRef]): String = {
    return fqReturnType(ret)
  }

  override def fqReturnType(ret: Option[TypeRef]): String = {
    throw new RuntimeException("Not implemented")
  }

  override def fieldType(tm: meta.MExpr): String = {
    throw new RuntimeException("Unapplicable")
  }

  override def fqFieldType(tm: meta.MExpr): String = {
    throw new RuntimeException("Unapplicable")
  }
}
