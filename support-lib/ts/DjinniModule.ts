export interface DjinniModule {
    allocateWasmBuffer(size: number): Uint8Array;
    registerProtobufLib(name: string, proto: any): void;
}
