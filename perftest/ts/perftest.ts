import * as perftest from "../generated-src/ts/perftest_d";
import {DjinniModule} from "@djinni_support/DjinniModule"
declare function Module(): Promise<perftest.Module_statics & DjinniModule>;

Module().then(module => {
    main(module);
})

function percentileFromSortedArray(samples, percentile) {
    return samples[Math.round((samples.length - 1) * percentile)];
}

function measure(name, func, times = 10) {
    var tbl = <HTMLTableElement>(document.getElementById('tbl'));
    var row = <HTMLTableRowElement>(tbl.insertRow(-1));
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
        for (var j = 0; j < 100; ++j) {
            func();
        }
        samples.push((performance.now() - t0)/100);
    }
    samples.sort(function(a, b){return a - b});

    const sum = samples.reduce((a, b) => a + b, 0);
    const avg = (sum / samples.length) || 0;
    const sqrSum = samples.reduce((a, b) => (a + (b - avg) * (b - avg)));
    const sd = Math.sqrt(sqrSum / samples.length);
    const min = percentileFromSortedArray(samples, 0);
    const p50 = percentileFromSortedArray(samples, 0.5);
    const p90 = percentileFromSortedArray(samples, 0.9);
    const max = percentileFromSortedArray(samples, 1);
    
    nameCell.innerText = name;
    avgCell.innerText = (avg*1000).toFixed(3) + ' µs';
    sdCell.innerText = (sd*1000).toFixed(3) + ' µs';
    minCell.innerText = (min*1000).toFixed(3) + ' µs';
    p50Cell.innerText = (p50*1000).toFixed(3) + ' µs';
    p90Cell.innerText = (p90*1000).toFixed(3) + ' µs';
    maxCell.innerText = (max*1000).toFixed(3) + ' µs';
}

class ObjectPlatformImpl {
    onDone() {}
}

function main (module: perftest.Module_statics & DjinniModule) {
    var minCount = 16;
    var lowCount = 128;
    var highCount = 4096;
    var hugeCount = 16384;

    var i64Array = {i1: BigInt(1), i2: BigInt(2), i3: BigInt(3), i4: BigInt(4), i5: BigInt(5), i6: BigInt(6)};

    var dpb = module.DjinniPerfBenchmark.getInstance();
    measure('memcpy256b', function(){ dpb.cppTests(); });
    measure("baseline", function() { dpb.baseline(); });

    [lowCount, highCount, hugeCount].forEach(function(count) {
        let bb = module.allocateWasmBuffer(count);
        for (let i = 0; i < count; i++) { bb[i] = i; }
        measure("argDataView " + count, function() {dpb.argDataView(bb);});
    });
    
    [lowCount, highCount, hugeCount].forEach(function(count) {
        let bb = module.allocateWasmBuffer(count);
        for (let i = 0; i < count; i++) { bb[i] = i; }
        measure("argDataRef " + count, function() {dpb.argDataRef(bb);});
    });

    var s = "1234567890ABCDEF" // minCount
    measure("argString " + minCount, function() { dpb.argString(s); });

    var sl = ""
    for (var i = 0; i < (lowCount/minCount); ++i) {sl = sl + s;}
    measure("argString " + lowCount, function(){dpb.argString(sl)});

    var sh = ""
    for (var i = 0; i < (highCount/lowCount); ++i) {sh = sh + sl;}
    measure("argString " + highCount, function(){dpb.argString(sh)});

    var sg = ""
    for (var i = 0; i < (hugeCount/highCount); ++i) {sg = sg + sh;}
    measure("argString " + hugeCount, function() {dpb.argString(sg)});

    [/*lowCount, highCount,*/ hugeCount].forEach(function(count) {
        var u8array = new Uint8Array(count);
        measure("argBinary " + count, function(){dpb.argBinary(u8array)});
    });

    var li = [];
    for (var i = 0; i < highCount; ++i) {li.push(BigInt(i));}
    var ai = new BigInt64Array(li);
    measure("argArrayInt " + highCount, function() {dpb.argArrayInt(ai)});

    var e = perftest.EnumSixValue.FIRST;
    measure("argEnumSixValue", function() {dpb.argEnumSixValue(e)});

    var r = i64Array;
    measure("argRecordSixInt", function() {dpb.argRecordSixInt(r)});

    var li = [];
    for (var i = 0; i < lowCount; ++i) {li.push(BigInt(i));}
    measure("argListInt " + lowCount, function() {dpb.argListInt(li)});

    var o = new ObjectPlatformImpl();
    measure("argObject", function(){dpb.argObject(o)});

    var lo = []
    for (var i = 0; i < lowCount; ++i) {lo.push(new ObjectPlatformImpl())}
    measure("argListObject " + lowCount, function() {dpb.argListObject(lo)});

    var lr = []
    for (var i = 0; i < lowCount; ++i) {lr.push(i64Array)}
    measure("argListRecord " + lowCount, function(){dpb.argListRecord(lr)});

    var ar = []
    for (var i = 0; i < lowCount; ++i) {ar.push(i64Array)}
    measure("argArrayRecord " + lowCount, function(){dpb.argArrayRecord(ar)});

    measure("returnInt", function() {var ri = dpb.returnInt(BigInt(42))});

    [1, 10, lowCount].forEach(function(count) {
        measure("returnListInt " + count, function(){ var rli = dpb.returnListInt(count)});
    });

    [1, 10, lowCount, highCount].forEach(function(count) {
        measure("returnArrayInt " + count, function(){ var rai = dpb.returnArrayInt(count)});
    });

    [minCount, lowCount, highCount, hugeCount].forEach(function(count) {
        measure("returnString " + count, function() { var rs = dpb.returnString(count)});
    });

    [lowCount, highCount, hugeCount].forEach(function(count) {
        measure("returnBinary " + count, function() { var rb = dpb.returnBinary(count)});
    });

    measure("returnObject", function() { var ro = dpb.returnObject()});

    [10, 100].forEach(function(count) {
        measure("returnListObject " + count, function(){var rlo = dpb.returnListObject(count)});
    });

    [1, 10, lowCount].forEach(function(count) {
        measure("returnListRecord " + count, function(){var rlr = dpb.returnListRecord(count)});
    });

    [1, 10, lowCount].forEach(function(count) {
        measure("returnArrayRecord " + count, function(){var rar = dpb.returnArrayRecord(count)});
    });
}
