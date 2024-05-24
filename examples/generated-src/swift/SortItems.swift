public protocol SortItems: AnyObject {
    func sort(order: SortOrder, items: ItemList)
}
