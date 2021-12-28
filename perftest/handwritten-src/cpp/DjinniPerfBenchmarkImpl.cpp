#include "DjinniPerfBenchmarkImpl.hpp"
#include "ObjectNativeImpl.hpp"

#include <chrono>
#include <string>
#include <thread>

namespace snapchat::djinni::benchmark {

std::shared_ptr<DjinniPerfBenchmark> DjinniPerfBenchmark::getInstance() {
    return std::make_shared<DjinniPerfBenchmarkImpl>();
}

int64_t DjinniPerfBenchmarkImpl::cppTests() {
    auto t1 = std::chrono::high_resolution_clock::now();
    constexpr int kBufSize = 256;
    char buf[kBufSize];
    char buf2[kBufSize];
    memcpy(buf, buf2, kBufSize);
    auto t2 = std::chrono::high_resolution_clock::now();
    int64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    return ns;
}

void DjinniPerfBenchmarkImpl::baseline() {}

void DjinniPerfBenchmarkImpl::argString(const std::string& /* s */) {}

void DjinniPerfBenchmarkImpl::argBinary(const std::vector<uint8_t>& /* b */) {}

void DjinniPerfBenchmarkImpl::argDataRef(const ::djinni::DataRef& /* r */) {}

void DjinniPerfBenchmarkImpl::argDataView(const ::djinni::DataView& /* d */) {}

void DjinniPerfBenchmarkImpl::argEnumSixValue(EnumSixValue /* e */) {}

void DjinniPerfBenchmarkImpl::argRecordSixInt(const RecordSixInt& /* r */) {}

void DjinniPerfBenchmarkImpl::argListInt(const std::vector<int64_t>& /* v */) {}

void DjinniPerfBenchmarkImpl::argArrayInt(const std::vector<int64_t>& /* v */) {}

void DjinniPerfBenchmarkImpl::argObject(const std::shared_ptr<ObjectPlatform>& /* c */) {}

void DjinniPerfBenchmarkImpl::argListObject(const std::vector<std::shared_ptr<ObjectPlatform>>& /* l */) {}

void DjinniPerfBenchmarkImpl::argListRecord(const std::vector<RecordSixInt>& /* l */) {}

void DjinniPerfBenchmarkImpl::argArrayRecord(const std::vector<RecordSixInt>& /* a */) {}

int64_t DjinniPerfBenchmarkImpl::returnInt(int64_t value) {
    return value;
}

// Each test is run 1000 times. We cache return values,
// so that the construction cost is only paid by the first run.

std::string DjinniPerfBenchmarkImpl::returnString(int32_t size) {
    static int32_t cachedReturnValueSize;
    static std::string cachedReturnValue;
    if (size != cachedReturnValueSize) {
        cachedReturnValue = std::string(size, '@');
        cachedReturnValueSize = size;
    }
    return cachedReturnValue;
}

std::vector<uint8_t> DjinniPerfBenchmarkImpl::returnBinary(int32_t size) {
    static int32_t cachedReturnValueSize;
    static std::vector<uint8_t> cachedReturnValue;
    if (size != cachedReturnValueSize) {
        cachedReturnValue.clear();
        for (int32_t i = 0; i < size; i++) {
            cachedReturnValue.push_back(i);
        }
        cachedReturnValueSize = size;
    }
    return cachedReturnValue;
}

std::shared_ptr<ObjectNative> DjinniPerfBenchmarkImpl::returnObject() {
    static auto cachedReturnValue = std::make_shared<ObjectNativeImpl>();
    return cachedReturnValue;
}

std::vector<int64_t> DjinniPerfBenchmarkImpl::returnListInt(int32_t size) {
    static int32_t cachedReturnValueSize;
    static std::vector<int64_t> cachedReturnValue;
    if (size != cachedReturnValueSize) {
        cachedReturnValue.clear();
        for (int64_t i = 0; i < size; i++) {
            cachedReturnValue.push_back(i);
        }
        cachedReturnValueSize = size;
    }
    return cachedReturnValue;
}

std::vector<int64_t> DjinniPerfBenchmarkImpl::returnArrayInt(int32_t size) {
    return returnListInt(size);
}

std::vector<std::shared_ptr<ObjectNative>> DjinniPerfBenchmarkImpl::returnListObject(int32_t size) {
    static int32_t cachedReturnValueSize;
    static std::vector<std::shared_ptr<ObjectNative>> cachedReturnValue;
    if (size != cachedReturnValueSize) {
        cachedReturnValue.clear();
        for (int32_t i = 0; i < size; i++) {
            auto o = std::make_shared<ObjectNativeImpl>();
            cachedReturnValue.push_back(o);
        }
        cachedReturnValueSize = size;
    }
    return cachedReturnValue;
}

std::vector<RecordSixInt> DjinniPerfBenchmarkImpl::returnListRecord(int32_t size) {
    static int32_t cachedReturnValueSize;
    static std::vector<RecordSixInt> cachedReturnValue;
    if (size != cachedReturnValueSize) {
        cachedReturnValue.clear();
        for (int32_t i = 0; i < size; i++) {
            RecordSixInt rsi{1, 2, 3, 4, 5, 6};
            cachedReturnValue.push_back(rsi);
        }
        cachedReturnValueSize = size;
    }
    return cachedReturnValue;
}

std::vector<RecordSixInt> DjinniPerfBenchmarkImpl::returnArrayRecord(int32_t size) {
    return returnListRecord(size);
}

std::string DjinniPerfBenchmarkImpl::roundTripString(const std::string& s) {
    return s;
}

} // namespace snap::djinni_perf_benchmark
