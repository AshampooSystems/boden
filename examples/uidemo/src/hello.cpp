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
#include <bdn/AppControllerBase.h>

#include <bdn/property.h>

using namespace bdn;

class Model : public Base
{
  public:
    Model()
    {
        setUserText("Edit me");

        _morphingTextCounter = -1;
        changeMorphingText();
    }

    BDN_PROPERTY(int, helloCounter, setHelloCounter);

    BDN_PROPERTY(String, userText, setUserText);

    /** A text that changes when changeMorphingText is called.
        The different text variations are significantly different in their
       length.*/
    BDN_PROPERTY(String, morphingText, setMorphingText);

    void changeMorphingText()
    {
        _morphingTextCounter++;

        int sel = _morphingTextCounter % 3;
        if (sel == 0)
            setMorphingText("Short text");
        else if (sel == 1)
            setMorphingText(
                "This is a single-line, medium sized text. Lorem ipsum.");
        else
            setMorphingText(
                "This is a long text that spans multiple lines and also has "
                "some free lines.\nLorem ipsum dolor sit amet, consectetuer "
                "adipiscing elit.\nAenean commodo ligula eget dolor.\n\nAenean "
                "massa. Cum sociis natoque penatibus et magnis dis parturient "
                "montes, nascetur ridiculus mus.");
    }

  protected:
    int _morphingTextCounter;
};

class ViewModel : public Base
{
  public:
    ViewModel(Model *pModel)
    {
        _pModel = pModel;

        // Connect our read-only morphingText property to the
        // model's read-write morphingText property
        BDN_BIND_TO_PROPERTY(*this, setMorphingText, *_pModel, morphingText);

        // do a two-way binding between the userText properties of the model and
        // view model
        BDN_BIND_PROPERTIES(*this, userText, setUserText, *_pModel, userText,
                            setUserText);

        // connect our helloMessage to the helloCounter.
        // Note that we use a filter here to transform the integer counter
        // to a string for our property.
        auto helloMessageFilter = [](int newCounter) {
            String message = "Hello World";
            if (newCounter > 0)
                message += " " + std::to_string(newCounter);

            return message;
        };
        BDN_BIND_TO_PROPERTY_WITH_FILTER(*this, setHelloMessage, *_pModel,
                                         helloCounter, helloMessageFilter);

        setToggleText("Toggle me");
        setSwitchText("Switch me");
        setCheckboxText("Check me");

        toggleOnChanged().subscribe([this](bool isOn) {
            setToggleText(isOn ? "Toggle is on" : "Toggle is off");
        });

        switchOnChanged().subscribe([this](bool isOn) {
            setSwitchText(isOn ? "Switch is on" : "Switch is off");

            // Set checkbox state to mixed to test whether programatically
            // setting it after user interaction works
            setCheckboxState(TriState::mixed);
        });

        checkboxStateChanged().subscribe([this](TriState state) {
            switch (state) {
            case TriState::on:
                setCheckboxText("Checkbox is on");
                break;
            case TriState::off:
                setCheckboxText("Checkbox is off");
                break;
            case TriState::mixed:
                setCheckboxText("Checkbox is mixed");
                break;
            }
        });
    }

    BDN_PROPERTY_WITH_CUSTOM_ACCESS(String, public, helloMessage, protected,
                                    setHelloMessage);
    BDN_PROPERTY_WITH_CUSTOM_ACCESS(String, public, morphingText, protected,
                                    setMorphingText);

    BDN_PROPERTY(String, userText, setUserText);

    BDN_PROPERTY(String, toggleText, setToggleText);
    BDN_PROPERTY(String, switchText, setSwitchText);
    BDN_PROPERTY(String, checkboxText, setCheckboxText);

    BDN_PROPERTY(bool, toggleOn, setToggleOn);
    BDN_PROPERTY(bool, switchOn, setSwitchOn);
    BDN_PROPERTY(TriState, checkboxState, setCheckboxState);

    void increaseHelloCounter()
    {
        _pModel->setHelloCounter(_pModel->helloCounter() + 1);
    }

    void changeMorphingText() { _pModel->changeMorphingText(); }

  protected:
    P<Model> _pModel;
};

