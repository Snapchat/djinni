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
        const p: prototest.Person = {name: "tom", id: 1, email:"", phones:[]};
        var rec: test.RecordWithEmbeddedProto = {person: p};
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
        assertEq(p?.name, 'tom');

        const r = this.m.testsuite.ProtoTests.stringToProtoOutcome('tom');
        assertEq(r, {result: {name: 'tom', id: 1, email:'', phones:[]}});
    }

    testProtobufEnum() {
        // Test enum conversion - should match Java/ObjC behavior
        const homeEnum = this.m.testsuite.ProtoTests.stringToPhoneType("HOME");
        assertEq(homeEnum, prototest.Person_PhoneType.HOME);
        
        const workEnum = this.m.testsuite.ProtoTests.stringToPhoneType("WORK");
        assertEq(workEnum, prototest.Person_PhoneType.WORK);
        
        const mobileEnum = this.m.testsuite.ProtoTests.stringToPhoneType("MOBILE");
        assertEq(mobileEnum, prototest.Person_PhoneType.MOBILE);
        
        // Test unknown string defaults to home (same as other platforms)
        const unknownEnum = this.m.testsuite.ProtoTests.stringToPhoneType("unknown");
        assertEq(unknownEnum, prototest.Person_PhoneType.HOME);
        
        // Test enum to string conversion
        const homeString = this.m.testsuite.ProtoTests.phoneTypeToString(prototest.Person_PhoneType.HOME);
        assertEq(homeString, "HOME");
        
        const workString = this.m.testsuite.ProtoTests.phoneTypeToString(prototest.Person_PhoneType.WORK);
        assertEq(workString, "WORK");
        
        const mobileString = this.m.testsuite.ProtoTests.phoneTypeToString(prototest.Person_PhoneType.MOBILE);
        assertEq(mobileString, "MOBILE");
    }

    testRecordWithProtobufEnum() {
        // Test record with protobuf enum
        const workRecord: test.RecordWithProtobufEnum = {
            phoneType: prototest.Person_PhoneType.WORK,
            priority: prototest.Priority.HIGH
        };
        const workString = this.m.testsuite.ProtoTests.enumRecordToString(workRecord);
        assertEq(workString, "WORK");
        
        const mobileRecord = this.m.testsuite.ProtoTests.stringToEnumRecord("MOBILE");
        assertEq(mobileRecord.phoneType, prototest.Person_PhoneType.MOBILE);
        
        const mobileString = this.m.testsuite.ProtoTests.enumRecordToString(mobileRecord);
        assertEq(mobileString, "MOBILE");
        
        // Test round-trip conversion
        const homeRecord: test.RecordWithProtobufEnum = {
            phoneType: prototest.Person_PhoneType.HOME,
            priority: prototest.Priority.LOW
        };
        const homeString = this.m.testsuite.ProtoTests.enumRecordToString(homeRecord);
        const reconvertedRecord = this.m.testsuite.ProtoTests.stringToEnumRecord(homeString);
        assertEq(reconvertedRecord.phoneType, prototest.Person_PhoneType.HOME);
    }

    testPriorityEnum() {
        // Test Priority enum conversion
        const lowPriority = this.m.testsuite.ProtoTests.stringToPriority("LOW");
        assertEq(lowPriority, prototest.Priority.LOW);
        
        const highPriority = this.m.testsuite.ProtoTests.stringToPriority("HIGH");
        assertEq(highPriority, prototest.Priority.HIGH);
        
        const urgentPriority = this.m.testsuite.ProtoTests.stringToPriority("URGENT");
        assertEq(urgentPriority, prototest.Priority.URGENT);
        
        // Test enum to string conversion
        const lowString = this.m.testsuite.ProtoTests.priorityToString(prototest.Priority.LOW);
        assertEq(lowString, "LOW");
        
        const highString = this.m.testsuite.ProtoTests.priorityToString(prototest.Priority.HIGH);
        assertEq(highString, "HIGH");
        
        const urgentString = this.m.testsuite.ProtoTests.priorityToString(prototest.Priority.URGENT);
        assertEq(urgentString, "URGENT");
    }
}

allTests.push(ProtoTest);
