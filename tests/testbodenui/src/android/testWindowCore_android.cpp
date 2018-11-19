#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/Window.h>
#include <bdn/test/TestWindowCore.h>
#include <bdn/android/UiProvider.h>
#include <bdn/android/WindowCore.h>
#include "TestAndroidViewCoreMixin.h"

using namespace bdn;

class TestAndroidWindowCore : public bdn::test::TestAndroidViewCoreMixin<bdn::test::TestWindowCore>
{
  protected:
    void initCore() override { bdn::test::TestAndroidViewCoreMixin<bdn::test::TestWindowCore>::initCore(); }

    void verifyCoreTitle() override
    {
        // the title is ignored on android. So nothing to test here
    }

    bool canManuallyChangePosition() const override { return false; }

    bool canManuallyChangeSize() const override { return false; }

    /** Removes all references to the core object.*/
    void clearAllReferencesToCore() override
    {
        bdn::test::TestAndroidViewCoreMixin<bdn::test::TestWindowCore>::clearAllReferencesToCore();

        _androidViewCore = nullptr;
        _jView = bdn::android::JView();
    }

    struct DestructVerificationInfo : public Base
    {
        DestructVerificationInfo(bdn::android::JView jView) : jView(jView) {}

        bdn::android::JView jView;
    };

    P<IBase> createInfoToVerifyCoreUiElementDestruction() override
    {
        // sanity check
        REQUIRE(!_jView.isNull_());
        REQUIRE(!_jView.getParent().isNull_());

        return newObj<DestructVerificationInfo>(_jView);
    }

    void verifyCoreUiElementDestruction(IBase *verificationInfo) override
    {
        bdn::android::JView jv = cast<DestructVerificationInfo>(verificationInfo)->jView;

        // the view object should have been removed from its parent
        REQUIRE(jv.getParent().isNull_());
    }
};

TEST_CASE("android.WindowCore")
{
    P<TestAndroidWindowCore> test = newObj<TestAndroidWindowCore>();

    test->runTests();
}
