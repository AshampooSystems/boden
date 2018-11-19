#ifndef BDN_TEST_TestMockViewCoreMixin_H_
#define BDN_TEST_TestMockViewCoreMixin_H_

#include <bdn/View.h>
#include <bdn/test/MockViewCore.h>
#include <bdn/test/MockUiProvider.h>

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
            TestMockViewCoreMixin() { _mockProvider = newObj<bdn::test::MockUiProvider>(); }

          protected:
            void initCore() override
            {
                BaseClass::initCore();

                _mockCore = cast<bdn::test::MockViewCore>(this->_view->getViewCore());
                REQUIRE(_mockCore != nullptr);
            }

            IUiProvider &getUiProvider() override { return *_mockProvider; }

            void verifyCoreVisibility() override
            {
                bool expectedVisible = this->_view->visible();

                REQUIRE(_mockCore->getVisible() == expectedVisible);
            }

            void verifyInitialDummyCoreSize() override
            {
                Size size = _mockCore->getBounds().getSize();
                REQUIRE(size == Size());
            }

            void verifyCorePosition() override
            {
                Point expectedPosition = this->_view->position();

                REQUIRE(_mockCore->getBounds().getPosition() == expectedPosition);
            }

            void verifyCoreSize() override
            {
                Size expectedSize = this->_view->size();

                REQUIRE(_mockCore->getBounds().getSize() == expectedSize);
            }

            void verifyCorePadding() override
            {
                Nullable<UiMargin> expectedPadding = this->_view->padding();

                if (expectedPadding.isNull())
                    REQUIRE(_mockCore->getPadding().isNull());
                else {
                    REQUIRE(!_mockCore->getPadding().isNull());

                    REQUIRE(_mockCore->getPadding().get() == expectedPadding.get());
                }
            }

            P<bdn::test::MockViewCore> _mockCore;
            P<bdn::test::MockUiProvider> _mockProvider;
        };
    }
}

#endif
