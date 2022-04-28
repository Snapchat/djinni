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
}

allTests.push(AsyncTest);
