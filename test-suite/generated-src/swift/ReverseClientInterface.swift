// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from client_interface.djinni

import Foundation

public protocol ReverseClientInterface: AnyObject {
    func returnStr() throws -> String
    func methTakingInterface(_ i: TestSuite.ReverseClientInterface) throws -> String
    func methTakingOptionalInterface(_ i: Optional<TestSuite.ReverseClientInterface>) throws -> String
}
