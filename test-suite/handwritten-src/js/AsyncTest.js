function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

async function asyncFunc() {
    await sleep(10);
    return 36;
}

class AsyncTest {
    constructor(module) {
        this.module = module;
    }
    async testConsumeNativeFuture() {
        const r = await this.module.TestHelpers.getAsyncResult();
        assertEq(r, 42);
    }

    async testFutureRoundtrip() {
        const f = asyncFunc();
        const s = await this.module.TestHelpers.futureRoundtrip(f);
        const r = parseInt(s);
        assertEq(r, 36);
    }
}

allTests.push(AsyncTest);
