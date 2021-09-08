#pragma once

#include <string>

namespace djinni::test2 {

class PersistingState {
public:
    auto user_id() const { return _user_id; }
    void set_user_id(const std::string& uid) { _user_id = uid; }
private:
    std::string _user_id;
    bool _is_active;
};

}
