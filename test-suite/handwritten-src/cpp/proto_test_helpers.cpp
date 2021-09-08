#include "proto_tests.hpp"
#include "RecordWithEmbeddedProto.hpp"
#include "RecordWithEmbeddedCppProto.hpp"

namespace testsuite {

std::vector<std::string> ProtoTests::protoToStrings(const ::djinni::test::AddressBook& x) {
    std::vector<std::string> res;
    for (const auto& p : x.people()) {
        res.push_back(p.name());
    }
    return res;
}

::djinni::test::AddressBook ProtoTests::stringsToProto(const std::vector<std::string>& x) {
    ::djinni::test::AddressBook proto;
    int id = 0;
    for (const auto& name: x) {
        auto* p = proto.add_people();
        p->set_name(name);
        p->set_id(++id);
    }
    return proto;
}

std::string ProtoTests::embeddedProtoToString(const RecordWithEmbeddedProto& x) {
    return x.person.name();
}

RecordWithEmbeddedProto ProtoTests::stringToEmbeddedProto(const std::string& x) {
    ::djinni::test::Person proto;
    proto.set_name(x);
    proto.set_id(1);
    return {proto};
}

std::string ProtoTests::cppProtoToString(const ::djinni::test2::PersistingState& x) {
    return x.user_id();
}

::djinni::test2::PersistingState ProtoTests::stringToCppProto(const std::string& x) {
    ::djinni::test2::PersistingState ret;
    ret.set_user_id(x);
    return ret;
}

std::string ProtoTests::embeddedCppProtoToString(const RecordWithEmbeddedCppProto& x) {
    return x.state.user_id();
}

RecordWithEmbeddedCppProto ProtoTests::stringToEmbeddedCppProto(const std::string& x) {
    RecordWithEmbeddedCppProto ret({});
    ret.state.set_user_id(x);
    return ret;
}

std::vector<std::string> ProtoTests::protoListToStrings(const std::vector<::djinni::test::Person>& x) {
    std::vector<std::string> ret;
    for (const auto& i : x) {
        ret.push_back(i.name());
    }
    return ret;
}

std::vector<::djinni::test::Person> ProtoTests::stringsToProtoList(const std::vector<std::string>& x) {
    std::vector<::djinni::test::Person> ret;
    int id = 0;
    for (const auto& i : x) {
        ::djinni::test::Person proto;
        proto.set_name(i);
        proto.set_id(++id);
        ret.push_back(proto);
    }
    return ret;
}

std::string ProtoTests::optionalProtoToString(const std::experimental::optional<::djinni::test::Person>& x) {
    return x->name();
}

std::experimental::optional<::djinni::test::Person> ProtoTests::stringToOptionalProto(const std::string& x) {
    ::djinni::test::Person proto;
    proto.set_name(x);
    proto.set_id(1);
    return proto;
}

djinni::expected<::djinni::test::Person, int32_t> ProtoTests::stringToProtoOutcome(const std::string& x) {
    ::djinni::test::Person proto;
    proto.set_name(x);
    proto.set_id(1);
    return proto;    
}

}
