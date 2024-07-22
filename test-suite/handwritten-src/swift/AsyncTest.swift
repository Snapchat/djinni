import XCTest
import DjinniSupport
import Combine
@testable import TestSuite

final class AsyncTest: XCTestCase {

    class AsyncInterfaceImpl: AsyncInterface {
        func futureRoundtrip(_ f: DJFuture<Int32>) throws -> DJFuture<String> {
            var token: AnyCancellable? = nil
            return DJFuture() { promise in
                token = f.sink { completion in
                    if case .failure = completion {
                        XCTAssertTrue(false)
                    }
                    token = withExtendedLifetime(token) {nil}
                } receiveValue: { i in
                    promise(.success(String(i)))
                }
            }
        }
    }
    
    func testConsumeNativeFuture() async throws {
        let f = try TestHelpers_statics.getAsyncResult()
        let i = try await f.value
        XCTAssertEqual(i, 42)
    }

    func testFutureRoundtrip() async throws {
        var p: DJFuture<String>.Promise? = nil
        let f = DJFuture<String>() { promise in
            p = promise
        }
        var sink: AnyCancellable? = nil
        let f2 = DJFuture<Int32>() { promise in
            sink = f.sink { completion in if case .failure = completion {XCTAssertTrue(false)}}
            receiveValue: { s in promise(.success(Int32(s)!))}
        }
        let f3 = try TestHelpers_statics.futureRoundtrip(f2)
        p!(.success("36"))
        let res = try await f3.value
        XCTAssertEqual(res, "36")
        sink = withExtendedLifetime(sink) {nil}
    }

    func testVoidRoundtrip() async throws {
        let f = DJFuture<Void>() { promise in
            promise(.success(()))
        }
        let f1 = try TestHelpers_statics.voidAsyncMethod(f)
        try await f1.value
    }

    func testFutureRoundtripWithException() async throws {
        var p: DJFuture<String>.Promise? = nil
        let f = DJFuture<String>() { promise in
            p = promise
        }
        var sink: AnyCancellable? = nil
        let f2 = DJFuture<Int32>() { promise in
            sink = f.sink { completion in if case .failure = completion {XCTAssertTrue(false)}}
            receiveValue: { s in
                promise(.failure(DjinniError("123")))
            }
        }
        let f3 = try TestHelpers_statics.futureRoundtrip(f2)
        p!(.success("36"))
        var s: String? = nil
        do {
            let _ = try await f3.value
        } catch let e as DjinniError {
            s = e.errorMessage
        }
        XCTAssertEqual(s, "123")
        sink = withExtendedLifetime(sink){nil}
    }

    func testFutureRoundtripBackwards() async throws {
        let s = try await TestHelpers_statics.checkAsyncInterface(AsyncInterfaceImpl()).value
        XCTAssertEqual(s, "36")
    }

    func testFutureComposition() async throws {
        let s = try await TestHelpers_statics.checkAsyncComposition(AsyncInterfaceImpl()).value
        XCTAssertEqual(s, "42")
    }

    func testOptionalFutureUnsetValue() async throws {
        let input = DJFuture<Optional<Int32>> { promise in
            promise(.success(nil))
        }
        let output = try await TestHelpers_statics.addOneIfPresent(input).value
        XCTAssertNil(output)
    }

    func testOptionalFutureSetValue() async throws {
        let input = DJFuture<Optional<Int32>> { promise in
            promise(.success(10))
        }
        let output = try await TestHelpers_statics.addOneIfPresent(input).value
        XCTAssertEqual(output, 11)
    }
}
