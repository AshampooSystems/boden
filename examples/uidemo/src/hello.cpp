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

        addControlWithHeading("Text Field", textFieldCtrl, mainColumn, true);
        textFieldCtrl->setPreferredSizeMinimum(Size(250, 0));

        auto textScrollView = newObj<ScrollView>();

        // limit the maximum size. We simply want the scroll view to fill the
        // available width and have a height of 100 dips.
        textScrollView->setPreferredSizeMinimum(Size(0, 186));
        textScrollView->setPreferredSizeMaximum(Size(0, 186));

        auto scrolledTextView = newObj<TextView>();
        scrolledTextView->setText("The user interface (UI), in the industrial design field of "
                                  "human–computer interaction, is the space where interactions "
                                  "between humans and machines occur. The goal of this interaction "
                                  "is to allow effective operation and control of the machine from "
                                  "the human end, whilst the machine simultaneously feeds back "
                                  "information that aids the operators' decision-making process. "
                                  "Examples of this broad concept of user interfaces include the "
                                  "interactive aspects of computer operating systems, hand tools, "
                                  "heavy machinery operator controls, and process controls. The "
                                  "design considerations applicable when creating user interfaces "
                                  "are related to or involve such disciplines as ergonomics and "
                                  "psychology.\n\n"
                                  "Generally, the goal of user interface design is to produce a user "
                                  "interface which makes it easy, efficient, and enjoyable "
                                  "(user-friendly) to operate a machine in the way which produces "
                                  "the desired result. This generally means that the operator needs "
                                  "to provide minimal input to achieve the desired output, and also "
                                  "that the machine minimizes undesired outputs to the human.\n\n"
                                  "User interfaces are composed of one or more layers including a "
                                  "human-machine interface (HMI) interfaces machines with physical "
                                  "input hardware such a keyboards, mice, game pads and output "
                                  "hardware such as computer monitors, speakers, and printers. A "
                                  "device that implements a HMI is called a human interface device "
                                  "(HID). Other terms for human-machine interfaces are man–machine "
                                  "interface (MMI) and when the machine in question is a computer "
                                  "human–computer interface. Additional UI layers may interact with "
                                  "one or more human sense, including: tactile UI (touch), visual UI "
                                  "(sight), auditory UI (sound), olfactory UI (smell), equilibrial "
                                  "UI (balance), and gustatory UI (taste).\n\n"
                                  "Composite user interfaces (CUI) are UIs that interact with two or "
                                  "more senses. The most common CUI is a graphical user interface "
                                  "(GUI), which is composed of a tactile UI and a visual UI capable "
                                  "of displaying graphics. When sound is added to a GUI it becomes a "
                                  "multimedia user interface (MUI). There are three broad categories "
                                  "of CUI: standard, virtual and augmented. Standard composite user "
                                  "interfaces use standard human interface devices like keyboards, "
                                  "mice, and computer monitors. When the CUI blocks out the real "
                                  "world to create a virtual reality, the CUI is virtual and uses a "
                                  "virtual reality interface. When the CUI does not block out the "
                                  "real world and creates augmented reality, the CUI is augmented "
                                  "and uses an augmented reality interface. When a UI interacts with "
                                  "all human senses, it is called a qualia interface, named after "
                                  "the theory of qualia. CUI may also be classified by how many "
                                  "senses they interact with as either an X-sense virtual reality "
                                  "interface or X-sense augmented reality interface, where X is the "
                                  "number of senses interfaced with. For example, a Smell-O-Vision "
                                  "is a 3-sense (3S) Standard CUI with visual display, sound and "
                                  "smells; when virtual reality interfaces interface with smells and "
                                  "touch it is said to be a 4-sense (4S) virtual reality interface; "
                                  "and when augmented reality interfaces interface with smells and "
                                  "touch it is said to be a 4-sense (4S) augmented reality "
                                  "interface.\n\nThis text uses material from the Wikipedia article "
                                  "https://en.wikipedia.org/wiki/User_interface");
        textScrollView->setContentView(scrolledTextView);

        addControlWithHeading("Scrolling multiline text", textScrollView, mainColumn, false);

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
            newEntry->setText(textFieldCtrl->text().empty() ? "New Item" : textFieldCtrl->text());
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
        clearButton->onClick() += [listColumn](auto) { listColumn->removeAllChildViews(); };

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

    void addControlWithHeading(String headingText, P<View> control, P<ContainerView> container, bool single)
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
    void beginLaunch(const AppLaunchInfo &launchInfo) override { _mainViewController = newObj<MainViewController>(); }

  protected:
    P<MainViewController> _mainViewController;
};

BDN_APP_INIT(AppController)
