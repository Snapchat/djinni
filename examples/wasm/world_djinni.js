// ---- Library code
Module.cppProxyFinalizerRegistry = new FinalizationRegistry(nativeRef => {
    console.log("finalizing cpp object");
    nativeRef.nativeDestroy();
    nativeRef.delete();
});

// ---- Generated javascript
class MyInterface_CppProxy {
    // take over ownership
    constructor(nativeRef) {
        console.log('new cpp proxy');
        this._djinni_native_ref = nativeRef;
    }
    foo(x) {
        return this._djinni_native_ref.foo(x);
    }
    testStr(x) {
        return this._djinni_native_ref.testStr(x);
    }
    testBin(x) {
        return this._djinni_native_ref.testBin(x);
    }
    testDate(x) {
        return this._djinni_native_ref.testDate(x);
    }
    testRecord(x) {
        return this._djinni_native_ref.testRecord(x);
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
    testBin(x) {
        console.log("MyInterfaceJS.testBin(" + x + ")");
    }
    testDate(x) {
        console.log("MyInterfaceJS.testDate(" + x + ")");
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
    console.log("-------- 1")
    {
        var i = Module.MyInterface.instance();
        var j = Module.MyInterface.instance();
        same(i, j, "same c++ object passed to js twice");
    }

    console.log("-------- 2")
    {
        var i = Module.MyInterface.create();
        var j = i;
        same_c(i, j, "cpp object to js and back");
    }

    console.log("-------- 3")
    {
        var i = new MyInterfaceJS();
        var j = new MyInterfaceJS();
        same_c(i, j, "different js object passed to cpp");
        same_c(i, i, "same js object passed to cpp twice");
    }

    console.log("-------- 4")
    {
        var i = new MyInterfaceJS();
        var j = Module.MyInterface.pass(i);
        same(i, j, "js object to cpp and back");
    }

    console.log("-------- 5")
    {
        var i = Module.MyInterface.instance();
        console.log("got string: " + i.testStr("hello"));
    }

    console.log("-------- 6")
    {
        var i = Module.MyInterface.instance();
        i.foo(32);
    }

    console.log("-------- 7")
    {
        var i = Module.MyInterface.instance();
        var v = i.testBin(new Uint8Array([1,2,3]));
        console.log(v);
    }

    console.log("-------- 8")
    {
        var d = new Date()
        console.log(d);
        var i = Module.MyInterface.instance();
        d = i.testDate(d);
        console.log(d);
    }

    console.log("-------- 9")
    {
        var i = Module.MyInterface.instance();
        var r = { _x: 100, _y: "hello" };
        console.log(i.testRecord(r));
    }

    console.log("-------- 10")
}
