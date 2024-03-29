// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from varnames.djinni

#pragma once

#include <memory>

namespace testsuite {

struct VarnameRecord;

class VarnameInterface {
public:
    virtual ~VarnameInterface() = default;

    /**
     * We should also rewrite parameter names in docstrings.
     * _r_arg_ should be rewritten.
     * _i_arg_ should not.
     */
    virtual VarnameRecord _rmethod_(const VarnameRecord & _r_arg_) = 0;

    virtual /*not-null*/ std::shared_ptr<VarnameInterface> _imethod_(const /*not-null*/ std::shared_ptr<VarnameInterface> & _i_arg_) = 0;
};

} // namespace testsuite
