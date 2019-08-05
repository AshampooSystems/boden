#include <bdn/Json.h>
#include <bdn/ui/ContainerView.h>
#include <bdn/ui/ImageView.h>
#include <bdn/ui/Label.h>
#include <bdn/ui/TextField.h>
#include <bdn/ui/yoga/FlexStylesheet.h>

#include "ListViewPage.h"

namespace bdn
{
    class DemoDataSource : public ui::ListViewDataSource
    {
      public:
        std::array<std::string, 5> data = {"Entry 1", "Entry 2", "Entry 3", "Entry 4", "Entry 5"};

      public:
        size_t numberOfRows() override { return data.size(); }
        std::shared_ptr<View> viewForRowIndex(size_t rowIndex, std::shared_ptr<View> reusableView) override
        {
            if (!reusableView) {
                reusableView = std::make_shared<Label>();
            }

            auto label = std::dynamic_pointer_cast<Label>(reusableView);
            if (label) {
                label->text = data[rowIndex];
            }
            return reusableView;
        }
        float heightForRowIndex(size_t rowIndex) override { return 25; }
    };

    void ListViewPage::init()
    {
        stylesheet = FlexJsonStringify({"flexGrow" : 1.0, "margin" : {"all" : 10}});

        auto indexView = std::make_shared<TextField>();
        indexView->text.bind(stringIndex);
        indexView->stylesheet = FlexJsonStringify({"flexGrow" : 1, "maximumSize" : {"width" : 250}});
        addChildView(makeRow("Current Index", indexView));

        _listView->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});
        _listView->dataSource = std::make_shared<DemoDataSource>();
        addChildView(_listView);

        _listView->reloadData();
    }
}
