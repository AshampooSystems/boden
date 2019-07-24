#include <bdn/ui.h>
#include <bdn/ui/yoga.h>
#include <functional>

#include "Page.h"

namespace bdn
{
    std::shared_ptr<View> createPageContainer(std::shared_ptr<View> page, String nextTitle,
                                              std::function<void()> nextFunc)
    {
        auto mainContainer = std::make_shared<ContainerView>();
        mainContainer->stylesheet = FlexJsonStringify(
            {"flexGrow" : 1.0, "padding" : {"left" : 28.0, "right" : 28.0, "top" : 15.0, "bottom" : 15.0}});

        mainContainer->addChildView(page);

        auto nextButton = std::make_shared<Button>();
        nextButton->label = nextTitle + " > ";
        nextButton->stylesheet =
            FlexJsonStringify({"flexShrink" : 0.0, "alignSelf" : "FlexEnd", "margin" : {"top" : 5}});

        nextButton->onClick() += [=](auto) { nextFunc(); };

        mainContainer->addChildView(nextButton);

        return mainContainer;
    }

    std::shared_ptr<View> makeRow(String title, std::shared_ptr<View> ctrl, double marginTop, double marginBottom,
                                  double contentRatio, bool baseline)
    {
        auto row = std::make_shared<ContainerView>();

        auto rowStylesheet = nlohmann::json{{"flex",
                                             {{"direction", FlexStylesheet::Direction::Row},
                                              {"minimumSize", {{"height", 40}}},
                                              {"alignItems", FlexStylesheet::Align::Center},
                                              {"justifyContent", FlexStylesheet::Align::SpaceBetween},
                                              {"flexShrink", 0},
                                              {"flexGrow", 0},
                                              {"margin", {{"top", marginTop}, {"bottom", marginBottom}}}}}};

        if (baseline) {
            rowStylesheet["flex"]["alignItems"] = FlexStylesheet::Align::Baseline;
        }

        row->stylesheet = rowStylesheet;

        auto leftColumn = std::make_shared<ContainerView>();
        leftColumn->stylesheet = nlohmann::json{{"flex",
                                                 {
                                                     {"direction", FlexStylesheet::Direction::Row},
                                                     {"flexGrow", 1. - contentRatio},
                                                     {"flexShrink", 0},
                                                     {"alignItems", FlexStylesheet::Align::Center},
                                                     {"justifyContent", FlexStylesheet::Align::FlexEnd},
                                                     {"size", {{"width", 1}}},
                                                     {"minimumSize", {{"height", 40}}},
                                                 }}};

        auto rightColumn = std::make_shared<ContainerView>();
        rightColumn->stylesheet = nlohmann::json{{"flex",
                                                  {{"direction", FlexStylesheet::Direction::Row},
                                                   {"flexGrow", contentRatio},
                                                   {"flexShrink", 0},
                                                   {"alignItems", FlexStylesheet::Align::Center},
                                                   {"justifyContent", FlexStylesheet::Align::FlexStart},
                                                   {"size", {{"width", 1}}},
                                                   {"minimumSize", {{"height", 40}}},
                                                   {"margin", {{"left", 10}}}}}};

        auto label = std::make_shared<Label>();
        label->text = title;
        leftColumn->addChildView(label);
        rightColumn->addChildView(ctrl);

        row->addChildView(leftColumn);
        row->addChildView(rightColumn);

        return row;
    }
}
