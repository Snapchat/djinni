/* eslint-disable */
import { util, configure, Writer, Reader } from "protobufjs/minimal";
import * as Long from "long";

export const protobufPackage = "djinni.test";

export interface Person {
  name: string;
  id: number;
  email: string;
  phones: Person_PhoneNumber[];
}

export enum Person_PhoneType {
  MOBILE = 0,
  HOME = 1,
  WORK = 2,
  UNRECOGNIZED = -1,
}

export function person_PhoneTypeFromJSON(object: any): Person_PhoneType {
  switch (object) {
    case 0:
    case "MOBILE":
      return Person_PhoneType.MOBILE;
    case 1:
    case "HOME":
      return Person_PhoneType.HOME;
    case 2:
    case "WORK":
      return Person_PhoneType.WORK;
    case -1:
    case "UNRECOGNIZED":
    default:
      return Person_PhoneType.UNRECOGNIZED;
  }
}

export function person_PhoneTypeToJSON(object: Person_PhoneType): string {
  switch (object) {
    case Person_PhoneType.MOBILE:
      return "MOBILE";
    case Person_PhoneType.HOME:
      return "HOME";
    case Person_PhoneType.WORK:
      return "WORK";
    default:
      return "UNKNOWN";
  }
}

export interface Person_PhoneNumber {
  number: string;
  type: Person_PhoneType;
}

export interface AddressBook {
  people: Person[];
}

const basePerson: object = { name: "", id: 0, email: "" };

export const Person = {
  encode(message: Person, writer: Writer = Writer.create()): Writer {
    if (message.name !== "") {
      writer.uint32(10).string(message.name);
    }
    if (message.id !== 0) {
      writer.uint32(16).int32(message.id);
    }
    if (message.email !== "") {
      writer.uint32(26).string(message.email);
    }
    for (const v of message.phones) {
      Person_PhoneNumber.encode(v!, writer.uint32(34).fork()).ldelim();
    }
    return writer;
  },

  decode(input: Reader | Uint8Array, length?: number): Person {
    const reader = input instanceof Reader ? input : new Reader(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = { ...basePerson } as Person;
    message.phones = [];
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          message.name = reader.string();
          break;
        case 2:
          message.id = reader.int32();
          break;
        case 3:
          message.email = reader.string();
          break;
        case 4:
          message.phones.push(
            Person_PhoneNumber.decode(reader, reader.uint32())
          );
          break;
        default:
          reader.skipType(tag & 7);
          break;
      }
    }
    return message;
  },

  fromJSON(object: any): Person {
    const message = { ...basePerson } as Person;
    message.phones = [];
    if (object.name !== undefined && object.name !== null) {
      message.name = String(object.name);
    } else {
      message.name = "";
    }
    if (object.id !== undefined && object.id !== null) {
      message.id = Number(object.id);
    } else {
      message.id = 0;
    }
    if (object.email !== undefined && object.email !== null) {
      message.email = String(object.email);
    } else {
      message.email = "";
    }
    if (object.phones !== undefined && object.phones !== null) {
      for (const e of object.phones) {
        message.phones.push(Person_PhoneNumber.fromJSON(e));
      }
    }
    return message;
  },

  toJSON(message: Person): unknown {
    const obj: any = {};
    message.name !== undefined && (obj.name = message.name);
    message.id !== undefined && (obj.id = message.id);
    message.email !== undefined && (obj.email = message.email);
    if (message.phones) {
      obj.phones = message.phones.map((e) =>
        e ? Person_PhoneNumber.toJSON(e) : undefined
      );
    } else {
      obj.phones = [];
    }
    return obj;
  },

  fromPartial(object: DeepPartial<Person>): Person {
    const message = { ...basePerson } as Person;
    message.phones = [];
    if (object.name !== undefined && object.name !== null) {
      message.name = object.name;
    } else {
      message.name = "";
    }
    if (object.id !== undefined && object.id !== null) {
      message.id = object.id;
    } else {
      message.id = 0;
    }
    if (object.email !== undefined && object.email !== null) {
      message.email = object.email;
    } else {
      message.email = "";
    }
    if (object.phones !== undefined && object.phones !== null) {
      for (const e of object.phones) {
        message.phones.push(Person_PhoneNumber.fromPartial(e));
      }
    }
    return message;
  },
};

