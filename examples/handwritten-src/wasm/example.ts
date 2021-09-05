export enum SortOrder {
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

declare global {
    function Module(): Promise<ModuleStatics>;
}

export function load(): Promise<ModuleStatics> {
    return Module();
}
