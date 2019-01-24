#pragma once

#include <bdn/android/ViewCore.h>
#include <bdn/android/JListView.h>
#include <bdn/ListView.h>
#include <bdn/ListViewCore.h>

namespace bdn
{
	namespace android
	{
		class JNativeListAdapter;

		class ListViewCore : public ViewCore, virtual public bdn::ListViewCore
		{
		public:
			ListViewCore(std::shared_ptr<ListView> outerView);
			
			virtual void setDataSource(const std::shared_ptr<ListViewDataSource> &dataSource) override;
            virtual void reloadData() override;

		private:
			static std::shared_ptr<JListView> createJListView(std::shared_ptr<ListView> outer);

	        std::shared_ptr<JListView> _jListView;
	        std::shared_ptr<JNativeListAdapter> _jNativeListAdapter;
		};
	}
}