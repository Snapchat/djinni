import {runTests, allTests} from "./testutils"
import "./ArrayTest"
import "./ClientInterfaceTest"
import "./CppExceptionTest"
import "./DataTest"
import "./DurationTest"
import "./EnumTest"
import "./MapRecordTest"
import "./NestedCollectionTest"
import "./OutcomeTest"
import "./PrimitiveListTest"
import "./PrimitivesTest"
import "./SetRecordTest"
import "./TokenTest"
import "./WcharTest"

declare function Module(): Promise<any>;
Module().then(m => {
    runTests(m, allTests);
})
