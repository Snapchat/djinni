import {TestCase, allTests, assertEq} from "./testutils"
import * as test from "../../generated-src/ts/test";

class ClientInterfaceImpl implements test.ClientInterface {
    getRecord(id: bigint, utf8String: string, misc: string | undefined) {
        if (utf8String != "Non-ASCII /\0 非 ASCII 字符" && utf8String != "Hello World!") {
            throw new Error("Unexpected string. Check UTF-8?");
        }
        return {recordId: id,
                content: utf8String,
                misc: misc};
    }
    identifierCheck(data: Uint8Array, r: number, jret: bigint) {
        return 0.0;
    }
    returnStr() {
        return "test";
    }
    methTakingInterface(i: test.ClientInterface | undefined) {
        if (i) {return i.returnStr();} else {return "";}
    }
    methTakingOptionalInterface(i: test.ClientInterface | undefined) {
        if (i) {return i.returnStr();} else {return "";}
    }
}

class ClientInterfaceTest extends TestCase {
    m: test.Test_statics;
    jsClientInterface: test.ClientInterface;
    constructor(module: test.Test_statics) {
        super(module);
        this.m = module;
        this.jsClientInterface = new ClientInterfaceImpl();
    }
    testClientReturn() {
        this.m.testsuite.TestHelpers.checkClientInterfaceAscii(this.jsClientInterface);
    }

    testClientReturnUTF8() {
        this.m.testsuite.TestHelpers.checkClientInterfaceNonascii(this.jsClientInterface);
    }

    testClientInterfaceArgs() {
        this.m.testsuite.TestHelpers.checkClientInterfaceArgs(this.jsClientInterface);
    }

    testReverseClientInterfaceArgs() {
        var i = this.m.testsuite.ReverseClientInterface.create();
        assertEq(i?.methTakingInterface(i), "test");
        assertEq(i?.methTakingOptionalInterface(i), "test");
    }
}

allTests.push(ClientInterfaceTest);
