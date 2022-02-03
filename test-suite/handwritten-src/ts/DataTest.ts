import {TestCase, allTests, assertArrayEq} from "./testutils"
import * as test from "../../generated-src/ts/test";
import {DjinniModule} from "@djinni_support/DjinniModule"

class DataTest extends TestCase {
    m: test.Test_statics & DjinniModule;
    test: test.DataRefTest;

    constructor(module: test.Test_statics & DjinniModule) {
        super(module);
        this.m = module;
        this.test = this.m.testsuite.DataRefTest.create()!;
    }

    testSend() {
        var input = [0, 1, 2, 3];
        var buf = this.m.allocateWasmBuffer(4);
        buf.set(input);
        this.test.sendData(buf);
        var output = this.test.retriveAsBin();
        assertArrayEq(input, output);
    }

    testSendMutable() {
        var input = [0, 1, 2, 3];
        var buf = this.m.allocateWasmBuffer(4);
        buf.set(input);
        this.test.sendMutableData(buf);
        assertArrayEq([3, 2, 1, 0], buf);
    }

    testGenerateData() {
        var buf = this.test.generateData();
        assertArrayEq([0, 1, 2, 3], buf);
    };

    testDataFromVec() {
        var buf = this.test.dataFromVec();
        assertArrayEq([0, 1, 2, 3], buf);
    }

    testDataFromStr() {
        var buf = this.test.dataFromStr();
        // codepoints of a, b, c, d
        assertArrayEq([97, 98, 99, 100], buf);
    }

    testSendDataView() {
        var input = [0, 1, 2, 3];
        var buf = this.m.allocateWasmBuffer(4);
        buf.set(input);
        var output = this.test.sendDataView(buf);
        assertArrayEq(input, output);
    }
    
    testRecvDataView() {
        var output = this.test.recvDataView();
        assertArrayEq([0, 1, 2, 3], output);
    }
}

allTests.push(DataTest);
