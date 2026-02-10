import { TestCase, allTests, assertEq } from "./testutils";
import * as test from "../../generated-src/ts/test";

class SupplierTest extends TestCase {
  m: test.Test_statics;
  constructor(module: test.Test_statics) {
    super(module);
    this.m = module;
  }

  test() {
    // Simple type tests
    // get supplier string from native and call it
    var supplierStr = this.m.testsuite.TestSupplier.getSupplierString();
    var result = supplierStr();
    assertEq(result, "hello");

    // get supplier int from native and call it
    var supplierInt = this.m.testsuite.TestSupplier.getSupplierInt();
    assertEq(supplierInt(), 42);

    // create supplier string in js and pass to native which calls it
    var jsSupplierStr = () => "world";
    assertEq(this.m.testsuite.TestSupplier.callSupplierString(jsSupplierStr), "world");

    // create supplier int in js and pass to native which calls it
    var jsSupplierInt = () => 123;
    assertEq(this.m.testsuite.TestSupplier.callSupplierInt(jsSupplierInt), 123);

    // test supplier as nested object
    var ns = this.m.testsuite.TestSupplier.getNestedSupplier();
    assertEq(ns.s(), 99);

    assertEq(this.m.testsuite.TestSupplier.callNestedSupplier({ s: () => 55 }), 55);

    // Interface supplier tests - main use case for deferred expensive object creation
    // get supplier object from native and call it
    var supplierObj = this.m.testsuite.TestSupplier.getSupplierObject();
    var obj = supplierObj();
    assertEq(obj.getValue(), 42);
    assertEq(obj.getName(), "expensive");

    // Verify supplier object is called from native
    var result1 = this.m.testsuite.TestSupplier.callSupplierObject(supplierObj);
    assertEq(result1, 42);

    var result2 = this.m.testsuite.TestSupplier.callSupplierObjectGetName(supplierObj);
    assertEq(result2, "expensive");

    // Create supplier object in JS and pass to native
    // Track that creation is deferred
    var creationCount = 0;
    var jsSupplierObj = () => {
      creationCount++;
      return this.m.testsuite.TestSupplier.createSimpleObject(999, "js-created");
    };

    // Object not created yet
    assertEq(creationCount, 0);

    // Call from native - object created on demand
    var result3 = this.m.testsuite.TestSupplier.callSupplierObject(jsSupplierObj);
    assertEq(result3, 999);
    assertEq(creationCount, 1);

    var result4 = this.m.testsuite.TestSupplier.callSupplierObjectGetName(jsSupplierObj);
    assertEq(result4, "js-created");
    assertEq(creationCount, 2); // Created again

    // Test supplier interface as nested object
    var nsi = this.m.testsuite.TestSupplier.getNestedSupplierInterface();
    var nsiObj = nsi.obj();
    var nsiValue = nsiObj.getValue();
    assertEq(nsiValue, 777);

    var result5 = this.m.testsuite.TestSupplier.callNestedSupplierInterface({
      obj: () => this.m.testsuite.TestSupplier.createSimpleObject(888, "nested-js"),
    });
    assertEq(result5, 888);

    // Memoization test - verify that we can wrap a supplier with memoization
    var memoCreationCount = 0;
    var unmemoizedSupplier = () => {
      memoCreationCount++;
      return this.m.testsuite.TestSupplier.createSimpleObject(555, "memoized");
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

    var memoizedSupplier = memoize(unmemoizedSupplier);

    // First call - factory is called
    assertEq(memoCreationCount, 0);
    var result6 = this.m.testsuite.TestSupplier.callSupplierObject(memoizedSupplier);
    assertEq(result6, 555);
    assertEq(memoCreationCount, 1); // Factory called once

    // Second call - factory is NOT called again (memoized)
    var result7 = this.m.testsuite.TestSupplier.callSupplierObjectGetName(memoizedSupplier);
    assertEq(result7, "memoized");
    assertEq(memoCreationCount, 1); // Still 1 - not called again!
  }
}

allTests.push(SupplierTest);
