var failed: string[];
var currentTest: string;

function println(o: string) {
    let output = document.getElementById('output');
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

function units(n: number, u: string) {
    u = (n == 1) ? u : u + 's';
    return n + ' ' + u;
}

export class TestCase {
    constructor(module) {}
    setUp() {}
    tearDown() {}
}

export function runTests(module: any, tests: Array<typeof TestCase>) {
    failed = [];
    var totalTests = 0;
    let runStart = performance.now();
    tests.forEach(testClass => {
        let testCase = new testClass(module);
        var count = 0;
        let testCaseStart = performance.now();
        Reflect.ownKeys(Reflect.getPrototypeOf(testCase)).forEach(method => {
            let methodName = method.toString();
            if (methodName.startsWith('test')) {
                currentTest = testClass.name + '.' + methodName;
                println('[ RUN      ] ' + currentTest)
                let numberOfFailsBeforeThisTest = failed.length;
                let testStart = performance.now();
                try {
                    testCase.setUp();
                    testCase[methodName]();
                    testCase.tearDown();
                }catch (err) {
                    console.log(err);
                    assertTrue(false);
                }
                let status = failed.length > numberOfFailsBeforeThisTest ? '[  FAILED  ] ' : '[       OK ] ';
                println(status + currentTest + ' (' + (performance.now() - testStart) + ' ms)')
                count++;
            }
        });
        totalTests += count;
        println('[----------] ' + units(count, 'test') + ' from ' + testClass.name + '(' + (performance.now() - testCaseStart) + ' ms total)')
    });
    println('[==========] ' + units(totalTests, 'test') + ' from ' + units(tests.length, 'test suite') + ' ran. (' + (performance.now() - runStart) + ' ms total)')
    println('[  PASSED  ] ' + units(totalTests - failed.length, 'test') + '.')
    if (failed.length > 0) {
        println('[  FAILED  ] ' + units(failed.length, 'test') + ', listed below:')
        failed.forEach(function(f) {
            println('[  FAILED  ] ' + f);
        });
    }
}

export function deepEqual(object1:any, object2:any) {
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

export function isObject(object) {
    return object != null && typeof object === 'object';
}
export function assertTrue(a: boolean) {
    if (!a) {
        console.error("[  FAILED  ] " + currentTest);
        failed.push(currentTest);
    }
}
export function assertFalse(a:boolean) {
    assertTrue(!a);
}
export function assertSame(a, b) {
    assertTrue(a === b);
}
export function assertNotSame(a, b) {
    assertFalse(a === b);
}
export function assertEq(a, b) {
    assertTrue(deepEqual(a, b));
}
export function assertNe(a, b) {
    assertFalse(deepEqual(a, b));
}
export function assertArrayEq(a, b) {
    assertEq(a.length, b.length);
    for (var i = 0; i < a.length; i++) {
        assertEq(a[i], b[i]);
    }
}
export function assertNull(a) {
    assertTrue (a === null);
}

export var allTests: Array<typeof TestCase> = [];
