import {TestCase, assertEq} from "./testutils"

export default class LazyTest extends TestCase {
    test() {
        // Simple type tests
        // get lazy string from native and call it
        var lazyStr = this.m.testsuite.TestLazy.getLazyString();
        assertEq(lazyStr(), "hello");

        // get lazy int from native and call it
        var lazyInt = this.m.testsuite.TestLazy.getLazyInt();
        assertEq(lazyInt(), 42);

        // create lazy string in js and pass to native which calls it
        var jsLazyStr = () => "world";
        assertEq(this.m.testsuite.TestLazy.callLazyString(jsLazyStr), "world");

        // create lazy int in js and pass to native which calls it
        var jsLazyInt = () => 123;
        assertEq(this.m.testsuite.TestLazy.callLazyInt(jsLazyInt), 123);

        // test lazy as nested object
        var nl = this.m.testsuite.TestLazy.getNestedLazy();
        assertEq(nl.l(), 99);
        assertEq(nl, {l: () => 99});

        assertEq(this.m.testsuite.TestLazy.callNestedLazy({l: () => 55}), 55);

        // Interface lazy tests - main use case for deferred expensive object creation
        // get lazy object from native and call it
        var lazyObj = this.m.testsuite.TestLazy.getLazyObject();
        var obj = lazyObj();
        assertEq(obj.getValue(), 42);
        assertEq(obj.getName(), "expensive");

        // Verify lazy object is called from native
        assertEq(this.m.testsuite.TestLazy.callLazyObject(lazyObj), 42);
        assertEq(this.m.testsuite.TestLazy.callLazyObjectGetName(lazyObj), "expensive");

        // Create lazy object in JS and pass to native
        // Track that lazy creation is deferred
        var creationCount = 0;
        var jsLazyObj = () => {
            creationCount++;
            return this.m.testsuite.TestLazy.createSimpleObject(999, "js-created");
        };
        
        // Object not created yet
        assertEq(creationCount, 0);
        
        // Call from native - object created on demand
        assertEq(this.m.testsuite.TestLazy.callLazyObject(jsLazyObj), 999);
        assertEq(creationCount, 1);
        
        assertEq(this.m.testsuite.TestLazy.callLazyObjectGetName(jsLazyObj), "js-created");
        assertEq(creationCount, 2); // Created again

        // Test lazy interface as nested object
        var nli = this.m.testsuite.TestLazy.getNestedLazyInterface();
        assertEq(nli.obj().getValue(), 777);

        assertEq(this.m.testsuite.TestLazy.callNestedLazyInterface({
            obj: () => this.m.testsuite.TestLazy.createSimpleObject(888, "nested-js")
        }), 888);
    }
}
