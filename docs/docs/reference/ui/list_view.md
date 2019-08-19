path: tree/master/framework/ui/include/bdn/ui
source: ListView.h

# ListView

A view displaying items in a scrollable single-column list.

See also: [ListViewDataSource](list_view_data_source.md).

## Declaration

```C++
namespace bdn::ui {
	class ListView : public View
}
```

## Example

```C++
#include <bdn/ui/ListView.h>
...
auto listView = std::make_shared<ListView>();
listView->dataSource = std::make_shared<MyDataSource>();
listView->reloadData();
```

## Properties

* **[Property](../foundation/property.md)<std::shared_ptr<[ListViewDataSource](list_view_data_source.md)\>\> dataSource**

	The data source of the list.

* **[Property](../foundation/property.md)<std::optional<size_t\>\> selectedRowIndex**

	The currently selected row index.

## Reloading the List's Data

* **void reloadData()**

	Reloads the list's data from its data source.

## Swipe to refresh

* **[Property](../foundation/property.md)<bool\> enableRefresh**

	Enables refresh on swipe down.

* **[Notifier](../foundation/notifier.md)<\> &onRefresh()**

	Signals that the user has requested the List to reload its data.

* **void refreshDone()**

	Finishes/hides the refresh animation if it was triggered by the user.

## Swipe to delete

* **[Property](../foundation/property.md)<bool\> enableSwipeToDelete**

	Enables deletion when swiping an item left.

* **[Notifier](../foundation/notifier.md)<size_t\> &onDelete()**

	Signals that the user has requested the deletion of an element.
	The `size_t` argument specifies the location of the element to be deleted.
	The ListView automatically updates itself after the notification is processed, 
	failure to update the datasource results in undefined behaviour. 

## Relationships

Inherits from: [View](view.md)
 