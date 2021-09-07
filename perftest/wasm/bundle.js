(function(){function r(e,n,t){function o(i,f){if(!n[i]){if(!e[i]){var c="function"==typeof require&&require;if(!f&&c)return c(i,!0);if(u)return u(i,!0);var a=new Error("Cannot find module '"+i+"'");throw a.code="MODULE_NOT_FOUND",a}var p=n[i]={exports:{}};e[i][0].call(p.exports,function(r){var n=e[i][1][r];return o(n||r)},p,p.exports,r,e,n,t)}return n[i].exports}for(var u="function"==typeof require&&require,i=0;i<t.length;i++)o(t[i]);return o}return r})()({1:[function(require,module,exports){
"use strict";
exports.__esModule = true;
Module().then(function (module) {
    main(module);
});
function percentileFromSortedArray(samples, percentile) {
    return samples[Math.round((samples.length - 1) * percentile)];
}
function measure(name, func, times) {
    if (times === void 0) { times = 1000; }
    var tbl = (document.getElementById('tbl'));
    var row = (tbl.insertRow(-1));
    row.align = 'right';
    var nameCell = (row.insertCell(-1));
    nameCell.align = 'left';
    var avgCell = row.insertCell(-1);
    var sdCell = row.insertCell(-1);
    var minCell = row.insertCell(-1);
    var p50Cell = row.insertCell(-1);
    var p90Cell = row.insertCell(-1);
    var maxCell = row.insertCell(-1);
    var samples = [];
    for (var i = 0; i < times; ++i) {
        var t0 = performance.now();
        func();
        samples.push(performance.now() - t0);
    }
    samples.sort(function (a, b) { return a - b; });
    var sum = samples.reduce(function (a, b) { return a + b; }, 0);
    var avg = (sum / samples.length) || 0;
    var sqrSum = samples.reduce(function (a, b) { return (a + (b - avg) * (b - avg)); });
    var sd = Math.sqrt(sqrSum / samples.length);
    var min = percentileFromSortedArray(samples, 0);
    var p50 = percentileFromSortedArray(samples, 0.5);
    var p90 = percentileFromSortedArray(samples, 0.9);
    var max = percentileFromSortedArray(samples, 1);
    nameCell.innerText = name;
    avgCell.innerText = (avg * 1000).toFixed(3) + ' µs';
    sdCell.innerText = (sd * 1000).toFixed(3) + ' µs';
    minCell.innerText = (min * 1000).toFixed(3) + ' µs';
    p50Cell.innerText = (p50 * 1000).toFixed(3) + ' µs';
    p90Cell.innerText = (p90 * 1000).toFixed(3) + ' µs';
    maxCell.innerText = (max * 1000).toFixed(3) + ' µs';
}
var ObjectPlatformImpl = /** @class */ (function () {
    function ObjectPlatformImpl() {
    }
    ObjectPlatformImpl.prototype.onDone = function () { };
    return ObjectPlatformImpl;
}());
function main(module) {
    var minCount = 16;
    var lowCount = 128;
    var highCount = 4096;
    var hugeCount = 16384;
    var i64Array = { i1: BigInt(1), i2: BigInt(2), i3: BigInt(3), i4: BigInt(4), i5: BigInt(5), i6: BigInt(6) };
    var dpb = module.DjinniPerfBenchmark.getInstance();
    measure('memcpy256b', function () { dpb.cppTests(); });
    measure("baseline", function () { dpb.baseline(); });
    var s = "1234567890ABCDEF"; // minCount
    measure("argString " + minCount, function () { dpb.argString(s); });
    var sl = "";
    for (var i = 0; i < (lowCount / minCount); ++i) {
        sl = sl + s;
    }
    measure("argString " + lowCount, function () { dpb.argString(sl); });
    var sh = "";
    for (var i = 0; i < (highCount / lowCount); ++i) {
        sh = sh + sl;
    }
    measure("argString " + highCount, function () { dpb.argString(sh); });
    var sg = "";
    for (var i = 0; i < (hugeCount / highCount); ++i) {
        sg = sg + sh;
    }
    measure("argString " + hugeCount, function () { dpb.argString(sg); });
    [lowCount, highCount, hugeCount].forEach(function (count) {
        measure("argBinary " + count, function () { dpb.argBinary(new Uint8Array(count)); });
    });
    var e = 0 /* FIRST */;
    measure("argEnumSixValue", function () { dpb.argEnumSixValue(e); });
    var r = i64Array;
    measure("argRecordSixInt", function () { dpb.argRecordSixInt(r); });
    var li = [];
    for (var i = 0; i < lowCount; ++i) {
        li.push(BigInt(i));
    }
    measure("argListInt " + lowCount, function () { dpb.argListInt(li); });
    var o = new ObjectPlatformImpl();
    measure("argObject", function () { dpb.argObject(o); });
    var lo = [];
    for (var i = 0; i < lowCount; ++i) {
        lo.push(new ObjectPlatformImpl());
    }
    measure("argListObject " + lowCount, function () { dpb.argListObject(lo); });
    var lr = [];
    for (var i = 0; i < lowCount; ++i) {
        lr.push(i64Array);
    }
    measure("argListRecord " + lowCount, function () { dpb.argListRecord(lr); });
    var ar = [];
    for (var i = 0; i < lowCount; ++i) {
        ar.push(i64Array);
    }
    measure("argArrayRecord " + lowCount, function () { dpb.argArrayRecord(ar); });
    measure("returnInt", function () { var ri = dpb.returnInt(BigInt(42)); });
    [1, 10, lowCount].forEach(function (count) {
        measure("returnListInt " + count, function () { var rli = dpb.returnListInt(count); });
    });
    [1, 10, lowCount].forEach(function (count) {
        measure("returnArrayInt " + count, function () { var rai = dpb.returnArrayInt(count); });
    });
    [minCount, lowCount, highCount, hugeCount].forEach(function (count) {
        measure("returnString " + count, function () { var rs = dpb.returnString(count); });
    });
    [lowCount, highCount, hugeCount].forEach(function (count) {
        measure("returnBinary " + count, function () { var rb = dpb.returnBinary(count); });
    });
    measure("returnObject", function () { var ro = dpb.returnObject(); });
    [10, 100].forEach(function (count) {
        measure("returnListObject " + count, function () { var rlo = dpb.returnListObject(count); });
    });
    [1, 10, lowCount].forEach(function (count) {
        measure("returnListRecord " + count, function () { var rlr = dpb.returnListRecord(count); });
    });
    [1, 10, lowCount].forEach(function (count) {
        measure("returnArrayRecord " + count, function () { var rar = dpb.returnArrayRecord(count); });
    });
}

},{}]},{},[1]);
