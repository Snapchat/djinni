#include "optional_interface.hpp"
#include "mixed_record.hpp"
#include "optional_record.hpp"
#include "optional_test_helpers.hpp"

namespace testsuite {
    std::shared_ptr<OptionalInterface> gOptInterface = std::make_shared<OptionalInterface>();
    const BaseRecord gBaseRecord = {7};

    BaseRecord OptionalTestHelpers::get_base_record() {
        return gBaseRecord;
    }

    std::shared_ptr<OptionalInterface> OptionalTestHelpers::optional_interface() {
        return gOptInterface;
    }

    MixedRecord OptionalTestHelpers::get_mixed_record() {
        auto mixedRecord = MixedRecord(5, gBaseRecord);
        mixedRecord.interfaceVal = gOptInterface;
        return mixedRecord;
    }

    OptionalRecord OptionalTestHelpers::get_optional_record() {
        auto optionalRecord = OptionalRecord();
        optionalRecord.optInt = 6;
        return optionalRecord;
    }

    /** Checks that translation occurs properly */
    MixedRecord OptionalTestHelpers::mixed_record_id(const MixedRecord & i) {
        return i;
    }
} // namespace testsuite
