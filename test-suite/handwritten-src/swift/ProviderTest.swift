import DjinniSupport
import XCTest
@testable import TestSuite

final class ProviderTest: XCTestCase {
    func test() throws {
        let providerStr = try TestProvider_statics.getProviderString()
        XCTAssertEqual(providerStr(), "hello")

        let providerInt = try TestProvider_statics.getProviderInt()
        XCTAssertEqual(providerInt(), 42)

        let swiftProviderStr = Provider { "world" }
        XCTAssertEqual(try TestProvider_statics.callProviderString(swiftProviderStr), "world")

        let swiftProviderInt = Provider { 123 as Int32 }
        XCTAssertEqual(try TestProvider_statics.callProviderInt(swiftProviderInt), 123)

        let np = try TestProvider_statics.getNestedProvider()
        XCTAssertEqual(np.s(), 99)
        XCTAssertEqual(try TestProvider_statics.callNestedProvider(NestedProvider(s: Provider { 55 })), 55)

        let providerObj = try TestProvider_statics.getProviderObject()
        let obj = providerObj()
        XCTAssertEqual(try obj.getValue(), 42)
        XCTAssertEqual(try obj.getName(), "expensive")
        XCTAssertEqual(try TestProvider_statics.callProviderObject(providerObj), 42)
        XCTAssertEqual(try TestProvider_statics.callProviderObjectGetName(providerObj), "expensive")

        var creationCount = 0
        let swiftProviderObj = Provider<SimpleObject> {
            creationCount += 1
            return try! TestProvider_statics.createSimpleObject(999, name: "swift-created")
        }
        XCTAssertEqual(creationCount, 0)
        XCTAssertEqual(try TestProvider_statics.callProviderObject(swiftProviderObj), 999)
        XCTAssertEqual(creationCount, 1)
        XCTAssertEqual(try TestProvider_statics.callProviderObjectGetName(swiftProviderObj), "swift-created")
        XCTAssertEqual(creationCount, 2)

        let npi = try TestProvider_statics.getNestedProviderInterface()
        XCTAssertEqual(try npi.obj().getValue(), 777)
        XCTAssertEqual(try TestProvider_statics.callNestedProviderInterface(
            NestedProviderInterface(obj: Provider {
                try! TestProvider_statics.createSimpleObject(888, name: "nested-swift")
            })
        ), 888)
    }
}
