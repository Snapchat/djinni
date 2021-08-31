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

function runTests(module, tests) {
    failed = [];
    var total = 0;
    var t2 = performance.now();
    tests.forEach(function(cls) {
        var t = new cls(module);
        var count = 0;
        var t1 = performance.now();
        Reflect.ownKeys(Reflect.getPrototypeOf(t)).forEach(function(m) {
            if (m.startsWith('test')) {
                currentTest = cls.name + '.' + m;
                println('[ RUN      ] ' + currentTest)
                var failedBefore = failed.length;
                var t0 = performance.now();
                if (t['setUp'] !== undefined) {t.setUp();}
                try {
                    t[m]();
                } catch (err) {
                    console.log(err);
                    assertTrue(false);
                }
                if (t['tearDown'] !== undefined) {t.tearDown();}
                var status = failed.length > failedBefore ? '[  FAILED  ] ' : '[       OK ] ';
                println(status + cls.name + '.' + m + ' (' + (performance.now() - t0) + ' ms)')
                count++;
            }
        });
        total += count;
        println('[----------] ' + units(count, 'test') + ' from ' + cls.name + '(' + (performance.now() - t1) + ' ms total)')
    });
    println('[==========] ' + units(total, 'test') + ' from ' + units(tests.length, 'test suite') + ' ran. (' + (performance.now() - t2) + ' ms total)')
    println('[  PASSED  ] ' + units(total - failed.length, 'test') + '.')
    if (failed.length > 0) {
        println('[  FAILED  ] ' + units(failed.length, 'test') + ', listed below:')
        failed.forEach(function(f) {
            println('[  FAILED  ] ' + f);
        });
    }
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
function assertEq(a, b) {
    assertTrue (a === b);
}
function assertNe(a, b) {
    assertTrue (a !== b);
}
function assertNull(a) {
    assertTrue (a === null);
}
