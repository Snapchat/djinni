# Snapchat Djinni

See the [original dropbox readme](README.dropbox.md) for the full Djinni docs.

## Working on Djinni in an IDE

You can load the project via Bazel

### Bazel

- Intellij with the Bazel and Scala plugin is required
- Configure the bazel binary, or download [Bazelisk](https://github.com/bazelbuild/bazelisk)
  and set it as the binary in the IDEA bazel settings.
- In Intellij, import a new Bazel project.
    - Workspace directory: `/Users/$HOME/Snapchat/Dev/client/tools/sc-djinni`
    - Import project view file: `WORKSPACE/bzl/ide/djinni.bazelproject`
- Similarly you can also use CLion if you wish to edit the C++ code
    - You can set up any of the cc_* targets after importing the workspace.

You can also use `bazel build //src:djinni` and `bazel run //src:djinni` to verify the build and binary from the command line.

## Future Work

- Add a proper djinni rule to bazel for generation
  - Main blocker here is djinni does not have deterministic outputs and bazel doesn't support this

## Modifications

 - Replaced sbt and gyp with Bazel
 - Added move assigment operator to GlobalRef in all djinni_support.hpp files
 - Made JniClassInitializer constructor public to allow arbitrary additional initialization in JNI_OnLoad()
 - Speed up string passing between Java and C++
 - Eliminate CppProxy finalizers
 - Injecting code with `DJINNI_FUNCTION_PROLOGUE`
 - Option to generate ObjC protocols
 - Option to generate function prologue
 - Option to omit objc helper methods
 - Option to disable exception translation in ObjC
 - outcome<> type support
 - Protobuf type support

## Using new features

### Injecting code to Djinni calls

sc-djinni introduced two new switches:

```
--jni-function-prologue-file "path/to/header_file.h"
--objcpp-function-prologue-file "path/to/header_file.h"
```

If they are specified at the command line, then all generated Djinni calls will
contain a macro at the beginning of the bridge function:

```
DJINNI_FUNCTION_PROLOGUE("DjinniInterfaceName.methodName");
```

You should then `#define` the `DJINNI_FUNCTION_PROLOGUE` macro in the specified
header file to inject whatever code you need there.

For example you could make it log the method name. Or you could instantiate a
scoped object to log the duration of the call.

### Generate all interfaces as ObjC protocols

By default, Djinni only generates interfaces as ObjC `@protocol` when these
interfaces are tagged with `+o`.  Otherwise they become `@interface` in ObjC.

sc-djinni introduces a new switch to override the default:

`--objc-gen-protocol true`

Enabling this will make all Djinni interfaces `@protocol` in ObjC. This means
you can build your code without linking with the native code implementations,
for example, when you want to mock the interfaces in pure ObjC for testing.

### Prohibit some ObjC helper methods to minimize binary size

You can use `--objc-disable-class-ctor true` to remove the static constructors
from Djinni generated classes.

You can also remove the `description` method in Djinni generated classes by
defining the `DJINNI_DISABLE_DESCRIPTION_METHODS` preprocessor symbol.

These help reducing the binary size of the app slightly.

### Disable C++ exception translation in ObjC

You can use `--objcpp-disable-exception true` to disable the translation of C++
exceptions to ObjC.  This means ObjC code will see C++ exceptions directly.

This is sometimes preferrable compared to the default behaviour which converts
C++ exceptions to ObjC NSException, because the original exception stack is
preserved.

### outcome<> builtin type

The `outcome<>` type provides a standard way to explicitly define error
conditions in Djinni IDL.  We believe this is a superior approach than throwing
exceptions across language boundaries.

An outcome is defined by combining a "result" type and an "error" type:
```
outcome<RESULT, ERROR>
```
Where RESULT and ERROR can be any Djinni types (primitives or records).


In C++, the `outcome<>` type maps to the template class `djinni::expected<>`.
In Java, it maps to the generic class `com.snapchat.djinni.Outcome<>`. In ObjC,
it maps to the generic class `DJOutcome<>`.

### Use Protobuf types in Djinni

We support using Protobuf types directly in Djinni IDL. In order to use Protobuf
types, you first need to declare them in a yaml manifest file, in which we specify

- How to find the header file and namespace/class/prefix
- List the Protobuf messages you want to use in Djinni

```
cpp:
    header: '"proto/cpp/test.pb.h"'
    namespace: 'djinni::test'
java:
    class: 'djinni.test.Test'
objc:
    header: '"proto/objc/test.pbobjc.h"'
    prefix: 'DJTest'
 
messages:
    - Person
    - AddressBook
```

After that you can reference it in Djinni IDL and use the types as if they are
regular Djinni types:

```
@protobuf "my_proto.yaml"
...
MyRecordWithProto = record {
    // Yay! proto!
    person: Person;
}

MyInterface = interface +c {
   // Yay! proto!
    static foo(x: AddressBook);
}
```
