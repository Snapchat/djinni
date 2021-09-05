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
