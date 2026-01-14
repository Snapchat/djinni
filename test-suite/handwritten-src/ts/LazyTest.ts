import { TestCase, allTests, assertEq } from "./testutils";
import * as test from "../../generated-src/ts/test";

class LazyTest extends TestCase {
  m: test.Test_statics;
  constructor(module: test.Test_statics) {
    super(module);
    this.m = module;
  }

  test() {
    // Simple type tests
    // get lazy string from native and call it
    var lazyStr = this.m.testsuite.TestLazy.getLazyString();
    var result = lazyStr();
    assertEq(result, "hello");

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

    assertEq(this.m.testsuite.TestLazy.callNestedLazy({ l: () => 55 }), 55);

    // Interface lazy tests - main use case for deferred expensive object creation
    // get lazy object from native and call it
    var lazyObj = this.m.testsuite.TestLazy.getLazyObject();
    var obj = lazyObj();
    assertEq(obj.getValue(), 42);
    assertEq(obj.getName(), "expensive");

    // Verify lazy object is called from native
    var result1 = this.m.testsuite.TestLazy.callLazyObject(lazyObj);
    assertEq(result1, 42);

    var result2 = this.m.testsuite.TestLazy.callLazyObjectGetName(lazyObj);
    assertEq(result2, "expensive");

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
    var result3 = this.m.testsuite.TestLazy.callLazyObject(jsLazyObj);
    assertEq(result3, 999);
    assertEq(creationCount, 1);

    var result4 = this.m.testsuite.TestLazy.callLazyObjectGetName(jsLazyObj);
    assertEq(result4, "js-created");
    assertEq(creationCount, 2); // Created again

    // Test lazy interface as nested object
    var nli = this.m.testsuite.TestLazy.getNestedLazyInterface();
    var nliObj = nli.obj();
    var nliValue = nliObj.getValue();
    assertEq(nliValue, 777);

    var result5 = this.m.testsuite.TestLazy.callNestedLazyInterface({
      obj: () => this.m.testsuite.TestLazy.createSimpleObject(888, "nested-js"),
    });
    assertEq(result5, 888);

    // Memoization test - verify that we can wrap a lazy with memoization
    var memoCreationCount = 0;
    var unmemoizedLazy = () => {
      memoCreationCount++;
      return this.m.testsuite.TestLazy.createSimpleObject(555, "memoized");
    };

    // Simple memoization wrapper
    function memoize<T>(fn: () => T): () => T {
      let cached: { value: T } | null = null;
      return () => {
        if (cached === null) {
          cached = { value: fn() };
        }
        return cached.value;
      };
    }

    var memoizedLazy = memoize(unmemoizedLazy);

    // First call - factory is called
    assertEq(memoCreationCount, 0);
    var result6 = this.m.testsuite.TestLazy.callLazyObject(memoizedLazy);
    assertEq(result6, 555);
    assertEq(memoCreationCount, 1); // Factory called once

    // Second call - factory is NOT called again (memoized)
    var result7 = this.m.testsuite.TestLazy.callLazyObjectGetName(memoizedLazy);
    assertEq(result7, "memoized");
    assertEq(memoCreationCount, 1); // Still 1 - not called again!
  }
}

allTests.push(LazyTest);
