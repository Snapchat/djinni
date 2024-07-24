import XCTest
import DjinniSupport
@testable import TestSuite

final class AsyncTest: XCTestCase {

    class AsyncInterfaceImpl: AsyncInterface {
        func futureRoundtrip(_ f: DJFuture<Int32>) throws -> DJFuture<String> {
            var token: Cancellable? = nil
            return DJFuture() { promise in
                token = f.getResult { result in
                    switch result {
                    case .success(let value):
                        promise(.success(String(value)))
                    case .failure:
                        XCTFail()
                        token = withExtendedLifetime(token) {nil}
                    }
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
        var cancellable: Cancellable? = nil
        let f2 = DJFuture<Int32>() { promise in
            cancellable = f.getResult { result in
                switch result {
                case .failure:
                    XCTFail()
                case .success(let value):
                    promise(.success(Int32(value)!))
                }
            }
        }
        let f3 = try TestHelpers_statics.futureRoundtrip(f2)
        p!(.success("36"))
        let res = try await f3.value
        XCTAssertEqual(res, "36")
        cancellable = withExtendedLifetime(cancellable) {nil}
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
        var cancellable: Cancellable? = nil
        let f2 = DJFuture<Int32>() { promise in
            cancellable = f.getResult { result in
                switch result {
                case .failure:
                    XCTFail()
                case .success(let value):
                    promise(.failure(DjinniError("123")))
                }
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
        cancellable = withExtendedLifetime(cancellable){nil}
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
