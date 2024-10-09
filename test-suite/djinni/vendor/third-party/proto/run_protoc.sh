#! /usr/bin/env bash
# Swift codegen: with protoc-gen-swift (from https://github.com/apple/swift-protobuf) in path, add `--swift_out=swift --swift_opt=Visibility=public` to command line
protoc --plugin=$(which protoc-gen-ts_proto) --objc_out=objc --java_out=java --cpp_out=cpp --ts_proto_out=ts test.proto
protoc --plugin=$(which protoc-gen-ts_proto) --java_out=java --cpp_out=cpp --ts_proto_out=ts test2.proto
