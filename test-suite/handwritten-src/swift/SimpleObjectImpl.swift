import Foundation
@testable import TestSuite

class SimpleObjectImpl: SimpleObject {
    private let value: Int32
    private let name: String
    
    init(value: Int32, name: String) {
        self.value = value
        self.name = name
    }
    
    func getValue() throws -> Int32 {
        return value
    }
    
    func getName() throws -> String {
        return name
    }
}
