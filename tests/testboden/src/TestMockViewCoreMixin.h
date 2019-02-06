#ifndef BDN_TEST_TestMockViewCoreMixin_H_
#define BDN_TEST_TestMockViewCoreMixin_H_

#include <bdn/View.h>
#include <bdn/test/MockViewCore.h>
#include <bdn/test/MockUIProvider.h>

namespace bdn
{
    namespace test
    {

        /** A mixin class that adds implementations of mock view specific
           functionality on top of the base class specified in the template
           parameter BaseClass.*/
        template <class BaseClass> class TestMockViewCoreMixin : public BaseClass
        {
          public:
            TestMockViewCoreMixin() { _mockProvider = std::make_shared<bdn::test::MockUIProvider>(); }

          protected:
            void initCore() override
            {
                BaseClass::initCore();

                _mockCore = std::dynamic_pointer_cast<bdn::test::MockViewCore>(this->_view->getViewCore());
                REQUIRE(_mockCore != nullptr);
            }

            std::shared_ptr<UIProvider> getUIProvider() override { return _mockProvider; }

            void verifyCoreVisibility() override
            {
                bool expectedVisible = this->_view->visible;

                REQUIRE(_mockCore->getVisible() == expectedVisible);
            }

            void verifyInitialDummyCoreSize() override
            {
                Size size = _mockCore->getBounds().getSize();
                REQUIRE(size == Size());
            }

            void verifyCorePosition() override
            {
                Point expectedPosition = this->_view->position;

                REQUIRE(_mockCore->getBounds().getPosition() == expectedPosition);
            }

            void verifyCoreSize() override
            {
                Size expectedSize = this->_view->size;

                REQUIRE(_mockCore->getBounds().getSize() == expectedSize);
            }

            void verifyCorePadding() override
            {
                std::optional<UIMargin> expectedPadding = this->_view->padding;

                if (expectedPadding)
                    REQUIRE(_mockCore->getPadding());
                else {
                    REQUIRE(!_mockCore->getPadding());

                    REQUIRE(_mockCore->getPadding() == expectedPadding);
                }
            }

            std::shared_ptr<bdn::test::MockViewCore> _mockCore;
            std::shared_ptr<bdn::test::MockUIProvider> _mockProvider;
        };
    }
}

#endif
