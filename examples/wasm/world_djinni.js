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
    testStr(x) {
        return this._nativeRef.testStr(x);
    }
}
Module.MyInterface_CppProxy = MyInterface_CppProxy;

// ---- User code
class MyInterfaceJS {
    foo(x) {
        console.log("MyInterfaceJS.foo(" + x + ")");
    }
    testStr(x) {
        console.log("MyInterfaceJS.testStr(" + x + ")");
    }
}

function same(a, b, desc) {
    if (a === b) {
        console.log(desc + ': yes');
    } else {
        console.log(desc + ': no');
    }
}

function same_c(a, b, desc) {
    if (Module.MyInterface.comp(a, b)) {
        console.log(desc + ': yes');
    } else {
        console.log(desc + ': no');
    }
}

let main = function() {
    {
        var i = Module.MyInterface.instance();
        var j = Module.MyInterface.instance();
        same(i, j, "same c++ object passed to js twice");
    }

    {
        var i = Module.MyInterface.create();
        var j = i;
        same_c(i, j, "cpp object to js and back");
    }

    {
        var i = new MyInterfaceJS();
        var j = new MyInterfaceJS();
        same_c(i, j, "same js object passed to cpp twice");
        same_c(i, i, "same js object passed to cpp twice");
    }

    {
        var i = new MyInterfaceJS();
        var j = Module.MyInterface.pass(i);
        same(i, j, "js object to cpp and back");
    }

    {
        var i = Module.MyInterface.instance();
        console.log("got string: " + i.testStr("hello"));
    }
}
