#include <bdn/ui/ContainerView.h>
#include <bdn/ui/Styler.h>
#include <gtest/gtest.h>

namespace bdn
{
    using namespace bdn::ui;

    TEST(Styler, Simple)
    {
        auto view = std::make_shared<ContainerView>();
        Styler styler;

        styler.setStyleSheet(view, JsonStringify([ {"test" : 1} ]));
        EXPECT_EQ(view->stylesheet->at("test"), 1);
    }

    TEST(Styler, Combine)
    {
        auto view = std::make_shared<ContainerView>();
        Styler styler;

        styler.setStyleSheet(view, JsonStringify([ {"test" : 1}, {"hello" : "world"} ]));
        EXPECT_EQ(view->stylesheet->at("test"), 1);
        EXPECT_EQ(view->stylesheet->at("hello"), "world");
    }

    TEST(Styler, ConditionEqual)
    {
        auto view = std::make_shared<ContainerView>();
        Styler styler;
        styler.setCondition("test", std::make_shared<Styler::equals_condition>(42));

        styler.setStyleSheet(view, JsonStringify([ {"if" : {"test" : 10}, "test" : 1} ]));
        EXPECT_EQ(view->stylesheet->count("test"), 0);

        styler.setStyleSheet(view, JsonStringify([ {"if" : {"test" : 42}, "test" : 1} ]));
        EXPECT_EQ(view->stylesheet->count("test"), 1);
        EXPECT_EQ(view->stylesheet->at("test"), 1);
    }

    TEST(Styler, ConditionEqualUpdate)
    {
        auto view = std::make_shared<ContainerView>();
        Styler styler;
        styler.setCondition("test", std::make_shared<Styler::equals_condition>(42));

        styler.setStyleSheet(view, JsonStringify([ {"if" : {"test" : 10}, "test" : 1} ]));
        EXPECT_EQ(view->stylesheet->count("test"), 0);

        styler.setCondition("test", std::make_shared<Styler::equals_condition>(10));
        EXPECT_EQ(view->stylesheet->count("test"), 1);
        EXPECT_EQ(view->stylesheet->at("test"), 1);
    }

    TEST(Styler, CombineIf)
    {
        auto view = std::make_shared<ContainerView>();
        Styler styler;

        styler.setCondition("test", std::make_shared<Styler::equals_condition>(42));
        styler.setStyleSheet(view, JsonStringify([ {"test" : 1}, {"if" : {"test" : 42}, "hello" : "world"} ]));
        EXPECT_EQ(view->stylesheet->at("test"), 1);
        EXPECT_EQ(view->stylesheet->at("hello"), "world");
    }
}
