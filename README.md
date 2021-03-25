# Snapchat Djinni

See the [original dropbox readme](README.dropbox.md) for the full Djinni docs.

## Building

Both the Djinni code generator and test suite are built with Bazel. You can use
either plain Bazel or [Bazelisk](https://github.com/bazelbuild/bazelisk).

### Building and running the test suite

`bazel test //test-suite:all` builds and then runs all tests.

Or `bazel test //test-suite:djinni-objc-tests` and `bazel test
//test-suite:djinni-java-tests` to run Objective-C and Java tests separately.

### Building and running the mobile example apps

The Android example app can be build with bazel: `bazel build
//examples:android-app`, and then install to a device with `adb install
bazel-bin/examples/android-app.apk`

The iOS example app are built with Xcode. Simply open the project in Xcode and
it should work.

### Working on the Djinni code generator

You can load the project via Bazel

- Intellij with the Bazel and Scala plugin is required
- Configure the bazel binary. If you use Bazelisk, set it as the binary in the IDEA bazel settings.
- In Intellij, import a new Bazel project.
    - Workspace directory: `/Users/$HOME/path-to-djinni-directory`
    - Import project view file: `WORKSPACE/bzl/ide/djinni.bazelproject`
- Similarly you can also use CLion if you wish to edit the C++ code
    - You can set up any of the cc_* targets after importing the workspace.

You can also use `bazel build //src:djinni` and `bazel run //src:djinni` to verify the build and binary from the command line.

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
 - array<> type support
 - outcome<> type support
 - Protobuf type support
 - Local flags with `@flag` directive

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

You can use `--objcpp-disable-exception-translation true` to disable the
translation of C++ exceptions to ObjC.  This means ObjC code will see C++
exceptions directly.

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

### Optimize primitive array with array<>

The new array<> type is similar to list<>, but optimized for Java primitive
arrays. It is mapped to regular Java arrays (T[]) instead of ArrayList. Since it
doesn't have to box elements as objects, it's order of magnitude faster than
list<> when the element type is primitive.

array<> is identical to list<> in Objective-C because there is no managed
primitive array in Objective-C.

### Local flags with `@flag` directive

In addition to supplying switches on the Djinni command line, it's also possible
to specify them in the idl file with the `@flag` directive.

For example, adding the line

```
@flag "--objc-gen-protocol true"
```

to the djinni idl file will enable generation of objc protocols for
interfaces. This is equivalent to adding the same switches to the djinni command
line.

- Multiple `@flag` lines can appear in the same file.
- `@flag` lines must appear at the top of the djinni idl file before anything
  else.
- `@import`ed files can include `@flag` lines too, and they will apply to the
  parent file. (this means you can put common flags in a file and `@import` it)

### FAQ

Q. Can we include arbitrary bytes in the `string` type?

A. No! iOS and Java strings are unicode, so arbitrary bytes won't necessarily be
able to get translated into such strings.

Q. How many Djinni objects can I have?

A. There is no limit in iOS.  But on Android, JNI has a limit on the number of
global references you can create (usually ~64k). Each `interface` type Djinni
object (either native object in java or java object in native) takes up one
global reference. `record` type objects do not take up global references.