class MainViewController : public Base // ViewControllerBase
{
  public:
    MainViewController(ViewModel *pViewModel)
    {
        _pViewModel = pViewModel;

        _pWindow = newObj<Window>();
        _pWindow->setTitle("hello");

        P<ColumnView> pColumnView = newObj<ColumnView>();

        P<RowView> pRowView = newObj<RowView>();
        pColumnView->addChildView(pRowView);

        P<ColumnView> pInnerColumnView = newObj<ColumnView>();
        pRowView->addChildView(pInnerColumnView);

        P<Button> pButton = newObj<Button>();
        pButton->setLabel("Left column 1");
        pInnerColumnView->addChildView(pButton);

        P<Button> pButton2 = newObj<Button>();
        pButton2->setLabel("Left column 2");
        pInnerColumnView->addChildView(pButton2);

        P<Button> pButton3 = newObj<Button>();
        pButton3->setLabel("Right column");
        pButton3->setVerticalAlignment(View::VerticalAlignment::middle);
        pRowView->addChildView(pButton3);

        _pToggle = newObj<Toggle>();
        _pToggle->setMargin(UiMargin(10, 10, 10, 10));
        _pToggle->setHorizontalAlignment(View::HorizontalAlignment::center);
        BDN_BIND_PROPERTIES(*_pToggle, label, setLabel, *_pViewModel,
                            toggleText, setToggleText);
        BDN_BIND_PROPERTIES(*_pToggle, on, setOn, *_pViewModel, toggleOn,
                            setToggleOn);
        pColumnView->addChildView(_pToggle);

        _pSwitch = newObj<Switch>();
        _pSwitch->setMargin(UiMargin(10, 10, 10, 10));
        _pSwitch->setHorizontalAlignment(View::HorizontalAlignment::center);
        BDN_BIND_PROPERTIES(*_pSwitch, label, setLabel, *_pViewModel,
                            switchText, setSwitchText);
        BDN_BIND_PROPERTIES(*_pSwitch, on, setOn, *_pViewModel, switchOn,
                            setSwitchOn);
        pColumnView->addChildView(_pSwitch);

        _pCheckbox = newObj<Checkbox>();
        _pCheckbox->setMargin(UiMargin(10, 10, 10, 10));
        _pCheckbox->setHorizontalAlignment(View::HorizontalAlignment::center);
        BDN_BIND_PROPERTIES(*_pCheckbox, label, setLabel, *_pViewModel,
                            checkboxText, setCheckboxText);
        BDN_BIND_PROPERTIES(*_pCheckbox, state, setState, *_pViewModel,
                            checkboxState, setCheckboxState);
        pColumnView->addChildView(_pCheckbox);

        _pHelloMessageButton = newObj<Button>();

        // we want the hello message on the button
        BDN_BIND_TO_PROPERTY(*_pHelloMessageButton, setLabel, *_pViewModel,
                             helloMessage);

        _pHelloMessageButton->setMargin(UiMargin(10, 10, 10, 10));
        _pHelloMessageButton->setHorizontalAlignment(
            View::HorizontalAlignment::center);

        pColumnView->addChildView(_pHelloMessageButton);
        _pHelloMessageButton->onClick().subscribeParamless(
            weakMethod(this, &MainViewController::buttonClicked));

        _pUserTextField = newObj<TextField>();

        BDN_BIND_PROPERTIES(*_pUserTextField, text, setText, *pViewModel,
                            userText, setUserText);

        _pUserTextField->setMargin(UiMargin(UiLength::sem(1)));
        _pUserTextField->setHorizontalAlignment(
            View::HorizontalAlignment::expand);
        _pUserTextField->onSubmit().subscribeParamless(
            weakMethod(this, &MainViewController::textFieldSubmitted));
        pColumnView->addChildView(_pUserTextField);

        _pUserTextView = newObj<TextView>();

        BDN_BIND_TO_PROPERTY(*_pUserTextView, setText, *_pViewModel, userText);

        _pUserTextView->setMargin(UiMargin(UiLength::sem(1)));
        _pUserTextView->setHorizontalAlignment(
            View::HorizontalAlignment::center);
        pColumnView->addChildView(_pUserTextView);

        _pMorphingTextView = newObj<TextView>();

        // show the morphing text in the text view
        BDN_BIND_TO_PROPERTY(*_pMorphingTextView, setText, *_pViewModel,
                             morphingText);
        _pMorphingTextView->setMargin(UiMargin(10, 10, 10, 10));

        pColumnView->addChildView(_pMorphingTextView);

        _pScrollView = newObj<ScrollView>();

        // limit the maximum size. We simply want the scroll view to fill the
        // available width and have a height of 100 dips.
        _pScrollView->setPreferredSizeMinimum(Size(0, 100));
        _pScrollView->setPreferredSizeMaximum(Size(0, 100));
        _pScrollView->setHorizontalAlignment(View::HorizontalAlignment::expand);

        _pScrollView->setPadding(UiMargin(5, 5, 5, 5));
        _pScrollView->setMargin(UiMargin(10, 10, 10, 10));

        _pScrolledTextView = newObj<TextView>();

        _pScrolledTextView->setText(
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
        _pScrollView->setContentView(_pScrolledTextView);

        pColumnView->addChildView(_pScrollView);

        _pWindow->setContentView(pColumnView);

        _pWindow->requestAutoSize();
        _pWindow->requestCenter();

        _pWindow->setVisible(true);
    }

  protected:
    void buttonClicked()
    {
        _pViewModel->increaseHelloCounter();
        _pViewModel->changeMorphingText();

        // wait until the model changes have propagated to the UI, then autosize
        P<Window> pWindow = _pWindow;
        asyncCallFromMainThreadWhenIdle(
            [pWindow]() { pWindow->requestAutoSize(); });
    }

    void textFieldSubmitted()
    {
        _pViewModel->increaseHelloCounter();
        _pViewModel->changeMorphingText();
    }

    P<ViewModel> _pViewModel;

    P<Window> _pWindow;
    P<Button> _pHelloMessageButton;

    P<TextField> _pUserTextField;
    P<TextView> _pUserTextView;

    P<TextView> _pMorphingTextView;
    P<ScrollView> _pScrollView;
    P<TextView> _pScrolledTextView;

    P<Toggle> _pToggle;
    P<Switch> _pSwitch;
    P<Checkbox> _pCheckbox;
};

class AppController : public AppControllerBase
{
  public:
    void beginLaunch(const AppLaunchInfo &launchInfo) override
    {
        _pModel = newObj<Model>();
        _pViewModel = newObj<ViewModel>(_pModel);

        _pMainViewController = newObj<MainViewController>(_pViewModel);
    }

  protected:
    P<MainViewController> _pMainViewController;
    P<ViewModel> _pViewModel;
    P<Model> _pModel;
};

BDN_APP_INIT(AppController)
