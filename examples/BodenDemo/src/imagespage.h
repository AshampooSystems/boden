#pragma once

#include <bdn/ui/ContainerView.h>
#include <bdn/ui/ImageView.h>
#include <bdn/ui/Json.h>
#include <bdn/ui/Label.h>
#include <bdn/ui/TextField.h>
#include <bdn/ui/yoga/FlexStylesheet.h>

namespace bdn
{
    void update(const std::shared_ptr<ui::ImageView> &imageView)
    {
        auto j = json{
            {"flex",
             {{"aspectRatio", imageView->aspectRatio.get()},
              {"maximumSize",
               {{"width", imageView->originalSize->width * 2}, {"height", imageView->originalSize->height * 2}}}}}};
        imageView->stylesheet = j;
    }

    void createSizeBindings(const std::shared_ptr<ui::ImageView> &imageView)
    {
        imageView->originalSize.onChange() += [=](auto &property) { update(imageView); };
        imageView->aspectRatio.onChange() += [=](auto &property) { update(imageView); };
    };

    auto createImageViewDemoPage()
    {
        auto mainContainer = std::make_shared<ui::ContainerView>();
        mainContainer->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        auto image = std::make_shared<ui::ImageView>();
        createSizeBindings(image);
        image->url = "image://main/images/logo.png";

        auto label = std::make_shared<ui::Label>();
        label->text = "Image from resource:";
        label->stylesheet = FlexJsonStringify({"flexShrink" : 0});

        mainContainer->addChildView(label);
        mainContainer->addChildView(image);

        auto imageAsset = std::make_shared<ui::ImageView>();
        createSizeBindings(imageAsset);
        imageAsset->url = "asset://main/images/asset_image.png";

        auto labelAsset = std::make_shared<ui::Label>();
        labelAsset->text = "Image from Assets:";
        labelAsset->stylesheet = FlexJsonStringify({"flexShrink" : 0});

        mainContainer->addChildView(labelAsset);
        mainContainer->addChildView(imageAsset);

        auto imageWeb = std::make_shared<ui::ImageView>();
        createSizeBindings(imageWeb);
        imageWeb->url = "https://testing.boden.io/boden-logo-colored.png";

        auto labelWeb = std::make_shared<ui::Label>();
        labelWeb->text = "Image from web:";
        labelWeb->stylesheet = FlexJsonStringify({"flexShrink" : 0});

        mainContainer->addChildView(labelWeb);
        mainContainer->addChildView(imageWeb);

        return mainContainer;
    }
}
