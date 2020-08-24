#! /usr/bin/env bash
protoc --objc_out=objc --java_out=java --cpp_out=cpp test.proto
protoc --java_out=java --cpp_out=cpp test2.proto
