function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

async function asyncFunc() {
    await sleep(10);
    throw new Error("321");
    return 36;
}

class AsyncTest {
    constructor(module) {
        this.module = module;
    }
    async testConsumeNativeFuture() {
        try {
            const r = await this.module.TestHelpers.getAsyncResult();
            assertEq(r, 42);
        } catch (e) {
            console.log(e);
        }
        // this.module.TestHelpers.getAsyncResult()
        //     .then(res => console.log('ok' + res))
        //     .catch(err => console.log('ko' + err));
    }

    async testFutureRoundtrip() {
        const f = asyncFunc();
        try {
            const s = await this.module.TestHelpers.futureRoundtrip(f);
            const r = parseInt(s);
            assertEq(r, 36);
        } catch (e) {
            console.log(e);
        }
    }
}

allTests.push(AsyncTest);
