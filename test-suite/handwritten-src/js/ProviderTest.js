class ProviderTest {
    constructor(module) {
        this.module = module;
    }

    test() {
        var providerStr = this.module.testsuite.TestProvider.getProviderString();
        assertEq(providerStr(), "hello");
        // C++ provider can be called more than once
        assertEq(providerStr(), "hello");
        assertEq(providerStr(), "hello");

        var providerInt = this.module.testsuite.TestProvider.getProviderInt();
        assertEq(providerInt(), 42);
        assertEq(providerInt(), 42);

        var jsProviderStr = () => "world";
        assertEq(this.module.testsuite.TestProvider.callProviderString(jsProviderStr), "world");
        // Host provider can be used in multiple C++ calls
        assertEq(this.module.testsuite.TestProvider.callProviderString(jsProviderStr), "world");

        var jsProviderInt = () => 123;
        assertEq(this.module.testsuite.TestProvider.callProviderInt(jsProviderInt), 123);
        assertEq(this.module.testsuite.TestProvider.callProviderInt(jsProviderInt), 123);

        var np = this.module.testsuite.TestProvider.getNestedProvider();
        assertEq(np.s(), 99);

        assertEq(this.module.testsuite.TestProvider.callNestedProvider({s: () => 55}), 55);

        var providerObj = this.module.testsuite.TestProvider.getProviderObject();
        var obj = providerObj();
        assertEq(obj.getValue(), 42);
        assertEq(obj.getName(), "expensive");

        assertEq(this.module.testsuite.TestProvider.callProviderObject(providerObj), 42);
        assertEq(this.module.testsuite.TestProvider.callProviderObjectGetName(providerObj), "expensive");

        var creationCount = 0;
        var jsProviderObj = () => {
            creationCount++;
            return this.module.testsuite.TestProvider.createSimpleObject(999, "js-created");
        };

        assertEq(creationCount, 0);
        // C++ calls host provider twice to verify multi-call; expect 2 creations
        assertEq(this.module.testsuite.TestProvider.callProviderObject(jsProviderObj), 999);
        assertEq(creationCount, 2);

        assertEq(this.module.testsuite.TestProvider.callProviderObjectGetName(jsProviderObj), "js-created");
        assertEq(creationCount, 4);

        var npi = this.module.testsuite.TestProvider.getNestedProviderInterface();
        assertEq(npi.obj().getValue(), 777);

        assertEq(this.module.testsuite.TestProvider.callNestedProviderInterface({
            obj: () => this.module.testsuite.TestProvider.createSimpleObject(888, "nested-js")
        }), 888);
    }
}

allTests.push(ProviderTest);
