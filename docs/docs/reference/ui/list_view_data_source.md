path: tree/master/framework/ui/include/bdn/ui
source: ListViewDataSource.h

# ListViewDataSource

An abstract base class for a [`ListView`](list_view.md)'s data source.

To implement a custom list view data source, inherit from `ListViewDataSource` and implement `numberOfRow()`, `viewForRoIndex()`, and `heightForRowIndex()`. See the example below for reference.

## Declaration

```C++
namespace bdn::ui {
	class ListViewDataSource
}
```

## Example

```C++
#include <bdn/String.h>
#include <bdn/ui/ListView.h>
#include <bdn/ui/ListViewDataSource.h>

class MyDataSource : public bdn::Base, virtual public bdn::ListViewDataSource
{
public:
	std::array<bdn::String, 2> _data = {"This is a test", "With only two lines of text"};

	size_t numberOfRows() override { return 2; }

	auto viewForRowIndex( size_t rowIndex, 
						  std::shared_ptr<bdn::View> reusableView) override
	{
		label = std::dynamic_pointer_cast<bdn::TextView>(reusableView);
		if(!label) {
			label = std::make_shared<bdn::TextView>();
		}
		label->text = _data[rowIndex];;
		return std::dynamic_pointer_cast<bdn::View>(label);
	}
	
	float heightForRowIndex(size_t rowIndex) { return 25; }
};
// ...
auto listView = std::make_shared<bdn::ListView>();
listView->dataSource = std::make_shared<MyDataSource>();
listView->reloadData();
```

## Implementing a Data Source

* **virtual size_t numberOfRows() = 0**

	Return the number of available rows in your data source implementation.
	
* **virtual std::shared_ptr<[View](view.md)\> viewForRowIndex(size_t rowIndex, std::shared_ptr<[View](view.md)\> reusableView) = 0**

	Return the item view for the given row index in your data source implementation.

* **virtual float heightForRowIndex(size_t rowIndex) = 0**

	Return the height of the row at the given row index in your data source implementation.


 