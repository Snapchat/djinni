class ClientInterfaceImpl {
    getRecord(id, utf8String, misc) {
        if (utf8String != "Non-ASCII /\0 非 ASCII 字符" && utf8String != "Hello World!") {
        }
        return {mRecordId: id,
                mContent: utf8String,
                mMisc: misc};
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

class ClientInterfaceTest {
    constructor(module) {
        this.module = module;
    }
    setUp() {
        this.jsClientInterface = new ClientInterfaceImpl();
    }
    testClientReturn() {
        this.module.TestHelpers.checkClientInterfaceAscii(this.jsClientInterface);
    }

    testClientReturnUTF8() {
        this.module.TestHelpers.checkClientInterfaceNonascii(this.jsClientInterface);
    }

    testClientInterfaceArgs() {
        this.module.TestHelpers.checkClientInterfaceArgs(this.jsClientInterface);
    }

    testReverseClientInterfaceArgs() {
        var i = this.module.ReverseClientInterface.create();
        assertEq(i.methTakingInterface(i), "test");
        assertEq(i.methTakingOptionalInterface(i), "test");
    }
}
