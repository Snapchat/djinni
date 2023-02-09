function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

async function asyncFunc() {
    await sleep(10);
    return 36;
}

async function asyncException() {
    await sleep(10);
    throw new Error("123");
}

class AsyncInterfaceImpl {
    async futureRoundtrip(f) {
        const i = await f;
        return i.toString();
    }
}

class AsyncTest {
    constructor(module) {
        this.module = module;
    }
    async testConsumeNativeFuture() {
        const r = await this.module.testsuite.TestHelpers.getAsyncResult();
        assertEq(r, 42);
    }

    async testConsumeNativeFutureAltSyntax() {
        return this.module.testsuite.TestHelpers.getAsyncResult().then(res => assertEq(res, 42)).catch(e => assertTrue(false));
    }

    async testFutureRoundtrip() {
        const s = await this.module.testsuite.TestHelpers.futureRoundtrip(asyncFunc());
        const r = parseInt(s);
        assertEq(r, 36);
    }

    async testVoidRoundTrip() {
        await this.m.testsuite.TestHelpers.voidAsyncMethod(sleep(10));
    }

    async testOptionalFutureUnsetValue() {
        assertUndefined(await this.m.testsuite.TestHelpers.addOneIfPresent(Promise.resolve(undefined)));
    }

    async testOptionalFutureSetValue() {
        assertEq(await this.m.testsuite.TestHelpers.addOneIfPresent(Promise.resolve(10)), 11);
    }

    async testFutureRoundtripWithException() {
        var s = null;
        try {
            const s = await this.module.testsuite.TestHelpers.futureRoundtrip(asyncException());
            const r = parseInt(s);
            assertEq(r, 36);
        } catch (e) {
            s = e.message;
        }
        console.log(s);
        assertEq(s, "123");
    }

    async testFutureRoundtripBackwards() {
        const s = await this.module.testsuite.TestHelpers.checkAsyncInterface(new AsyncInterfaceImpl());
        assertEq(s, "36");
    }

    async testFutureComposition() {
        const s = await this.module.testsuite.TestHelpers.checkAsyncComposition(new AsyncInterfaceImpl());
        assertEq(s, "42");
    }

    async testEarlyThrow() {
        const r = this.module.testsuite.TestHelpers.asyncEarlyThrow();
        let s = null;
        try {
            await r;
        } catch (e) {
            s = e.message;
        }
        assertEq(s, "C++: error");
    }

    async testReturnExceptionString_valid() {
        const result = await this.module.testsuite.TestHelpers.returnExceptionString(Promise.reject(new Error("hello")));
        assertTrue(result.startsWith("Error: hello"));
    }

    async testReturnExceptionString_null() {
        const result = await this.module.testsuite.TestHelpers.returnExceptionString(Promise.reject(null));
        assertEq(result, "JS promise rejected with non-error type");
    }

    async testReturnExceptionString_undefined() {
        const result = await this.module.testsuite.TestHelpers.returnExceptionString(Promise.reject(undefined));
        assertEq(result, "JS promise rejected with non-error type");
    }
}

allTests.push(AsyncTest);
