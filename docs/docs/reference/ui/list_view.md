path: tree/master/framework/ui/include/bdn/
source: ListView.h

# ListView

A view displaying items in a scrollable single-column list.

See also: [ListViewDataSource](list_view_data_source.md).

## Declaration

```C++
class ListView : public View
```

## Example

```C++
#include <bdn/ListView.h>
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

## Enabling Swipe Refresh

* **[Property](../foundation/property.md)<bool\> enableRefresh**

	Enables refresh on swipe down.

* **void refreshDone()**

	Finishes/hides the refresh animation if it was triggered by the user.

## Events

* **[Notifier](../foundation/notifier.md)<\> &onRefresh()**

	Signals that the list has updated its data.


## Relationships

Inherits from: [View](view.md)
 