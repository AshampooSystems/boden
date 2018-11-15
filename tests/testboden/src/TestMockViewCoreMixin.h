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
            TestMockViewCoreMixin() { _pMockProvider = newObj<bdn::test::MockUiProvider>(); }

          protected:
            void initCore() override
            {
                BaseClass::initCore();

                _pMockCore = cast<bdn::test::MockViewCore>(this->_pView->getViewCore());
                REQUIRE(_pMockCore != nullptr);
            }

            IUiProvider &getUiProvider() override { return *_pMockProvider; }

            void verifyCoreVisibility() override
            {
                bool expectedVisible = this->_pView->visible();

                REQUIRE(_pMockCore->getVisible() == expectedVisible);
            }

            void verifyInitialDummyCoreSize() override
            {
                Size size = _pMockCore->getBounds().getSize();
                REQUIRE(size == Size());
            }

            void verifyCorePosition() override
            {
                Point expectedPosition = this->_pView->position();

                REQUIRE(_pMockCore->getBounds().getPosition() == expectedPosition);
            }

            void verifyCoreSize() override
            {
                Size expectedSize = this->_pView->size();

                REQUIRE(_pMockCore->getBounds().getSize() == expectedSize);
            }

            void verifyCorePadding() override
            {
                Nullable<UiMargin> expectedPadding = this->_pView->padding();

                if (expectedPadding.isNull())
                    REQUIRE(_pMockCore->getPadding().isNull());
                else {
                    REQUIRE(!_pMockCore->getPadding().isNull());

                    REQUIRE(_pMockCore->getPadding().get() == expectedPadding.get());
                }
            }

            P<bdn::test::MockViewCore> _pMockCore;
            P<bdn::test::MockUiProvider> _pMockProvider;
        };
    }
}

#endif
