class ArrayTest {
    constructor(module) {
        this.module = module;
    }
    testStringArray() {
        var inputStrings = ['1', '2', '3'];
        var expectedStrings = ['3', '2', '1'];
        assertArrayEq(this.module.TestArray.testStringArray(inputStrings), expectedStrings);
    }

    testIntArray() {
        var inputIntegers = new Uint32Array([1,2,3]);
        var expectedIntegers = new Uint32Array([3,2,1]);
        assertArrayEq(this.module.TestArray.testIntArray(inputIntegers), expectedIntegers);
    }

    testRecordArray() {
        var inputRecords = [{mX:1, mY:1}, {mX:2, mY:2}, {mX:3, mY:3}];
        var expectedRecords = [{mX:3, mY:3}, {mX:2, mY:2}, {mX:1, mY:1}];
        assertArrayEq(this.module.TestArray.testRecordArray(inputRecords), expectedRecords);
    }
    
    testArrayofArray() {
        var inputArrays = [new Uint32Array([1,1,1]), new Uint32Array([2,2,2]), new Uint32Array([3,3,3])];
        var expectedArrays = [new Uint32Array([3,3,3]), new Uint32Array([2,2,2]), new Uint32Array([1,1,1])];
        var outputArrays = this.module.TestArray.testArrayOfArray(inputArrays);
        assertArrayEq(outputArrays, expectedArrays);
    }
}

allTests.push(ArrayTest);
