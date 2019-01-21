
#include <bdn/LayoutCoordinator.h>

#include <bdn/NotImplementedError.h>
#include <bdn/log.h>

namespace bdn
{

    LayoutCoordinator::LayoutCoordinator() {}

    void LayoutCoordinator::windowNeedsAutoSizing(std::shared_ptr<Window> window)
    {
        AppRunnerBase::assertInMainThread();

        _windowAutoSizeSet.insert(window);

        needUpdate();
    }

    void LayoutCoordinator::windowNeedsCentering(std::shared_ptr<Window> window)
    {
        AppRunnerBase::assertInMainThread();

        _windowCenterSet.insert(window);

        needUpdate();
    }

    void LayoutCoordinator::viewNeedsLayout(std::shared_ptr<View> view)
    {
        AppRunnerBase::assertInMainThread();

        _layoutSet.insert(view);

        needUpdate();
    }

    void LayoutCoordinator::needUpdate()
    {
        AppRunnerBase::assertInMainThread();

        if (!_updateScheduled) {
            // TODO

            std::shared_ptr<LayoutCoordinator> self = std::static_pointer_cast<LayoutCoordinator>(shared_from_this());

            _updateScheduled = true;

            // note that we use asyncCallFromMainThread here. I.e. if we are
            // in the main thread then this is still scheduled for later, rather
            // than run immediately.
            // That is what we want, because that allows us to collect and
            // combine multiple operations.
            asyncCallFromMainThread([self]() {
                self->_updateScheduled = false;

                self->updateNow();
            });
        }
    }

