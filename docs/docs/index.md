template: hero.html

***************************************************************************************************************
WARNING: THIS IS NOT THE SOURCE FILE FOR THE BODEN.IO FRONT PAGE. THE FRONT PAGE SOURCE IS IN themes/hero.html!
PLEASE DO NOT DELETE THIS FILE. WE KEEP IT FOR REFERENCE AND TO GENERATE SYNTAX HIGHLIGHTED SOURCE BLOCKS.
***************************************************************************************************************

# Build real native mobile apps using modern C++17 and Boden

Write your mobile apps in modern C++17 and compile 100% native Android and iOS apps from a single statically typed codebase. No need for scripting. No overhead introduced by virtual machines. Using Boden is straight forward:

```C++
// MainViewController.cpp
#include <bdn/ui.h>
#include <bdn/ui/yoga.h>

#include "MainViewController.h"

using namespace bdn;
using namespace bdn::ui;

MainViewController::MainViewController()
{
    _window = std::make_shared<Window>();
    _window->title = "AwesomeApp";
    _window->geometry = Rect{0, 0, 400, 300};
    _window->setLayout(std::make_shared<yoga::Layout>());

    auto button = std::make_shared<Button>();
    button->label = "Hello World";

    _window->contentView = button;

    _window->visible = true;
}
```

<p><br></p>

# Harness the power of native widgets and native user interface behavior

Boden wraps the native user interface controls provided by the platform. This is done consistently, down to the level of a list item in a native list control. In doing so, Boden lets you create apps as if you had used the native platform framework. And users won't recognize any difference to the behavior they are already used to.

# Lay out your user interface with Flex layouts

With Boden laying out your UI is a breeze. Just define your layouts using Flex stylesheets directly in C++. Boden uses the Yoga layout engine accross all platforms.

```C++
// MainViewController.cpp
#include <bdn/ui.h>
#include <bdn/ui/yoga.h>

#include "MainViewController.h"

using namespace bdn;
using namespace bdn::ui;

MainViewController::MainViewController()
{
    _window = std::make_shared<Window>();
    _window->title = "AwesomeApp";
    _window->geometry = Rect{0, 0, 400, 300};
    _window->setLayout(std::make_shared<yoga::Layout>());
    _window->stylesheet =
                FlexJsonStringify({
                                      "justifyContent": "Center", 
                                      "alignItems": "Center"
                                  });

    std::shared_ptr<Button> button = std::make_shared<Button>();
    button->label = "Hello World";

    _window->contentView = button;
    _window->visible = true;
}
```

<p><br></p>

# Reuse your existing programming skills

Boden builds upon well-established technologies and best practices where possible. A Boden string literally is an `std::string`. No custom containers, smart pointers, or other basic concepts. This allows you to reuse your existing knowledge and focus on what's most important: your app.

# Enjoy the modern lightweight approach to building cross-platform apps

Boden doesn't come with 20 gigabytes of bloat and it doesn't require you to go through a lengthy installation process. A simple `git clone` command and an integrated development environment most likely already installed on your machine is enough to get started.
