#pragma once

#include <string>
#include <vector>

namespace djinni::test {

class Person {
public:
    auto name() const { return _name; }
    void set_name(std::string name) { _name = name; }

    void set_id(int32_t id) { _id = id; }
    
private:
    std::string _name;
    int32_t _id;
    std::string _email;
};

class AddressBook {
public:
    auto people() const { return _people; }

    Person* add_people() {
        _people.push_back({});
        return &_people.back();
    }
private:
    std::vector<Person> _people;
};

}
