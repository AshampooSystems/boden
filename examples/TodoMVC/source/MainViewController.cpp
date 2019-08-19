#include <bdn/log.h>
#include <bdn/ui.h>
#include <bdn/ui/yoga.h>

#include "MainViewController.h"

using namespace bdn::ui;

MainViewController::MainViewController()
{
    _window = std::make_shared<Window>();
    _window->title = "TodoMVC";
    _window->geometry = bdn::Rect{0, 0, 300, 500};
    _window->setLayout(std::make_shared<yoga::Layout>());

    _mainContainer = std::make_shared<ContainerView>();
    _mainContainer->stylesheet =
        FlexJsonStringify({"flexGrow" : 1.0, "direction" : "Column", "justifyContent" : "Center"});

    _newEntryField = std::make_shared<TextField>();
    _newEntryField->placeholder = "What needs to be done?";
    _newEntryField->stylesheet = FlexJsonStringify({"alignSelf" : "Center"});

    _mainContainer->addChildView(_newEntryField);

    _listView = std::make_shared<ListView>();
    _listView->enableSwipeToDelete = true;
    _todoDataSource = std::make_shared<TodoListDataSource>();
    _todoDataSource->entryCompletedChanged() += [=](auto, auto) { /*listView->reloadData();*/ };
    _listView->dataSource = _todoDataSource;
    _listView->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

    _listView->onDelete() += [this](int index) { _todoDataSource->remove(index); };

    _newEntryField->onSubmit() += [=](auto ev) {
        if (!_newEntryField->text->empty()) {
            _todoDataSource->add(_newEntryField->text);
            _newEntryField->text = std::string();
            _listView->reloadData();
        }
    };

    _todoDataSource->empty.onChange() += [=](auto &property) { handleDataEmptyChange(); };

#ifdef BDN_PLATFORM_OSX
    _todoDataSource->onChange() += [this]() { _listView->reloadData(); };
#endif

    handleDataEmptyChange();

    _window->contentView = _mainContainer;
    _window->visible = true;
}

void MainViewController::handleDataEmptyChange()
{
    int textWidth = 0;

    if (_todoDataSource->empty) {
        _mainContainer->removeChildView(_listView);

        textWidth = 66;
    } else {
        _mainContainer->addChildView(_listView);
        textWidth = 100;
    }

    auto stylesheet = _newEntryField->stylesheet.get();
    stylesheet["flex"]["size"]["width"] = std::to_string(textWidth) + "%";
    _newEntryField->stylesheet = stylesheet;
}
