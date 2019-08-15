#include <bdn/Application.h>
#include <bdn/ui/Button.h>
#include <bdn/ui/ContainerView.h>
#include <gtest/gtest.h>

namespace bdn
{
    using namespace bdn::ui;

    TEST(ContainerView, childIndex)
    {
        bdn::App()->dispatchQueue()->dispatchSync([=]() {
            const auto container = std::make_shared<ContainerView>();
            const auto children = {std::make_shared<Button>(), std::make_shared<Button>(), std::make_shared<Button>()};

            for (auto child : children) {
                container->addChildView(child);
            }

            int i = 0;
            for (auto it = children.begin(); it != children.end(); ++it, ++i) {
                EXPECT_EQ(container->childIndex(*it), i);
            }

            container->removeAllChildViews();

            for (auto it = children.begin(); it != children.end(); ++it) {
                EXPECT_EQ(container->childIndex(*it), std::nullopt);
            }
        });
    }
}
