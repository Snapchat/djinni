export interface DjinniModule {
    allocateWasmBuffer(size: number): Uint8Array;
    getExceptionMessage(ex: number): string;
    registerProtobufLib(name: string, proto: any): void;
}