    void LayoutCoordinator::updateNow()
    {
        if (_inUpdateNow) {
            // this means that one of the views has caused events to be handled
            // during its sizing, layout or other call. Ignore this here.
            return;
        }

        _inUpdateNow = true;

        try {
            // first we have to determine the order in which we should
            // update the views.
            // For resizing the optimal order is child-to-parent. Because if
            // a child's child changes then this might influence the parent's
            // size. If we would update the parent first and then the child,
            // then the child update might cause the parent to need another
            // update afterwards.

            struct ToDo
            {
                ToDo() : level(-1) {}

                ToDo(const std::shared_ptr<View> &view)
                {
                    this->view = view;

                    // find out the view's level inside the UI tree
                    this->level = 0;
                    std::shared_ptr<View> currParent = view->getParentView();
                    while (currParent != nullptr) {
                        this->level++;
                        currParent = currParent->getParentView();
                    }
                }

                bool operator<(const ToDo &o) const
                {
                    return (level < o.level || (level == o.level && view.get() < o.view.get()));
                }

                std::shared_ptr<View> view;
                int level;
            };

            // now we do window auto sizing
            bool anyWindowsAutoSized = false;
            {
                // note that the order in which we auto-size the windows
                // does not matter, since all windows are top-level

                std::set<std::shared_ptr<Window>> toDoSet;
                while (true) {
                    toDoSet.insert(_windowAutoSizeSet.begin(), _windowAutoSizeSet.end());
                    _windowAutoSizeSet.clear();

                    if (toDoSet.empty()) {
                        // done.
                        break;
                    }

                    std::shared_ptr<Window> window = *toDoSet.begin();
                    toDoSet.erase(toDoSet.begin());

                    anyWindowsAutoSized = true;

                    std::shared_ptr<IWindowCoreExtension> core =
                        std::dynamic_pointer_cast<IWindowCoreExtension>(window->getViewCore());
                    if (core != nullptr) {
                        try {
                            core->autoSize();
                        }
                        catch (std::exception &e) {
                            handleException(&e, "LayoutCoordinator::"
                                                "IWindowCoreExtension::autoSize");
                        }
                        catch (...) {
                            handleException(nullptr, "LayoutCoordinator::"
                                                     "IWindowCoreExtension::autoSize");
                        }
                    }
                }
            }

            // note that for window sizing we also need to do the same async
            // interruption here that we do after sizing info updates. While
            // window sizes cannot impact parents, they CAN cause layout
            // operations of inner windows. And we do want all of those to be in
            // our queues, so that we can remove duplicates.
            // So again, we reschedule another update if we have done anything.
            if (anyWindowsAutoSized)
                needUpdate();
            else {
                bool anyLayoutDone = false;

                // now do the layout operations.
                // Note that layout operations that have been triggered by any
                // of the sizing info updates, or window auto-sizing are
                // included in this!
                {
                    ToDo nextToDo;

                    {
                        std::set<std::shared_ptr<View>> layoutSetCopy;

                        {
                            layoutSetCopy = _layoutSet;
                        }

                        {
                            // note that we only remove one view at a time from
                            // the pending set. The reason is that we need to
                            // handle pending events after each layout. So we
                            // select the one we want (parent first order) and
                            // leave the others in the set for now.

                            for (auto &view : layoutSetCopy) {
                                // construct ToDo object so that we know the
                                // level.
                                ToDo toDo(view);

                                if (nextToDo.view == nullptr || toDo < nextToDo)
                                    nextToDo = toDo;
                            }
                        }
                    }

                    if (nextToDo.view != nullptr) {
                        // remove the view we selected from the set
                        _layoutSet.erase(nextToDo.view);
                    }

                    if (nextToDo.view != nullptr) {
                        anyLayoutDone = true;

                        try {
                            std::shared_ptr<IViewCoreExtension> core =
                                std::dynamic_pointer_cast<IViewCoreExtension>(nextToDo.view->getViewCore());
                            if (core != nullptr)
                                core->layout();
                        }
                        catch (std::exception &e) {
                            handleException(&e, "LayoutCoordinator::"
                                                "IViewCoreExtension::layout");
                        }
                        catch (...) {
                            handleException(nullptr, "LayoutCoordinator::"
                                                     "IViewCoreExtension::layout");
                        }
                    }
                }

                // if a parent view has been layouted then we want events to be
                // handled at this point, before we continue with our todo list
                // (which is in parent-first order). The reason is that child
                // sizes may have changed and the property change notifications
                // for those changes are queued up in the dispatcher queue now
                // (if child sizes changed). And those queued up change
                // notifications will likely trigger a layout request for the
                // child view they belong to. And since the child view might
                // already be in our todo list it is important that we wait for
                // the queued up events to be handled before we proceed with the
                // layout. Only if we wait can we detect the duplicate layout
                // and prevent the child from being layouted twice. So if we did
                // layout anything then we schedule another update. Otherwise we
                // continue.
                if (anyLayoutDone)
                    needUpdate();
                else {
                    // now we do window centering
                    {
                        // note that the order in which we auto-size the windows
                        // does not matter, since all windows are top-level

                        std::set<std::shared_ptr<Window>> toDoSet;
                        while (true) {
                            toDoSet.insert(_windowCenterSet.begin(), _windowCenterSet.end());
                            _windowCenterSet.clear();

                            if (toDoSet.empty()) {
                                // done.
                                break;
                            }

                            std::shared_ptr<Window> window = *toDoSet.begin();
                            toDoSet.erase(toDoSet.begin());

                            std::shared_ptr<IWindowCoreExtension> core =
                                std::dynamic_pointer_cast<IWindowCoreExtension>(window->getViewCore());
                            if (core != nullptr) {
                                try {
                                    core->center();
                                }
                                catch (std::exception &e) {
                                    handleException(&e, "LayoutCoordinator::"
                                                        "IWindowCoreExtension::center");
                                }
                                catch (...) {
                                    handleException(nullptr, "LayoutCoordinator::"
                                                             "IWindowCoreExtension::center");
                                }
                            }
                        }
                    }
                }
            }
        }
        catch (...) {
            _inUpdateNow = false;
            throw;
        }

        _inUpdateNow = false;
    }

    void LayoutCoordinator::handleException(const std::exception *exceptionIfAvailable, const String &functionName)
    {
        // log and ignore
        if (exceptionIfAvailable != nullptr)
            logError(*exceptionIfAvailable,
                     "Exception in " + functionName + " during LayoutCoordinator updating. Ignording.");
        else
            logError("Exception pf unknown type in " + functionName + " during LayoutCoordinator updating. Ignording.");
    }
}