const basePerson_PhoneNumber: object = { number: "", type: 0 };

export const Person_PhoneNumber = {
  encode(
    message: Person_PhoneNumber,
    writer: Writer = Writer.create()
  ): Writer {
    if (message.number !== "") {
      writer.uint32(10).string(message.number);
    }
    if (message.type !== 0) {
      writer.uint32(16).int32(message.type);
    }
    return writer;
  },

  decode(input: Reader | Uint8Array, length?: number): Person_PhoneNumber {
    const reader = input instanceof Reader ? input : new Reader(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = { ...basePerson_PhoneNumber } as Person_PhoneNumber;
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          message.number = reader.string();
          break;
        case 2:
          message.type = reader.int32() as any;
          break;
        default:
          reader.skipType(tag & 7);
          break;
      }
    }
    return message;
  },

  fromJSON(object: any): Person_PhoneNumber {
    const message = { ...basePerson_PhoneNumber } as Person_PhoneNumber;
    if (object.number !== undefined && object.number !== null) {
      message.number = String(object.number);
    } else {
      message.number = "";
    }
    if (object.type !== undefined && object.type !== null) {
      message.type = person_PhoneTypeFromJSON(object.type);
    } else {
      message.type = 0;
    }
    return message;
  },

  toJSON(message: Person_PhoneNumber): unknown {
    const obj: any = {};
    message.number !== undefined && (obj.number = message.number);
    message.type !== undefined &&
      (obj.type = person_PhoneTypeToJSON(message.type));
    return obj;
  },

  fromPartial(object: DeepPartial<Person_PhoneNumber>): Person_PhoneNumber {
    const message = { ...basePerson_PhoneNumber } as Person_PhoneNumber;
    if (object.number !== undefined && object.number !== null) {
      message.number = object.number;
    } else {
      message.number = "";
    }
    if (object.type !== undefined && object.type !== null) {
      message.type = object.type;
    } else {
      message.type = 0;
    }
    return message;
  },
};

const baseAddressBook: object = {};

export const AddressBook = {
  encode(message: AddressBook, writer: Writer = Writer.create()): Writer {
    for (const v of message.people) {
      Person.encode(v!, writer.uint32(10).fork()).ldelim();
    }
    return writer;
  },

  decode(input: Reader | Uint8Array, length?: number): AddressBook {
    const reader = input instanceof Reader ? input : new Reader(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = { ...baseAddressBook } as AddressBook;
    message.people = [];
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          message.people.push(Person.decode(reader, reader.uint32()));
          break;
        default:
          reader.skipType(tag & 7);
          break;
      }
    }
    return message;
  },

  fromJSON(object: any): AddressBook {
    const message = { ...baseAddressBook } as AddressBook;
    message.people = [];
    if (object.people !== undefined && object.people !== null) {
      for (const e of object.people) {
        message.people.push(Person.fromJSON(e));
      }
    }
    return message;
  },

  toJSON(message: AddressBook): unknown {
    const obj: any = {};
    if (message.people) {
      obj.people = message.people.map((e) =>
        e ? Person.toJSON(e) : undefined
      );
    } else {
      obj.people = [];
    }
    return obj;
  },

  fromPartial(object: DeepPartial<AddressBook>): AddressBook {
    const message = { ...baseAddressBook } as AddressBook;
    message.people = [];
    if (object.people !== undefined && object.people !== null) {
      for (const e of object.people) {
        message.people.push(Person.fromPartial(e));
      }
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
