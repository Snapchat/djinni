import {TestCase, assertEq} from "./testutils"

export default class SupplierTest extends TestCase {
    test() {
        // Simple type tests
        // get supplier string from native and call it
        var supplierStr = this.m.testsuite.TestSupplier.getSupplierString();
        assertEq(supplierStr(), "hello");

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
        assertEq(ns, {s: () => 99});

        assertEq(this.m.testsuite.TestSupplier.callNestedSupplier({s: () => 55}), 55);

        // Interface supplier tests - main use case for deferred expensive object creation
        // get supplier object from native and call it
        var supplierObj = this.m.testsuite.TestSupplier.getSupplierObject();
        var obj = supplierObj();
        assertEq(obj.getValue(), 42);
        assertEq(obj.getName(), "expensive");

        // Verify supplier object is called from native
        assertEq(this.m.testsuite.TestSupplier.callSupplierObject(supplierObj), 42);
        assertEq(this.m.testsuite.TestSupplier.callSupplierObjectGetName(supplierObj), "expensive");

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
        assertEq(this.m.testsuite.TestSupplier.callSupplierObject(jsSupplierObj), 999);
        assertEq(creationCount, 1);

        assertEq(this.m.testsuite.TestSupplier.callSupplierObjectGetName(jsSupplierObj), "js-created");
        assertEq(creationCount, 2); // Created again

        // Test supplier interface as nested object
        var nsi = this.m.testsuite.TestSupplier.getNestedSupplierInterface();
        assertEq(nsi.obj().getValue(), 777);

        assertEq(this.m.testsuite.TestSupplier.callNestedSupplierInterface({
            obj: () => this.m.testsuite.TestSupplier.createSimpleObject(888, "nested-js")
        }), 888);
    }
}
