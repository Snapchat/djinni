import XCTest
@testable import TextSort;
@testable import TextSortBridge;

class MyListener: TextboxListener {
    deinit {
        print("*** deinit called")
    }
    func update(_ items: ItemList) throws {
        print("*** update() called")
        print(items)
    }
}

final class AllTests: XCTestCase {
    func testExample() throws {
        let listener = MyListener()
        let s = try SortItems_statics.createWithListener(listener)
        let items = ItemList(items: ["bbb", "ccc", "aaa"])
        try s.sort(.ascending, items: items)
    }
}
