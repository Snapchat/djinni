// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from example.djinni

#pragma once

namespace textsort {

struct ItemList;

class TextboxListener {
public:
    virtual ~TextboxListener() = default;

    virtual void update(const ItemList & items) = 0;
};

}  // namespace textsort
