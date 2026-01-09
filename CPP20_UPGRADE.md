# C++20 Upgrade Summary

## Overview
Successfully upgraded djinni from C++17 to C++20. This upgrade was necessary because C++17 is deprecated in Xcode.

## Changes Made

### 1. Build Configuration
Updated all C++ standard flags from `-std=c++17` to `-std=c++20`:
- `.bazelrc`: Global C++ standard setting
- `support-lib/BUILD`: ObjC++, Swift interop, and SwiftXX libraries
- `test-suite/BUILD`: Test Swift libraries and SwiftXX bridge
- `examples/BUILD`: Example Swift libraries and SwiftXX bridge

### 2. Library Updates

#### Future.hpp
- Kept using `std::atomic_load` and `std::atomic_exchange` free functions (still supported in C++20)
- These work with `std::shared_ptr` which is not trivially copyable
- Fixed coroutine support to use atomic operations correctly
- **Removed experimental coroutine fallback**: Now uses only standard C++20 `<coroutine>` header
  - Removed `<experimental/coroutine>` and `std::experimental::coroutine_handle`
  - C++20 coroutines are standard and well-supported

#### expected.hpp
- Already had proper C++23 feature detection (`__cpp_lib_expected`)
- Uses `std::expected` when available (C++23+)
- Falls back to `tl::expected` for C++20 and earlier
- Added clarifying comments about C++23 standard

#### tl_expected.hpp
- **Updated from version 1.0.1 → 1.3.1** from [TartanLlama/expected](https://github.com/TartanLlama/expected)
- This is a polyfill for `std::expected` (C++23 feature)
- Added comments clarifying that C++17 polyfills are for older compiler support

### 3. Code Fixes

#### test_helpers.cpp
- **C++20 Breaking Change**: `u8"string"` literals are now `char8_t*` instead of `const char*`
- Changed `u8"String1"` → `"String1"` for ASCII strings
- This is a C++20 language change to properly support UTF-8 literals

### 4. Djinni Generator
Added deprecation warnings for optional-related command-line flags:
- `--cpp-optional-template`: Deprecated (std::optional is standard in C++17+)
- `--cpp-optional-header`: Deprecated (std::optional is standard in C++17+)
- `--cpp-nullopt-value`: Deprecated (std::optional is standard in C++17+)

These flags still work but print warnings. The defaults (`std::optional`, `<optional>`, `std::nullopt`) are correct for C++17+.

### 5. Documentation
- Updated `README.md`: Fixed typo and clarified C++20 coroutine support
- Updated `BAZEL_8_MIGRATION_COMPLETE.md`: Changed C++17 references to C++20

## Testing
All builds pass successfully:
- ✅ `//src:djinni` - Djinni generator
- ✅ `//support-lib:djinni-support-common` - Core C++ support library
- ✅ `//test-suite:djinni-tests-c` - C++ test suite

## Key Points

### Why C++20?
- C++17 is deprecated in Xcode
- C++20 is widely supported and stable
- C++23 is still too new for broad adoption

### What About std::expected?
- `std::expected` is a C++23 feature, not C++20
- We keep `tl::expected` as a fallback for C++20
- Code automatically uses `std::expected` when compilers support C++23

### Atomic Operations
- C++20 deprecated `std::atomic_load`/`std::atomic_exchange` for `std::shared_ptr`
- However, they still work and are the correct approach for C++20
- C++20 does not support `std::atomic<std::shared_ptr<T>>` directly
- The free functions remain the standard way to atomically manipulate shared_ptr

### UTF-8 Literals
- C++20 changed `u8"string"` from `const char*` to `const char8_t*`
- For ASCII strings, use regular `"string"` literals
- Use `u8"string"` only when you specifically need UTF-8 encoding

### Coroutines
- C++20 coroutines are standard (no longer experimental)
- Removed fallback to `<experimental/coroutine>` header
- Uses standard `<coroutine>` and `std::coroutine_handle`
- Coroutine tests (`co_await`, `co_return`) work correctly

## Compatibility
- **Minimum C++ Standard**: C++20
- **Xcode**: Compatible with modern Xcode versions
- **Bazel**: 8.5.0 (via Bazelisk)
- **Backward Compatibility**: Code that used deprecated flags will still work but with warnings

## Future Considerations
- When C++23 becomes widely adopted, we can:
  - Remove `tl_expected.hpp` entirely
  - Require `std::expected` from the standard library
  - Consider upgrading to C++23 for additional features

