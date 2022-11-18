import {TestCase, allTests, assertEq, assertTrue} from "./testutils"
import * as test from "../../generated-src/ts/test";

function sleep(ms: number): Promise<void> {
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
    async futureRoundtrip(f: Promise<number>) {
        const i = await f;
        return i.toString();
    }
}

class AsyncTest extends TestCase {
    m: test.Test_statics;
    constructor(module: test.Test_statics) {
        super(module);
        this.m = module;
    }
    async testConsumeNativeFuture() {
        const r = await this.m.testsuite.TestHelpers.getAsyncResult();
        assertEq(r, 42);
    }

    async testConsumeNativeFutureAltSyntax() {
        return this.m.testsuite.TestHelpers.getAsyncResult().then(res => assertEq(res, 42)).catch(e => assertTrue(false));
    }

    async testFutureRoundtrip() {
        const s = await this.m.testsuite.TestHelpers.futureRoundtrip(asyncFunc());
        const r = parseInt(s);
        assertEq(r, 36);
    }

  async testVoidRountTrip() {
    await this.m.testsuite.TestHelpers.voidAsyncMethod(sleep(10));
  }

    async testFutureRoundtripWithException() {
        var s = null;
        try {
            const s = await this.m.testsuite.TestHelpers.futureRoundtrip(asyncException());
            const r = parseInt(s);
            assertEq(r, 36);
        } catch (e: any) {
            s = e.message;
        }
        console.log(s);
        assertEq(s, "123");
    }

    async testFutureRoundtripBackwards() {
        const s = await this.m.testsuite.TestHelpers.checkAsyncInterface(new AsyncInterfaceImpl());
        assertEq(s, "36");
    }
    async testFutureComposition() {
        const s = await this.m.testsuite.TestHelpers.checkAsyncComposition(new AsyncInterfaceImpl());
        assertEq(s, "42");
    }

    async testEarlyThrow() {
        const r = this.m.testsuite.TestHelpers.asyncEarlyThrow();
        let s = null;
        try {
            await r;
        } catch (e: any) {
            s = e.message.split(/\r?\n/).shift();
        }
        assertEq(s, "C++: error");
    }
}

allTests.push(AsyncTest);
