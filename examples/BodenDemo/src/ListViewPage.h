#pragma once

#include "Page.h"

#include <bdn/property/TransformBacking.h>

namespace bdn
{
    class ListViewPage : public ui::CoreLess<ContainerView>
    {
      public:
        using CoreLess<ContainerView>::CoreLess;

        void init() override;

      private:
        std::shared_ptr<ListView> _listView = std::make_shared<ListView>();

        Property<std::string> stringIndex{
            TransformBacking<std::string, std::optional<size_t>>{_listView->selectedRowIndex,
                                                                 [](auto idx) {
                                                                     if (idx) {
                                                                         return std::to_string(*idx);
                                                                     }
                                                                     return std::string();
                                                                 },
                                                                 [](auto strIdx) -> std::optional<size_t> {
                                                                     if (strIdx.empty())
                                                                         return std::nullopt;
                                                                     std::istringstream sstr(strIdx);
                                                                     size_t v;
                                                                     sstr >> v;
                                                                     return v;
                                                                 }}};
    };
}
