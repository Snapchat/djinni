#! /usr/bin/env bash
protoc --plugin=$(which protoc-gen-ts_proto) --objc_out=objc --java_out=java --cpp_out=cpp --ts_proto_out=ts test.proto
protoc --plugin=$(which protoc-gen-ts_proto) --java_out=java --cpp_out=cpp --ts_proto_out=ts test2.proto
