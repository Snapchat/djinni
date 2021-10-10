/* eslint-disable */
import { util, configure, Writer, Reader } from "protobufjs/minimal";
import * as Long from "long";

export const protobufPackage = "djinni.test2";

export interface PersistingState {
  userId: string;
  isActive: boolean;
}

const basePersistingState: object = { userId: "", isActive: false };

export const PersistingState = {
  encode(message: PersistingState, writer: Writer = Writer.create()): Writer {
    if (message.userId !== "") {
      writer.uint32(10).string(message.userId);
    }
    if (message.isActive === true) {
      writer.uint32(16).bool(message.isActive);
    }
    return writer;
  },

  decode(input: Reader | Uint8Array, length?: number): PersistingState {
    const reader = input instanceof Reader ? input : new Reader(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = { ...basePersistingState } as PersistingState;
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          message.userId = reader.string();
          break;
        case 2:
          message.isActive = reader.bool();
          break;
        default:
          reader.skipType(tag & 7);
          break;
      }
    }
    return message;
  },

  fromJSON(object: any): PersistingState {
    const message = { ...basePersistingState } as PersistingState;
    if (object.userId !== undefined && object.userId !== null) {
      message.userId = String(object.userId);
    } else {
      message.userId = "";
    }
    if (object.isActive !== undefined && object.isActive !== null) {
      message.isActive = Boolean(object.isActive);
    } else {
      message.isActive = false;
    }
    return message;
  },

  toJSON(message: PersistingState): unknown {
    const obj: any = {};
    message.userId !== undefined && (obj.userId = message.userId);
    message.isActive !== undefined && (obj.isActive = message.isActive);
    return obj;
  },

  fromPartial(object: DeepPartial<PersistingState>): PersistingState {
    const message = { ...basePersistingState } as PersistingState;
    if (object.userId !== undefined && object.userId !== null) {
      message.userId = object.userId;
    } else {
      message.userId = "";
    }
    if (object.isActive !== undefined && object.isActive !== null) {
      message.isActive = object.isActive;
    } else {
      message.isActive = false;
    }
    return message;
  },
};

type Builtin =
  | Date
  | Function
  | Uint8Array
  | string
  | number
  | boolean
  | undefined;
export type DeepPartial<T> = T extends Builtin
  ? T
  : T extends Array<infer U>
  ? Array<DeepPartial<U>>
  : T extends ReadonlyArray<infer U>
  ? ReadonlyArray<DeepPartial<U>>
  : T extends {}
  ? { [K in keyof T]?: DeepPartial<T[K]> }
  : Partial<T>;

// If you get a compile-error about 'Constructor<Long> and ... have no overlap',
// add '--ts_proto_opt=esModuleInterop=true' as a flag when calling 'protoc'.
if (util.Long !== Long) {
  util.Long = Long as any;
  configure();
}
