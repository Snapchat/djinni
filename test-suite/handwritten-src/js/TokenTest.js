class JsToken {
    whoami() {
        return "Javascript";
    }
}

class TokenTest {
    constructor(module) {
        this.m = module;
    }
    
    testTokens() {
        var jt = new JsToken();
        assertSame(this.m.TestHelpers.tokenId(jt), jt);
    }

    testNullToken() {
        assertSame(this.m.TestHelpers.tokenId(null), null);
    }

    testCppToken() {
        var ct = this.m.TestHelpers.createCppToken();
        assertSame(this.m.TestHelpers.tokenId(ct), ct);
        this.m.TestHelpers.checkCppToken(ct);
        ct = null;
    }

    testTokenType() {
        this.m.TestHelpers.checkTokenType(new JsToken(), "Javascript");
        this.m.TestHelpers.checkTokenType(this.m.TestHelpers.createCppToken(), "C++");
        var threw = false;
        try {
            this.m.TestHelpers.checkTokenType(new JsToken(), "foo");
        } catch (e) {
            threw = true;
        }
        assertTrue(threw);
        threw = false;
        try {
            this.m.TestHelpers.checkTokenType(this.m.TestHelpers.createCppToken(), "foo");
        } catch (e) {
            threw = true;
        }
        assertTrue(threw);
    }

    testNotCppToken() {
        var threw = false;
        try {
            this.m.TestHelpers.checkCppToken(new JsToken());
        } catch (e) {
            threw = true;
        }
        assertTrue(threw);
    }
}

allTests.push(TokenTest);
