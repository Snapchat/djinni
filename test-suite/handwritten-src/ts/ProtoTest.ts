import {TestCase, allTests, assertArrayEq, assertEq} from "./testutils"
import * as test from "../../generated-src/ts/test";
import * as prototest from "../../djinni/vendor/third-party/proto/ts/test"
import {DjinniModule} from "@djinni_support/DjinniModule"
import { Writer } from "protobufjs/minimal";

export class ProtoTest extends TestCase {
    m: test.Test_statics;
    constructor(module: test.Test_statics & DjinniModule) {
        super(module);
        this.m = module;
        // register protobuf types with wasm module
        module.registerProtobufLib("prototest", prototest);
        // optional: override buffer allocator, make it avialble to wasm without
        // copying.
        Writer.alloc = function(size) {
            return module.allocateWasmBuffer(size);
        }
    }

    testJsToNative() {
        const p1 = {name: "tom", id: 1, email:"", phones:[]};
        const p2 = {name: "jerry", id: 2, email:"", phones:[]};
        const proto = {people: [p1, p2]};
        const r = this.m.testsuite.ProtoTests.protoToStrings(proto);
        assertArrayEq(r, ["tom", "jerry"]);
    }

    testNativeToJs() {
        const names = ["tom", "jerry"];
        const proto = this.m.testsuite.ProtoTests.stringsToProto(names);
        assertEq(proto.people[0].name, 'tom');
        assertEq(proto.people[1].name, 'jerry');
    }

    testEmbeddedProto() {
        const p = {name: "tom", id: 1, email:"", phones:[]};
        var rec = {person: p};
        const s = this.m.testsuite.ProtoTests.embeddedProtoToString(rec);
        assertEq(s, p.name);
        rec = this.m.testsuite.ProtoTests.stringToEmbeddedProto(s);
        assertEq(s, rec.person.name);
    }

    testBoxing() {
        const p1 = {name: "tom", id: 1, email:"", phones:[]};
        const p2 = {name: "jerry", id: 2, email:"", phones:[]};
        var plist:prototest.Person[] = [p1, p2];
        const slist = this.m.testsuite.ProtoTests.protoListToStrings(plist);
        assertArrayEq(slist, ['tom', 'jerry']);

        var plist = this.m.testsuite.ProtoTests.stringsToProtoList(slist);
        assertEq(plist[0].name, 'tom');
        assertEq(plist[1].name, 'jerry');

        const s = this.m.testsuite.ProtoTests.optionalProtoToString(p1);
        assertEq(s, 'tom');
        const p = this.m.testsuite.ProtoTests.stringToOptionalProto('tom');
        assertEq(p.name, 'tom');

        const r = this.m.testsuite.ProtoTests.stringToProtoOutcome('tom');
        assertEq(r, {result: {name: 'tom', id: 1, email:'', phones:[]}});
    }
}

allTests.push(ProtoTest);
