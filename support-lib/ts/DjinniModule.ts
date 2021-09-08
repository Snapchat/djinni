export interface DjinniModule {
    allocateWasmBuffer(size: number): Uint8Array;
}
