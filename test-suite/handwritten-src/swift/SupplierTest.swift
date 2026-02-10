import DjinniSupport
import XCTest
@testable import TestSuite

final class SupplierTest: XCTestCase {
    func test() throws {
        // Simple type tests
        // get supplier string from native and call it
        let supplierStr = try TestSupplier_statics.getSupplierString()
        XCTAssertEqual(supplierStr(), "hello")

        // get supplier int from native and call it
        let supplierInt = try TestSupplier_statics.getSupplierInt()
        XCTAssertEqual(supplierInt(), 42)

        // create supplier string in swift and pass to native which calls it
        let swiftSupplierStr = Supplier { "world" }
        XCTAssertEqual(try TestSupplier_statics.callSupplierString(swiftSupplierStr), "world")

        // create supplier int in swift and pass to native which calls it
        let swiftSupplierInt = Supplier { 123 as Int32 }
        XCTAssertEqual(try TestSupplier_statics.callSupplierInt(swiftSupplierInt), 123)

        // test supplier as nested object
        let ns = try TestSupplier_statics.getNestedSupplier()
        XCTAssertEqual(ns.s(), 99)

        XCTAssertEqual(try TestSupplier_statics.callNestedSupplier(NestedSupplier(s: Supplier { 55 })), 55)

        // Interface supplier tests - main use case for deferred expensive object creation
        // get supplier object from native and call it
        let supplierObj = try TestSupplier_statics.getSupplierObject()
        let obj = try supplierObj()
        XCTAssertEqual(try obj.getValue(), 42)
        XCTAssertEqual(try obj.getName(), "expensive")

        // Verify supplier object is called from native
        XCTAssertEqual(try TestSupplier_statics.callSupplierObject(supplierObj), 42)
        XCTAssertEqual(try TestSupplier_statics.callSupplierObjectGetName(supplierObj), "expensive")

        // Create supplier object in Swift and pass to native
        // Track that creation is deferred
        var creationCount = 0
        let swiftSupplierObj = Supplier<SimpleObject> {
            creationCount += 1
            return try! TestSupplier_statics.createSimpleObject(999, name: "swift-created")
        }

        // Object not created yet
        XCTAssertEqual(creationCount, 0)

        // Call from native - object created on demand
        XCTAssertEqual(try TestSupplier_statics.callSupplierObject(swiftSupplierObj), 999)
        XCTAssertEqual(creationCount, 1)

        XCTAssertEqual(try TestSupplier_statics.callSupplierObjectGetName(swiftSupplierObj), "swift-created")
        XCTAssertEqual(creationCount, 2) // Created again

        // Test supplier interface as nested object
        let nsi = try TestSupplier_statics.getNestedSupplierInterface()
        XCTAssertEqual(try nsi.obj().getValue(), 777)

        XCTAssertEqual(try TestSupplier_statics.callNestedSupplierInterface(
            NestedSupplierInterface(obj: Supplier {
                try! TestSupplier_statics.createSimpleObject(888, name: "nested-swift")
            })
        ), 888)
    }
}
