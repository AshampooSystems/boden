#pragma once

#include <bdn/IWindowCore.h>
#include <bdn/Window.h>

#include <bdn/android/IParentViewCore.h>
#include <bdn/android/JActivity.h>
#include <bdn/android/JConfiguration.h>
#include <bdn/android/JNativeRootView.h>
#include <bdn/android/ViewCore.h>
#include <bdn/java/WeakReference.h>

#include "JWindow.h"
#include <bdn/log.h>

namespace bdn
{
    namespace android
    {

        class WindowCore : public ViewCore, virtual public IWindowCore, virtual public IParentViewCore
        {
          private:
            JView createJNativeViewGroup(std::shared_ptr<Window> outerWindow);

          public:
            WindowCore(std::shared_ptr<Window> outerWindow);
            virtual ~WindowCore();

            void enableBackButton(bool enable);

            static void _rootViewCreated(const java::Reference &javaRef);

            static void _rootViewDisposed(const java::Reference &javaRef);

            static void _rootViewSizeChanged(const java::Reference &javaRef, int width, int height);

            static void _rootViewConfigurationChanged(const java::Reference &javaRef, JConfiguration config);

            static bool _handleBackPressed(const java::Reference &javaRef);

            double getUIScaleFactor() const override { return ViewCore::getUIScaleFactor(); }

            void addChildJView(JView childJView) override;

            void removeChildJView(JView childJView) override;

            class IAndroidNavigationButtonHandler
            {
              public:
                virtual bool handleBackButton() = 0;
            };

            void setAndroidNavigationButtonHandler(std::shared_ptr<IAndroidNavigationButtonHandler> handler);

            void scheduleLayout() override;

            void initTag() override;

          protected:
            Rect getContentArea();

            /** Called when the window core's root view was garbage collected or
             * disposed.*/
            virtual void rootViewDisposed();

            /** Called when the root view that this window is attached to has
             * changed its size.
             *
             *  The default implementation updates the Window's size to match
             * the new root dimensions.
             *  */
            virtual void rootViewSizeChanged(int width, int height);

            /** Called when the configuration changed for this window core.
             *
             *  The default implementation updates the Window's size to match
             * the new root dimensions.
             *  */
            virtual void rootViewConfigurationChanged(JConfiguration config);

            virtual void attachedToNewRootView(const java::Reference &javaRef);

            virtual bool handleBackPressed();

          private:
            Rect getScreenWorkArea() const;

            void updateUIScaleFactor(JConfiguration config);

            static void getWindowCoreListFromRootView(const java::Reference &javaRootViewRef,
                                                      std::list<std::shared_ptr<WindowCore>> &windowCoreList);

            /** Returns an accessible reference to the window's root view.
             *
             *  Note that we only hold a weak reference to the root view, so the
             * root view may have been garbage collected. If it was then this
             * function will return a null reference.*/
            java::Reference tryGetAccessibleRootViewRef() const;

            class RootViewRegistry : public Base
            {
              public:
                void add(const java::WeakReference &javaRef) { _rootViewList.push_back(javaRef); }

                void remove(const java::Reference &javaRef)
                {
                    auto it = std::find(_rootViewList.begin(), _rootViewList.end(), javaRef);
                    if (it != _rootViewList.end())
                        _rootViewList.erase(it);
                }

                /** Returns the a strong java reference to the most recently
                 * created root view that is still valid.
                 *
                 *  The function automatically checks if the Java side object of
                 * a registered root view has already been garbage collected. If
                 * it has then the root view is considered invalid and will not
                 * be returned.
                 *
                 *  Returns a null reference if not valid root view is found.
                 *
                 * */
                java::Reference getNewestValidRootView()
                {
                    // So we can simply return the first root view from the
                    // list.
                    while (!_rootViewList.empty()) {
                        java::Reference javaRef = _rootViewList.back().toStrong();

                        if (!javaRef.isNull())
                            return javaRef;

                        // java-side object has been disposed or garbage
                        // collected. Remove the entry from the list.
                        _rootViewList.pop_back();
                    }

                    return java::Reference();
                }

              private:
                std::list<java::WeakReference> _rootViewList;
            };

          public:
            // Activities from different applications can sometimes run in the same process.
            // Make RootViewRegistry thread local to ensure access to most recently created root view.
            static RootViewRegistry &getRootViewRegistryForCurrentThread()
            {
                static thread_local RootViewRegistry registry;
                return registry;
            }

          private:
            mutable std::recursive_mutex _rootViewMutex;
            java::WeakReference _weakRootViewRef;

            Rect _currentBounds;

            std::shared_ptr<IAndroidNavigationButtonHandler> _navButtonHandler;
        };
    }
}
