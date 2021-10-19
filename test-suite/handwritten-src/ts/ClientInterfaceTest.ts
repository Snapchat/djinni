import {TestCase, allTests, assertEq} from "./testutils"
import * as test from "../../generated-src/ts/test";

class ClientInterfaceImpl implements test.ClientInterface {
    getRecord(id, utf8String, misc) {
        if (utf8String != "Non-ASCII /\0 非 ASCII 字符" && utf8String != "Hello World!") {
            throw new Error("Unexpected string. Check UTF-8?");
        }
        return {recordId: id,
                content: utf8String,
                misc: misc};
    }
    identifierCheck(data, r, jret) {
        return 0.0;
    }
    returnStr() {
        return "test";
    }
    methTakingInterface(i) {
        if (i != null) {return i.returnStr();} else {return "";}
    }
    methTakingOptionalInterface(i) {
        if (i != null) {return i.returnStr();} else {return "";}
    }
}

class ClientInterfaceTest extends TestCase {
    m: test.Test_statics;
    jsClientInterface: test.ClientInterface;
    constructor(module) {
        super(module);
        this.m = <test.Test_statics>module;
    }
    setUp() {
        this.jsClientInterface = new ClientInterfaceImpl();
    }
    testClientReturn() {
        this.m.TestHelpers.checkClientInterfaceAscii(this.jsClientInterface);
    }

    testClientReturnUTF8() {
        this.m.TestHelpers.checkClientInterfaceNonascii(this.jsClientInterface);
    }

    testClientInterfaceArgs() {
        this.m.TestHelpers.checkClientInterfaceArgs(this.jsClientInterface);
    }

    testReverseClientInterfaceArgs() {
        var i = this.m.ReverseClientInterface.create();
        assertEq(i.methTakingInterface(i), "test");
        assertEq(i.methTakingOptionalInterface(i), "test");
    }
}

allTests.push(ClientInterfaceTest);
