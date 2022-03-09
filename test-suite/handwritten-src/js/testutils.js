var failed;
var currentTest;

function println(o) {
    var output = document.getElementById('output');
    if (o.startsWith('[  FAILED  ]')) {
        var red = document.createElement("SPAN")
        red.style.color="red";
        red.innerText += String(o) + '\n';
        output.appendChild(red)
    } else if (o.startsWith('[')) {
        var green = document.createElement("SPAN")
        green.style.color="green";
        green.innerText += String(o) + '\n';
        output.appendChild(green)
    } else {
        var txt = document.createTextNode(String(o) + '\n')
        output.appendChild(txt)
    }
}

function units(n, u) {
    u = (n == 1) ? u : u + 's';
    return n + ' ' + u;
}

async function runTests(module, tests) {
    failed = [];
    var total = 0;
    var t2 = performance.now();
    for (const cls of tests) {
        var t = new cls(module);
        var count = 0;
        var t1 = performance.now();
        for (const m of Reflect.ownKeys(Reflect.getPrototypeOf(t))) {
            if (m.startsWith('test')) {
                currentTest = cls.name + '.' + m;
                println('[ RUN      ] ' + currentTest)
                var failedBefore = failed.length;
                var t0 = performance.now();
                try {
                    if (t['setUp'] !== undefined) {t.setUp();}
                    var r = t[m]();
                    if (typeof r === 'object' && typeof r.then === 'function') {
                        await r;
                    }
                    if (t['tearDown'] !== undefined) {t.tearDown();}
                } catch (err) {
                    console.log("C++ exception: " + err);
                    assertTrue(false);
                }
                var status = failed.length > failedBefore ? '[  FAILED  ] ' : '[       OK ] ';
                println(status + cls.name + '.' + m + ' (' + (performance.now() - t0) + ' ms)')
                count++;
            }
        }
        total += count;
        println('[----------] ' + units(count, 'test') + ' from ' + cls.name + '(' + (performance.now() - t1) + ' ms total)')
    }
    println('[==========] ' + units(total, 'test') + ' from ' + units(tests.length, 'test suite') + ' ran. (' + (performance.now() - t2) + ' ms total)')
    println('[  PASSED  ] ' + units(total - failed.length, 'test') + '.')
    if (failed.length > 0) {
        println('[  FAILED  ] ' + units(failed.length, 'test') + ', listed below:')
        failed.forEach(function(f) {
            println('[  FAILED  ] ' + f);
        });
    }
}

function deepEqual(object1, object2) {
    if (!isObject(object1) || !isObject(object2)) {
        return object1 == object2;
    }

    const keys1 = Object.keys(object1);
    const keys2 = Object.keys(object2);

    if (keys1.length !== keys2.length) {
        return false;
    }

    for (const key of keys1) {
        const val1 = object1[key];
        const val2 = object2[key];
        const areObjects = isObject(val1) && isObject(val2);
        if (areObjects && !deepEqual(val1, val2) || !areObjects && val1 !== val2) {
            return false;
        }
    }

    return true;
}

function isObject(object) {
    return object != undefined && object != null && typeof object === 'object';
}

function assertTrue(a) {
    if (!a) {
        console.error("[  FAILED  ] " + currentTest);
        failed.push(currentTest);
    }
}
function assertFalse(a) {
    assertTrue(!a);
}
function assertSame(a, b) {
    assertTrue(a === b);
}
function assertNotSame(a, b) {
    assertFalse(a === b);
}
function assertEq(a, b) {
    assertTrue(deepEqual(a, b));
}
function assertNe(a, b) {
    assertFalse(deepEqual(a, b));
}
function assertArrayEq(a, b) {
    assertEq(a.length, b.length);
    for (var i = 0; i < a.length; i++) {
        assertEq(a[i], b[i]);
    }
}
function assertUndefined(a) {
    assertTrue (a === undefined);
}

var allTests = [];
