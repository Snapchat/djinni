import {TestCase, allTests, assertEq, assertTrue} from "./testutils"
import * as test from "../../generated-src/ts/test";

function sleep(ms: number) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

async function asyncFunc() {
    await sleep(10);
    return 36;
}

async function asyncException() {
    await sleep(10);
    throw new Error("123");
    return 36;
}

class AsyncInterfaceImpl implements test.AsyncInterface {
    async futureRoundtrip(f) {
        const i = await f;
        return i.toString();
    }
}

class AsyncTest extends TestCase {
    m: test.Test_statics;
    constructor(module: test.Test_module_statics) {
        super(module);
        this.m = module.testsuite;
    }
    async testConsumeNativeFuture() {
        const r = await this.m.TestHelpers.getAsyncResult();
        assertEq(r, 42);
    }

    async testConsumeNativeFutureAltSyntax() {
        return this.m.TestHelpers.getAsyncResult().then(res => assertEq(res, 42)).catch(e => assertTrue(false));
    }

    async testFutureRoundtrip() {
        const s = await this.m.TestHelpers.futureRoundtrip(asyncFunc());
        const r = parseInt(s);
        assertEq(r, 36);
    }

    async testFutureRoundtripWithException() {
        var s = null;
        try {
            const s = await this.m.TestHelpers.futureRoundtrip(asyncException());
            const r = parseInt(s);
            assertEq(r, 36);
        } catch (e) {
            s = e.message;
        }
        console.log(s);
        assertEq(s, "123");
    }

    async testFutureRoundtripBackwards() {
        const s = await this.m.TestHelpers.checkAsyncInterface(new AsyncInterfaceImpl());
        assertEq(s, "36");
    }
}

allTests.push(AsyncTest);
