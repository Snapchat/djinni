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
        const s = await this.module.TestHelpers.futureRoundtrip(asyncFunc());
        const r = parseInt(s);
        assertEq(r, 36);
    }

    async testFutureRoundtripWithException() {
        var s = null;
        try {
            const s = await this.module.TestHelpers.futureRoundtrip(asyncException());
            const r = parseInt(s);
            assertEq(r, 36);
        } catch (e) {
            s = e.message;
        }
        console.log(s);
        assertEq(s, "123");
    }

    async testFutureRoundtripBackwards() {
        const s = await this.module.TestHelpers.checkAsyncInterface(new AsyncInterfaceImpl());
        assertEq(s, "36");
    }
}

allTests.push(AsyncTest);
