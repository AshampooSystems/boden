# ListView

Displays Items from its model in a list. See [ListViewDataSource](list_view_data_source.md).

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

* **Property<std::shared_ptr<ListViewDataSource\>\> dataSource**

	The datasource of the list

* **Property<std::optional<size_t\>\> selectedRowIndex**

	The currently selected row index


## Data

* **void reloadData()**

	Forces the list to refresh from its datasource

## Swipe refresh

<video width="320" height="240" autoplay loop>
	<source src="https://img.ashampoo.com/ashampoo.com_images/img/1/external/Boden/refresh.webm" type="video/webm">
</video>


* **Property<bool\> enableRefresh**

	Enables refresh on swipe down.

* **void refreshDone()**

	Finishes/hides the refresh animation if it was triggered by the user

## Events

* **SimpleNotifier<\> &onRefresh()**

	Signals that the list has updated its data.


## Relationships

Inherits from: [View](view.md)
 