class DataTest {
    constructor(module) {
        this.m = module;
    }

    setUp() {
        this.test = this.m.DataRefTest.create();
    }

    testSend() {
        var input = [0, 1, 2, 3];
        var buf = this.m.allocateDirectBuffer(4);
        buf.set(input);
        this.test.sendData(buf);
        var output = this.test.retriveAsBin();
        assertArrayEq(input, output);
    }

    testSendMutable() {
        var input = [0, 1, 2, 3];
        var buf = this.m.allocateDirectBuffer(4);
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
        var buf = this.m.allocateDirectBuffer(4);
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
