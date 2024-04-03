/**
  * Copyright 2024 Snap, Inc.
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *    http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */

#include "djinni_composer.hpp"

#include "composer_core/cpp/Utils/ValueTypedArray.hpp"

namespace djinni::composer {

using namespace Composer;

std::unordered_map<ComposerProxyId, std::weak_ptr<ComposerProxyBase>> jsProxyCache;
std::unordered_map<void*, CppProxyCacheEntry> cppProxyCache;
std::mutex jsProxyCacheMutex;
std::mutex cppProxyCacheMutex;

void checkForNull(void* ptr, const char* context) {
    if (!ptr) {
        throw std::invalid_argument(std::string("nullptr is not allowed in ") + context);
    }
}

ValueSchema resolveSchema(const ValueSchema& unresolved, std::function<void()> registerSchemaFunc) noexcept {
    registerSchemaFunc();
    Composer::ValueSchemaTypeResolver resolver(Composer::ValueSchemaRegistry::sharedInstance().get());
    auto result = resolver.resolveTypeReferences(unresolved);
    result.ensureSuccess();
    return result.moveValue();
}

void registerSchemaImpl(const ValueSchema& schema, bool resolve) noexcept {
    auto registry = ValueSchemaRegistry::sharedInstance();
    if (!resolve) {
        registry->registerSchema(schema);
    } else {
        Composer::ValueSchemaTypeResolver resolver(registry.get());
        auto result = resolver.resolveTypeReferences(schema);
        result.ensureSuccess();
        auto key = ValueSchemaRegistryKey(ValueSchema::typeReference(ValueSchemaTypeReference::named(schema.getClass()->getClassName())));
        registry->updateSchemaIfKeyExists(key, result.moveValue());
    }
}

Binary::CppType Binary::toCpp(const Binary::ComposerType& v) noexcept {
    auto a = v.getTypedArrayRef();
    auto buffer = a->getBuffer();
    return CppType(buffer.begin(), buffer.end());
}

Binary::ComposerType Binary::fromCpp(const Binary::CppType& c) noexcept {
    auto bytes = makeShared<Bytes>();
    bytes->assignData(c.data(), c.size());
    auto va = makeShared<ValueTypedArray>(TypedArrayType::Uint8Array, bytes);
    return Value(va);
}

const ValueSchema& Binary::schema() noexcept {
    static auto schema = ValueSchema::valueTypedArray();
    return schema;
}

Date::CppType Date::toCpp(const Date::ComposerType& v) noexcept {
    auto millisecondsSinceEpoch = std::chrono::milliseconds(static_cast<int64_t>(v.toDouble()));
    return CppType(std::chrono::duration_cast<std::chrono::system_clock::duration>(millisecondsSinceEpoch));
}

Date::ComposerType Date::fromCpp(const Date::CppType& c) noexcept {
    auto millisecondsSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(c.time_since_epoch());
    return Composer::Value(static_cast<double>(millisecondsSinceEpoch.count()));
}

const ValueSchema& Date::schema() noexcept {
    static auto schema = ValueSchema::date();
    return schema;
}

} // namespace djinni
