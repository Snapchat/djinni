// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from djinni_perf_benchmark.djinni


export enum EnumSixValue {
    FIRST = 0,
    SECOND = 1,
    THIRD = 2,
    FOURTH = 3,
    FIFTH = 4,
    SIXTH = 5,
}

export interface /*record*/ RecordSixInt {
    i1: bigint;
    i2: bigint;
    i3: bigint;
    i4: bigint;
    i5: bigint;
    i6: bigint;
}

/** interfaces for native C++ objects, to be returned from C++ */
export interface ObjectNative {
    baseline(): void;
}

/** interfaces for platform Java or Objective-C objects, to be passed to C++ */
export interface ObjectPlatform {
    onDone(): void;
}

/** djinni_perf_benchmark: This interface will be implemented in C++ and can be called from any language. */
export interface DjinniPerfBenchmark {
    cppTests(): bigint;
    baseline(): void;
    argString(s: string): void;
    argBinary(b: Uint8Array): void;
    argDataRef(r: Uint8Array): void;
    argDataView(d: Uint8Array): void;
    argEnumSixValue(e: EnumSixValue): void;
    argRecordSixInt(r: RecordSixInt): void;
    argListInt(v: Array<bigint>): void;
    argArrayInt(v: BigInt64Array): void;
    argObject(c: ObjectPlatform): void;
    argListObject(l: Array<ObjectPlatform>): void;
    argListRecord(l: Array<RecordSixInt>): void;
    argArrayRecord(a: Array<RecordSixInt>): void;
    returnInt(i: bigint): bigint;
    returnString(size: number): string;
    returnBinary(size: number): Uint8Array;
    returnObject(): ObjectNative;
    returnListInt(size: number): Array<bigint>;
    returnArrayInt(size: number): BigInt64Array;
    returnListObject(size: number): Array<ObjectNative>;
    returnListRecord(size: number): Array<RecordSixInt>;
    returnArrayRecord(size: number): Array<RecordSixInt>;
    roundTripString(s: string): string;
}
export interface DjinniPerfBenchmark_statics {
    getInstance(): DjinniPerfBenchmark;
}

export interface Perftest_statics {
    benchmark_DjinniPerfBenchmark: DjinniPerfBenchmark_statics;
}
