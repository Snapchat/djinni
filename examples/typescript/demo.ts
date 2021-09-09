import * as example from "../../generated-src/ts/example";

declare function Module(): Promise<example.Module_statics>;

var sorter: example.SortItems;

Module().then(m => {
    sorter = m.SortItems.createWithListener(new SortListener());
})

class SortListener implements example.TextboxListener {
    update(itemList: example.ItemList) {
        var txt = <HTMLTextAreaElement>document.getElementById('txt');
        txt.value = itemList.items.join('\n')
    }
}

function sort(order: example.SortOrder): void {
    var txt = <HTMLTextAreaElement>document.getElementById('txt');
    var lines = txt.value.split('\n');
    var itemList: example.ItemList = {items: lines};
    sorter.sort(order, itemList);
}

document.getElementById('btnAsc').addEventListener('click', e => {
    sort(example.SortOrder.ASCENDING)
})
document.getElementById('btnDesc').addEventListener('click', e => {
    sort(example.SortOrder.DESCENDING)
})
document.getElementById('btnRandom').addEventListener('click', e => {
    sort(example.SortOrder.RANDOM)
})
