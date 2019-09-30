#include <bdn/Application.h>
#include <bdn/Json.h>
#include <bdn/platform.h>
#include <bdn/ui/ContainerView.h>
#include <bdn/ui/ImageView.h>
#include <bdn/ui/Label.h>
#include <bdn/ui/TextField.h>
#include <bdn/ui/yoga/FlexStylesheet.h>

#include "ListViewPage.h"

using namespace std::chrono_literals;

namespace bdn
{
    class DemoDataSource : public ui::ListViewDataSource
    {
      public:
        std::vector<std::string> data = {"Entry 1", "Entry 2", "Entry 3 (not selectable)", "Entry 4", "Entry 5"};
        float rowHeight = 25.0f;

      public:
        size_t numberOfRows(const std::shared_ptr<ui::ListView> &) override { return data.size(); }
        std::shared_ptr<View> viewForRowIndex(const std::shared_ptr<ListView> &, size_t rowIndex,
                                              std::shared_ptr<View> reusableView) override
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

        float heightForRowIndex(const std::shared_ptr<ui::ListView> &, size_t rowIndex) override { return rowHeight; }

        void remove(int pos)
        {
            assert(pos >= 0 && pos < data.size());
            data.erase(data.begin() + pos);
        }

        bool shouldSelectRow(const std::shared_ptr<ListView> &listView, size_t rowIndex) override
        {
            if (rowIndex == 2)
                return false;
            return true;
        }
    };

    void ListViewPage::init()
    {
        stylesheet = FlexJsonStringify({"flexGrow" : 1.0, "margin" : {"all" : 10}});

        auto refreshBox = std::make_shared<Switch>();
        addChildView(makeRow("Enable Refresh", refreshBox));

#if defined(BDN_PLATFORM_IOS) || defined(BDN_PLATFORM_ANDROID)
        auto deleteBox = std::make_shared<Switch>();
        addChildView(makeRow("Enable Deletion", deleteBox));
#endif

        auto heightSlider = std::make_shared<Slider>();
        heightSlider->value = 0.5f;
        heightSlider->stylesheet = FlexJsonStringify({"minimumSize" : {"width" : 100}});
        heightSlider->value.onChange() += [this](const auto &p) {
            std::dynamic_pointer_cast<DemoDataSource>(_listView->dataSource.get())->rowHeight = (float)p * 50.0f;
            _listView->reloadData();
        };
        addChildView(makeRow("Row height", heightSlider));

        auto indexView = std::make_shared<TextField>();
        indexView->text.bind(stringIndex);
        indexView->stylesheet = FlexJsonStringify({"flexGrow" : 1, "maximumSize" : {"width" : 250}});
        addChildView(makeRow("Current Index", indexView));

        _listView->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});
        _listView->dataSource = std::make_shared<DemoDataSource>();
        _listView->enableRefresh.bind(refreshBox->on);

#if defined(BDN_PLATFORM_IOS) || defined(BDN_PLATFORM_ANDROID)
        _listView->enableSwipeToDelete.bind(deleteBox->on);
#endif

        _listView->onRefresh() +=
            [this]() { App()->dispatchQueue()->dispatchAsyncDelayed(1s, [this]() { _listView->refreshDone(); }); };

        _listView->onDelete() +=
            [this](int pos) { std::dynamic_pointer_cast<DemoDataSource>(_listView->dataSource.get())->remove(pos); };

        addChildView(_listView);

        _listView->reloadData();
    }
}
