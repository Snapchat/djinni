import { TestCase, allTests, assertEq } from "./testutils";
import * as test from "../../generated-src/ts/test";

class ProviderTest extends TestCase {
  m: test.Test_statics;
  constructor(module: test.Test_statics) {
    super(module);
    this.m = module;
  }

  test() {
    var providerStr = this.m.testsuite.TestProvider.getProviderString();
    var result = providerStr();
    assertEq(result, "hello");
    // C++ provider can be called more than once
    assertEq(providerStr(), "hello");
    assertEq(providerStr(), "hello");

    var providerInt = this.m.testsuite.TestProvider.getProviderInt();
    assertEq(providerInt(), 42);
    assertEq(providerInt(), 42);

    var jsProviderStr = () => "world";
    assertEq(this.m.testsuite.TestProvider.callProviderString(jsProviderStr), "world");
    // Host provider can be used in multiple C++ calls
    assertEq(this.m.testsuite.TestProvider.callProviderString(jsProviderStr), "world");

    var jsProviderInt = () => 123;
    assertEq(this.m.testsuite.TestProvider.callProviderInt(jsProviderInt), 123);
    assertEq(this.m.testsuite.TestProvider.callProviderInt(jsProviderInt), 123);

    var np = this.m.testsuite.TestProvider.getNestedProvider();
    assertEq(np.s(), 99);

    assertEq(this.m.testsuite.TestProvider.callNestedProvider({ s: () => 55 }), 55);

    var providerObj = this.m.testsuite.TestProvider.getProviderObject();
    var obj = providerObj();
    assertEq(obj.getValue(), 42);
    assertEq(obj.getName(), "expensive");

    var result1 = this.m.testsuite.TestProvider.callProviderObject(providerObj);
    assertEq(result1, 42);

    var result2 = this.m.testsuite.TestProvider.callProviderObjectGetName(providerObj);
    assertEq(result2, "expensive");

    var creationCount = 0;
    var jsProviderObj = () => {
      creationCount++;
      return this.m.testsuite.TestProvider.createSimpleObject(999, "js-created");
    };

    assertEq(creationCount, 0);

    // C++ calls host provider twice to verify multi-call; expect 2 creations
    var result3 = this.m.testsuite.TestProvider.callProviderObject(jsProviderObj);
    assertEq(result3, 999);
    assertEq(creationCount, 2);

    var result4 = this.m.testsuite.TestProvider.callProviderObjectGetName(jsProviderObj);
    assertEq(result4, "js-created");
    assertEq(creationCount, 4);

    var npi = this.m.testsuite.TestProvider.getNestedProviderInterface();
    var npiObj = npi.obj();
    var npiValue = npiObj.getValue();
    assertEq(npiValue, 777);

    var result5 = this.m.testsuite.TestProvider.callNestedProviderInterface({
      obj: () => this.m.testsuite.TestProvider.createSimpleObject(888, "nested-js"),
    });
    assertEq(result5, 888);

    var memoCreationCount = 0;
    var unmemoizedProvider = () => {
      memoCreationCount++;
      return this.m.testsuite.TestProvider.createSimpleObject(555, "memoized");
    };

    function memoize<T>(fn: () => T): () => T {
      let cached: { value: T } | null = null;
      return () => {
        if (cached === null) {
          cached = { value: fn() };
        }
        return cached.value;
      };
    }

    var memoizedProvider = memoize(unmemoizedProvider);

    assertEq(memoCreationCount, 0);
    var result6 = this.m.testsuite.TestProvider.callProviderObject(memoizedProvider);
    assertEq(result6, 555);
    assertEq(memoCreationCount, 1);

    var result7 = this.m.testsuite.TestProvider.callProviderObjectGetName(memoizedProvider);
    assertEq(result7, "memoized");
    assertEq(memoCreationCount, 1);
  }
}

allTests.push(ProviderTest);
