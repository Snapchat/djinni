#include "DataRefTest.hpp"

namespace testsuite {

using namespace snapchat::djinni;

class DataRefTestCpp : public ::testsuite::DataRefTest {
    DataRef _data;
public:
    void sendData(const DataRef& data) override {
        _data = data;
    }

    std::vector<uint8_t> retriveAsBin() override {
        return {_data.buf(), _data.buf() + _data.len()};
    }

    void sendMutableData(const DataRef& data) override {
        auto buf = data.mutableBuf();
        std::reverse(buf, buf + data.len());
    }

    DataRef generateData() override {
        DataRef data(4);
        auto buf = data.mutableBuf();
        for (size_t i = 0; i < data.len(); ++i) {
            buf[i] = i;
        }
        return data;
    }

    DataRef dataFromVec() override {
        std::vector<uint8_t> buf = {0, 1, 2, 3};
        return DataRef(std::move(buf));
    }

    DataRef dataFromStr() override {
        std::string buf = "abcd";
        return DataRef(std::move(buf));
    }

    std::vector<uint8_t> sendDataView(const DataView& data) override {
        return {data.buf(), data.buf() + data.len()};
    }

    DataView recvDataView() override {
        // Return a static buffer that is still valid after the function
        // returns, because DataView does not carry ownership.
        const static uint8_t buf[] = {0, 1, 2, 3};
        return DataView(buf, sizeof(buf));
    }
};

std::shared_ptr<DataRefTest> DataRefTest::create() {
    return std::make_shared<DataRefTestCpp>();
}

}
