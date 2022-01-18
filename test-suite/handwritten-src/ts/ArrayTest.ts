import {TestCase, allTests, assertArrayEq} from "./testutils"
import * as test from "../../generated-src/ts/test";

export class ArrayTest extends TestCase {
    m: test.Test_statics;
    constructor(module: test.Test_statics) {
        super(module);
        this.m = module;
    }
    testStringArray() {
        var inputStrings = ['1', '2', '3'];
        var expectedStrings = ['3', '2', '1'];
        assertArrayEq(this.m.testsuite.TestArray.testStringArray(inputStrings), expectedStrings);
    }

    testIntArray() {
        var inputIntegers = new Int32Array([1,2,3]);
        var expectedIntegers = new Int32Array([3,2,1]);
        assertArrayEq(this.m.testsuite.TestArray.testIntArray(inputIntegers), expectedIntegers);
    }

    testRecordArray() {
        var inputRecords = [{x:1, y:1}, {x:2, y:2}, {x:3, y:3}];
        var expectedRecords = [{x:3, y:3}, {x:2, y:2}, {x:1, y:1}];
        assertArrayEq(this.m.testsuite.TestArray.testRecordArray(inputRecords), expectedRecords);
    }
    
    testArrayofArray() {
        var inputArrays = [new Int32Array([1,1,1]), new Int32Array([2,2,2]), new Int32Array([3,3,3])];
        var expectedArrays = [new Int32Array([3,3,3]), new Int32Array([2,2,2]), new Int32Array([1,1,1])];
        var outputArrays = this.m.testsuite.TestArray.testArrayOfArray(inputArrays);
        assertArrayEq(outputArrays, expectedArrays);
    }
}

allTests.push(ArrayTest);
