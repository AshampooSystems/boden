#include <bdn/init.h>
#include <bdn/Window.h>
#include <bdn/ColumnView.h>
#include <bdn/RowView.h>
#include <bdn/Button.h>
#include <bdn/TextView.h>
#include <bdn/ScrollView.h>
#include <bdn/TextField.h>
#include <bdn/Toggle.h>
#include <bdn/Switch.h>
#include <bdn/Checkbox.h>

#include <bdn/appInit.h>
#include <bdn/UiAppControllerBase.h>

#include <bdn/property.h>

using namespace bdn;

class MainViewController : public Base
{
  public:
    MainViewController()
    {
        _window = newObj<Window>();
        _window->setTitle("UI Demo");

        auto mainColumn = newObj<ColumnView>();
        mainColumn->setPreferredSizeMinimum(Size(250, 0));

        addControlWithHeading("Switch", newObj<Switch>(), mainColumn, true);
        addControlWithHeading("Checkbox", newObj<Checkbox>(), mainColumn, true);

        auto btn = newObj<Button>();
        btn->setLabel("Button");
        addControlWithHeading("Button", btn, mainColumn, true);

        P<TextField> textFieldCtrl = newObj<TextField>();

        addControlWithHeading("Text Input", textFieldCtrl, mainColumn, true);
        textFieldCtrl->setPreferredSizeMinimum(Size(250, 0));
        textFieldCtrl->setText("Test text");

        auto textScrollView = newObj<ScrollView>();

        // limit the maximum size. We simply want the scroll view to fill the
        // available width and have a height of 100 dips.
        textScrollView->setPreferredSizeMinimum(Size(0, 100));
        textScrollView->setPreferredSizeMaximum(Size(0, 100));

        auto scrolledTextView = newObj<TextView>();
        scrolledTextView->setText(
            "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam "
            "nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam "
            "erat, sed diam voluptua. At vero eos et accusam et justo duo "
            "dolores et ea rebum. Stet clita kasd gubergren, no sea takimata "
            "sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit "
            "amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor "
            "invidunt ut labore et dolore magna aliquyam erat, sed diam "
            "voluptua. At vero eos et accusam et justo duo dolores et ea "
            "rebum. Stet clita kasd gubergren, no sea takimata sanctus est "
            "Lorem ipsum dolor sit amet.");
        textScrollView->setContentView(scrolledTextView);

        addControlWithHeading("Scrolling multi line text", textScrollView,
                              mainColumn, false);

        auto listColumn = newObj<ColumnView>();

        String demoEntries[] = {"List", "Demo"};

        for (auto entry : demoEntries) {
            auto newEntry = newObj<TextView>();
            newEntry->setText(entry);
            listColumn->addChildView(newEntry);
        }

        listColumn->setHorizontalAlignment(View::HorizontalAlignment::expand);
        listColumn->setMargin(UiMargin(15, 15, 15, 15));

        auto addButton = newObj<Button>();
        addButton->setLabel("Add");
        addButton->setHorizontalAlignment(View::HorizontalAlignment::expand);

        addButton->onClick() += [listColumn, textFieldCtrl](auto) {
            auto newEntry = newObj<TextView>();
            newEntry->setText(textFieldCtrl->text());
            listColumn->addChildView(newEntry);
        };

        auto removeButton = newObj<Button>();
        removeButton->setLabel("Remove");
        removeButton->setHorizontalAlignment(View::HorizontalAlignment::expand);
        removeButton->onClick() += [listColumn](auto) {
            bdn::List<bdn::P<bdn::View>> children;
            listColumn->getChildViews(children);
            if (children.size() > 0) {
                listColumn->removeChildView(children.back());
            }
        };

        auto clearButton = newObj<Button>();
        clearButton->setLabel("Clear");
        clearButton->setHorizontalAlignment(View::HorizontalAlignment::expand);
        clearButton->onClick() +=
            [listColumn](auto) { listColumn->removeAllChildViews(); };

        auto buttonRow = newObj<RowView>();
        buttonRow->setPreferredSizeMinimum(Size(200, 0));
        buttonRow->addChildView(addButton);
        buttonRow->addChildView(removeButton);
        buttonRow->addChildView(clearButton);

        addControlWithHeading("Mutable list", buttonRow, mainColumn, true);
        mainColumn->addChildView(listColumn);

        auto mainScrollView = newObj<ScrollView>();
        mainScrollView->setContentView(mainColumn);

        _window->setContentView(mainScrollView);

        _window->requestAutoSize();
        _window->requestCenter();

        _window->setVisible(true);
    }

    void addControlWithHeading(String headingText, P<View> control,
                               P<ContainerView> container, bool single)
    {
        auto header = newObj<TextView>();
        header->setText(headingText);
        header->setHorizontalAlignment(View::HorizontalAlignment::left);

        if (single) {
            control->setHorizontalAlignment(View::HorizontalAlignment::right);
            control->setVerticalAlignment(View::VerticalAlignment::middle);
            header->setVerticalAlignment(View::VerticalAlignment::middle);
            auto row = newObj<RowView>();
            row->setMargin(UiMargin(15, 15, 15, 15));
            row->setHorizontalAlignment(View::HorizontalAlignment::expand);
            header->setHorizontalAlignment(View::HorizontalAlignment::expand);
            row->addChildView(header);
            row->addChildView(control);
            container->addChildView(row);
        } else {
            header->setMargin(UiMargin(15 + 6, 15, 18, 15));
            control->setMargin(UiMargin(2, 15, 15, 15));
            control->setHorizontalAlignment(View::HorizontalAlignment::expand);

            container->addChildView(header);
            container->addChildView(control);
        }
    }

  protected:
    P<Window> _window;
};

class AppController : public UiAppControllerBase
{
  public:
    void beginLaunch(const AppLaunchInfo &launchInfo) override
    {
        _mainViewController = newObj<MainViewController>();
    }

  protected:
    P<MainViewController> _mainViewController;
};

BDN_APP_INIT(AppController)
