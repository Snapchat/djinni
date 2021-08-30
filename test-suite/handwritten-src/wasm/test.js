class Test1 {
    constructor() {
        this.x = 42;
    }
    setUp() {
    }
    tearDown() {
    }
    testAbc() {
        assertTrue(true);
    }
    testBcd() {
        assertTrue(false);
    }
}

runTests([Test1]);
