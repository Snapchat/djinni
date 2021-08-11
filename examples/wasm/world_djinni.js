// ---- Library code
const djinniFinalizerRegistry = new FinalizationRegistry(nativeRef => {
    console.log("finalizing " + nativeRef);
    nativeRef.nativeDestroy();
    nativeRef.delete();
});
Module.djinniFinalizerRegistry = djinniFinalizerRegistry;

// ---- Generated javascript
class MyInterface_CppProxy {
    // take over ownership
    constructor(nativeRef) {
        console.log('new cpp proxy');
        this._nativeRef = nativeRef;
    }
    foo(x) {
        return this._nativeRef.foo(x);
    }
}
Module.MyInterface_CppProxy = MyInterface_CppProxy;

// ---- User code
class MyInterfaceJS {
    foo(x) {
        console.log("MyInterfaceJS.foo(" + x + ")");
    }
}

let main = function() {
    var i = Module.MyInterface.create();
    i.foo(42);
    var k = new MyInterfaceJS();
    var j = Module.MyInterface.pass(k);
    j.foo(33);
    if (k === j) {
        console.log('same');
    } else {
        console.log('not same');
    }
    if (Module.MyInterface.comp(k, k)) {
        console.log('same');
    } else {
        console.log('not same');
    }
    i = Module.MyInterface.instance();
    j = Module.MyInterface.instance();
    if (i === j) {
        console.log('same');
    } else {
        console.log('not same');
    }
}
