"use strict";
exports.__esModule = true;
var example = require("./example");
var sorter;
example.load().then(function (m) {
    sorter = m.SortItems.createWithListener(new SortListener());
});
var SortListener = /** @class */ (function () {
    function SortListener() {
    }
    SortListener.prototype.update = function (itemList) {
        var txt = document.getElementById('txt');
        txt.value = itemList.items.join('\n');
    };
    return SortListener;
}());
function sort(order) {
    var txt = document.getElementById('txt');
    var lines = txt.value.split('\n');
    var itemList = { items: lines };
    sorter.sort(order, itemList);
}
document.getElementById('btnAsc').addEventListener('click', function (e) {
    sort(example.SortOrder.ASCENDING);
});
document.getElementById('btnDesc').addEventListener('click', function (e) {
    sort(example.SortOrder.DESCENDING);
});
document.getElementById('btnRandom').addEventListener('click', function (e) {
    sort(example.SortOrder.RANDOM);
});
