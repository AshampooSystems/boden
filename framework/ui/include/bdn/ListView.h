#pragma once

#include <bdn/View.h>
#include <bdn/ListViewDataSource.h>

namespace bdn
{
	class ListView : public View
	{
	public:
		ListView();
		virtual ~ListView() = default;

		void reloadData();

		static String getListViewCoreTypeName();
		String getCoreTypeName() const override;

		Property<std::shared_ptr<ListViewDataSource>> dataSource;
	};
}
