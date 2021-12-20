#pragma once

#include "EnumSixValue.hpp"
#include "ObjectNative.hpp"
#include "ObjectPlatform.hpp"
#include "RecordSixInt.hpp"
#include "djinni_perf_benchmark.hpp"
#include <string>

namespace snapchat::djinni::benchmark {

class DjinniPerfBenchmarkImpl : public DjinniPerfBenchmark {
public:
    static std::shared_ptr<DjinniPerfBenchmarkImpl> getInstance();

    DjinniPerfBenchmarkImpl() = default;

    // Delete all default copy and move constructors.
    DjinniPerfBenchmarkImpl(const DjinniPerfBenchmarkImpl&) = delete;
    DjinniPerfBenchmarkImpl& operator=(const DjinniPerfBenchmarkImpl&) = delete;
    DjinniPerfBenchmarkImpl(DjinniPerfBenchmarkImpl&&) noexcept = delete;
    DjinniPerfBenchmarkImpl& operator=(DjinniPerfBenchmarkImpl&&) noexcept = delete;

    ~DjinniPerfBenchmarkImpl() override = default;

    int64_t cppTests() override;
    void baseline() override;
    void argString(const std::string& s) override;
    void argBinary(const std::vector<uint8_t>& b) override;
    void argDataRef(const ::djinni::DataRef& r) override;
    void argDataView(const ::djinni::DataView& d) override;
    void argEnumSixValue(EnumSixValue e) override;
    void argRecordSixInt(const RecordSixInt& r) override;
    void argListInt(const std::vector<int64_t>& v) override;
    void argArrayInt(const std::vector<int64_t>& v) override;
    void argObject(const std::shared_ptr<ObjectPlatform>& c) override;
    void argListObject(const std::vector<std::shared_ptr<ObjectPlatform>>& l) override;
    void argListRecord(const std::vector<RecordSixInt>& l) override;
    void argArrayRecord(const std::vector<RecordSixInt>& a) override;
    int64_t returnInt(int64_t value) override;
    std::string returnString(int32_t size) override;
    std::vector<uint8_t> returnBinary(int32_t size) override;
    std::shared_ptr<ObjectNative> returnObject() override;
    std::vector<int64_t> returnListInt(int32_t size) override;
    std::vector<int64_t> returnArrayInt(int32_t size) override;
    std::vector<std::shared_ptr<ObjectNative>> returnListObject(int32_t size) override;
    std::vector<RecordSixInt> returnListRecord(int32_t size) override;
    std::vector<RecordSixInt> returnArrayRecord(int32_t size) override;
    std::string roundTripString(const std::string& s) override;
};

} // namespace snap::djinni_perf_benchmark
