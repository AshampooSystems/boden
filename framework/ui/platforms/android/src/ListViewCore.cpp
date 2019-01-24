#include <bdn/android/ListViewCore.h>
#include <bdn/android/JNativeListAdapter.h>

namespace bdn
{
	namespace android
	{
        std::shared_ptr<JListView> ListViewCore::createJListView(std::shared_ptr<ListView> outer)
        {
            std::shared_ptr<View> parent = outer->getParentView();
            if (parent == nullptr)
                throw ProgrammingError("ListViewCore instance requested for a ListView that does "
                                       "not have a parent.");

            std::shared_ptr<ViewCore> parentCore = std::dynamic_pointer_cast<ViewCore>(parent->getViewCore());
            if (parentCore == nullptr)
                throw ProgrammingError("ListViewCore instance requested for "
                                       "a ListView with core-less parent.");

            JContext context = parentCore->getJView().getContext();

            return std::make_shared<JListView>(context);
        }

		ListViewCore::ListViewCore(std::shared_ptr<ListView> outer) : ViewCore(outer, createJListView(outer))
		{
	        _jListView = std::dynamic_pointer_cast<JListView>(getJViewPtr());

	        _jNativeListAdapter = std::make_shared<bdn::android::JNativeListAdapter>(*_jListView);
	        _jListView->setAdapter(*_jNativeListAdapter);
		}

		void ListViewCore::setDataSource(const std::shared_ptr<ListViewDataSource> &dataSource)
		{
		}

		void ListViewCore::reloadData()
		{
			_jNativeListAdapter->notifyDataSetChanged();
		}
	}
}