export = Module;
declare function Module(): Promise<Module.ModuleStatics>;

declare namespace Module {

export const enum SortOrder {
    ASCENDING,
    DESCENDING,
    RANDOM,
}

export interface ItemList {
    items: string[];
}

export interface TextboxListener {
    update(items: ItemList): void;
}

export interface SortItems {
    sort(order: SortOrder, items: ItemList): void;
}

export interface SortItemsStatics {
    createWithListener(listener: TextboxListener): SortItems;
    runSort(items: ItemList): ItemList;
}

export interface ModuleStatics {
    SortItems: SortItemsStatics;
}

}
