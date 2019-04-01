path: tree/master/framework/ui/include/bdn/
source: ListViewDataSource.h

# ListViewDataSource

An abstract base class for a [ListView's](list_view.md) data source

## Declaration

```C++
class ListViewDataSource
```

## Example

```C++
#include <bdn/String.h>
#include <bdn/ListView.h>
#include <bdn/ListViewDataSource.h>

class MyDataSource : public bdn::Base, virtual public bdn::ListViewDataSource
{
public:
	std::array<bdn::String, 2> _data = {"This is a test", "With only two lines of text"};

	size_t numberOfRows() override { return 2; }

	String labelTextForRowIndex(size_t rowIndex) override { return _data[rowIndex]; }
	
	auto viewForRowIndex( size_t rowIndex, 
						  std::shared_ptr<bdn::View> reusableView) override
	{
		label = std::dynamic_pointer_cast<bdn::TextView>(reusableView);
		if(!label) {
			label = std::make_shared<bdn::TextView>();
		}
		label->text = labelTextForRowIndex(rowIndex);
		return std::dynamic_pointer_cast<bdn::View>(label);
	}
	
	float heightForRowIndex(size_t rowIndex) { return 25; }
};
// ...
auto listView = std::make_shared<bdn::ListView>();
listView->dataSource = std::make_shared<MyDataSource>();
listView->reloadData();
```

## Functions

* **virtual size_t numberOfRows() = 0**

	Return the number of rows

* **virtual String labelTextForRowIndex(size_t rowIndex) = 0**

	<span style="color: red">DOCFIXME: Currently unused! Remove or reimplement.</span>

	Return the text for a row
	
* **virtual std::shared_ptr<View> viewForRowIndex(size_t rowIndex, std::shared_ptr<View> reusableView) = 0**

	Return the view for a row

* **virtual float heightForRowIndex(size_t rowIndex) = 0**

	Return the height of a row


 