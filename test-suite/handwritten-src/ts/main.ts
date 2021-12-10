import {DjinniModule} from "@djinni_support/DjinniModule"
import {runTests, allTests} from "./testutils"
import "./ArrayTest"
import "./AsyncTest"
import "./ClientInterfaceTest"
import "./ConstantsTest"
import "./CppExceptionTest"
import "./DataTest"
import "./DurationTest"
import "./EnumTest"
import "./MapRecordTest"
import "./NestedCollectionTest"
import "./OutcomeTest"
import "./PrimitiveListTest"
import "./PrimitivesTest"
import "./ProtoTest"
import "./SetRecordTest"
import "./TokenTest"
import "./WcharTest"

declare function Module(): Promise<DjinniModule>;
Module().then(m => {
    runTests(m, allTests);
})
