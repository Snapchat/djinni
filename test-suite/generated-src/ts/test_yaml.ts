// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from yaml-test.djinni

import { ClientInterface, ClientReturnedRecord, Color, RecordWithDerivings, SampleInterface, TestHelpers } from "test"

/** This file tests YAML dumped by Djinni can be parsed back in */
export interface /*record*/ ExternRecordWithDerivings {
    member: RecordWithDerivings;
    e: Color;
}

export interface ExternInterface1 {
    foo(i: ClientInterface): ClientReturnedRecord;
    bar(e: Color): Color;
}

export interface ExternInterface2 {
    foo(i: TestHelpers): ExternRecordWithDerivings;
}

export interface /*record*/ TestOptionalExternInterfaceRecord {
    sampleInterface: SampleInterface;
}

export interface TestYaml_statics {
}
