#include <bdn/init.h>
//#include <bdn/Frame.h>
//#include <bdn/Button.h>
//#include <bdn/Switch.h>
#include <bdn/App.h>

#include <string>

using namespace bdn;


class MyApp : public App
{
public:
/*
    void onClick(ClickEvent& evt)
    {
        _counter++;

        dynamic_cast<Button*>(evt.getWindow())->setLabel("Hello World: "+std::to_string(_counter));
    }*/

    virtual void initUI() override
    {
        /*Frame* pFrame = new Frame;

        Button* pButton = new Button(pFrame, "Hello World");
        pButton->getClickEventSource()->subscribeMember<MyApp>(this, &MyApp::onClick );

        Switch* pSwitch = new Switch(pFrame, "This is a switch/checkbox");

        pFrame->show();*/
    }

protected:
    int _counter = 0;
};


MyApp _app;



