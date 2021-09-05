(function(){function r(e,n,t){function o(i,f){if(!n[i]){if(!e[i]){var c="function"==typeof require&&require;if(!f&&c)return c(i,!0);if(u)return u(i,!0);var a=new Error("Cannot find module '"+i+"'");throw a.code="MODULE_NOT_FOUND",a}var p=n[i]={exports:{}};e[i][0].call(p.exports,function(r){var n=e[i][1][r];return o(n||r)},p,p.exports,r,e,n,t)}return n[i].exports}for(var u="function"==typeof require&&require,i=0;i<t.length;i++)o(t[i]);return o}return r})()({1:[function(require,module,exports){
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

},{"./example":2}],2:[function(require,module,exports){
"use strict";
exports.__esModule = true;
exports.load = exports.SortOrder = void 0;
var SortOrder;
(function (SortOrder) {
    SortOrder[SortOrder["ASCENDING"] = 0] = "ASCENDING";
    SortOrder[SortOrder["DESCENDING"] = 1] = "DESCENDING";
    SortOrder[SortOrder["RANDOM"] = 2] = "RANDOM";
})(SortOrder = exports.SortOrder || (exports.SortOrder = {}));
function load() {
    return Module();
}
exports.load = load;

},{}]},{},[1]);
