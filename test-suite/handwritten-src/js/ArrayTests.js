class ArrayTest {
    constructor(module) {
        this.module = module;
    }
    testStringArray() {
        var inputStrings = ['1', '2', '3'];
        var expectedStrings = ['3', '2', '1'];
        assertArrayEq(this.module.testsuite.TestArray.testStringArray(inputStrings), expectedStrings);
    }

    testIntArray() {
        var inputIntegers = new Uint32Array([1,2,3]);
        var expectedIntegers = new Uint32Array([3,2,1]);
        assertArrayEq(this.module.testsuite.TestArray.testIntArray(inputIntegers), expectedIntegers);
    }

    testRecordArray() {
        var inputRecords = [{x:1, y:1}, {x:2, y:2}, {x:3, y:3}];
        var expectedRecords = [{x:3, y:3}, {x:2, y:2}, {x:1, y:1}];
        assertArrayEq(this.module.testsuite.TestArray.testRecordArray(inputRecords), expectedRecords);
    }
    
    testArrayofArray() {
        var inputArrays = [new Uint32Array([1,1,1]), new Uint32Array([2,2,2]), new Uint32Array([3,3,3])];
        var expectedArrays = [new Uint32Array([3,3,3]), new Uint32Array([2,2,2]), new Uint32Array([1,1,1])];
        var outputArrays = this.module.testsuite.TestArray.testArrayOfArray(inputArrays);
        assertArrayEq(outputArrays, expectedArrays);
    }
}

allTests.push(ArrayTest);
