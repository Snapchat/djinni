import XCTest
@testable import TestSuite

final class LazyTest: XCTestCase {
    func test() throws {
        // Simple type tests
        // get lazy string from native and call it
        let lazyStr = try TestLazy_statics.getLazyString()
        XCTAssertEqual(lazyStr(), "hello")

        // get lazy int from native and call it
        let lazyInt = try TestLazy_statics.getLazyInt()
        XCTAssertEqual(lazyInt(), 42)

        // create lazy string in swift and pass to native which calls it
        let swiftLazyStr: () -> String = { "world" }
        XCTAssertEqual(try TestLazy_statics.callLazyString(swiftLazyStr), "world")

        // create lazy int in swift and pass to native which calls it
        let swiftLazyInt: () -> Int32 = { 123 }
        XCTAssertEqual(try TestLazy_statics.callLazyInt(swiftLazyInt), 123)

        // test lazy as nested object
        let nl = try TestLazy_statics.getNestedLazy()
        XCTAssertEqual(nl.l(), 99)
        XCTAssertEqual(nl, NestedLazy(l: { 99 }))

        XCTAssertEqual(try TestLazy_statics.callNestedLazy(NestedLazy(l: { 55 })), 55)

        // Interface lazy tests - main use case for deferred expensive object creation
        // get lazy object from native and call it
        let lazyObj = try TestLazy_statics.getLazyObject()
        let obj = try lazyObj()
        XCTAssertEqual(try obj.getValue(), 42)
        XCTAssertEqual(try obj.getName(), "expensive")

        // Verify lazy object is called from native
        XCTAssertEqual(try TestLazy_statics.callLazyObject(lazyObj), 42)
        XCTAssertEqual(try TestLazy_statics.callLazyObjectGetName(lazyObj), "expensive")

        // Create lazy object in Swift and pass to native
        // Track that lazy creation is deferred
        var creationCount = 0
        let swiftLazyObj: () -> SimpleObject = {
            creationCount += 1
            return try! TestLazy_statics.createSimpleObject(999, name: "swift-created")
        }
        
        // Object not created yet
        XCTAssertEqual(creationCount, 0)
        
        // Call from native - object created on demand
        XCTAssertEqual(try TestLazy_statics.callLazyObject(swiftLazyObj), 999)
        XCTAssertEqual(creationCount, 1)
        
        XCTAssertEqual(try TestLazy_statics.callLazyObjectGetName(swiftLazyObj), "swift-created")
        XCTAssertEqual(creationCount, 2) // Created again

        // Test lazy interface as nested object
        let nli = try TestLazy_statics.getNestedLazyInterface()
        XCTAssertEqual(try nli.obj().getValue(), 777)

        XCTAssertEqual(try TestLazy_statics.callNestedLazyInterface(
            NestedLazyInterface(obj: { 
                try! TestLazy_statics.createSimpleObject(888, name: "nested-swift")
            })
        ), 888)
    }
}
