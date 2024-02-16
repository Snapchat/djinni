import { Arena } from 'composer_protobuf/src/Arena';

declare const global: any;

export function registerProtobufLib(name: string, protolib: any) {
  if (!('protoSupport' in global)) {
    global.protoSupport = {
      Arena: Arena,
      protoLibs: {},
    };
  }
  global.protoSupport.protoLibs[name] = protolib;
}
