// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from client_interface.djinni

import Foundation

/** Record returned by a client */
public struct ClientReturnedRecord {
    public var recordId: Int64
    public var content: String
    public var misc: Optional<String>

    public init(recordId: Int64, content: String, misc: Optional<String>)
    {
        self.recordId = recordId
        self.content = content
        self.misc = misc
    }
}
