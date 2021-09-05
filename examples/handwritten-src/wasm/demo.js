var m;
var sorter;

Module().then(module => {
    m = module;
    sorter = m.SortItems.createWithListener(new SortListener())
});

class SortListener {
    update(itemList) {
        document.getElementById('txt').value = itemList.items.join('\n')
    }
}
function sort(order) {
    var lines = document.getElementById('txt').value.split('\n');
    var itemList = {
        items: lines
    };
    sorter.sort(order, itemList);
}
function onSortAsc() {
    sort(m.SortOrder.ASCENDING)
}
function onSortDesc() {
    sort(m.SortOrder.DESCENDING)
}
function onSortRandom() {
    sort(m.SortOrder.RANDOM)
}
