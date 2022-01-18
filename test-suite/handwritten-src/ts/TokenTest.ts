import {TestCase, allTests, assertTrue, assertSame} from "./testutils"
import * as test from "../../generated-src/ts/test";

class JsToken {
    whoami() {
        return "Javascript";
    }
}

export class TokenTest extends TestCase {
    m: test.Test_statics;

    constructor(module: test.Test_statics) {
        super(module);
        this.m = module;
    }
    
    testTokens() {
        var jt = new JsToken();
        assertSame(this.m.testsuite.TestHelpers.tokenId(jt), jt);
    }

    testNullToken() {
        assertSame(this.m.testsuite.TestHelpers.tokenId(null), null);
    }

    testCppToken() {
        var ct = this.m.testsuite.TestHelpers.createCppToken();
        assertSame(this.m.testsuite.TestHelpers.tokenId(ct), ct);
        this.m.testsuite.TestHelpers.checkCppToken(ct);
        ct = null;
    }

    testTokenType() {
        this.m.testsuite.TestHelpers.checkTokenType(new JsToken(), "Javascript");
        this.m.testsuite.TestHelpers.checkTokenType(this.m.testsuite.TestHelpers.createCppToken(), "C++");
        var threw = false;
        try {
            this.m.testsuite.TestHelpers.checkTokenType(new JsToken(), "foo");
        } catch (e) {
            threw = true;
        }
        assertTrue(threw);
        threw = false;
        try {
            this.m.testsuite.TestHelpers.checkTokenType(this.m.testsuite.TestHelpers.createCppToken(), "foo");
        } catch (e) {
            threw = true;
        }
        assertTrue(threw);
    }

    testNotCppToken() {
        var threw = false;
        try {
            this.m.testsuite.TestHelpers.checkCppToken(new JsToken());
        } catch (e) {
            threw = true;
        }
        assertTrue(threw);
    }
}

allTests.push(TokenTest);
