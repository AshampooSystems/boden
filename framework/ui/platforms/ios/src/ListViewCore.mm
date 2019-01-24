#import <UIKit/UIKit.h>
#import <bdn/ios/ListViewCore.hh>
#import <bdn/ios/util.hh>

#include <bdn/ListViewDataSource.h>

@interface ListViewDataSourceIOS : NSObject<UITableViewDataSource>
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView;
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section;
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath;

@property (nonatomic, assign) std::weak_ptr<bdn::ListViewDataSource> outerDataSource;
@end

@implementation ListViewDataSourceIOS

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
	return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    std::shared_ptr<bdn::ListViewDataSource> outerDataSource = self.outerDataSource.lock();
    
    if (outerDataSource == nullptr) {
        return 0;
    }
    
    return (NSInteger)outerDataSource->numberOfRows();
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    std::shared_ptr<bdn::ListViewDataSource> outerDataSource = self.outerDataSource.lock();

    if (outerDataSource == nullptr) {
        return nil;
    }

    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"Cell"];

	if (cell == nil) {
		cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"Cell"];
	}

    cell.textLabel.text = bdn::ios::stringToNSString(outerDataSource->labelTextForRowIndex(indexPath.row));

	return cell;
}

@end

namespace bdn
{
	namespace ios
	{
		ListViewCore::ListViewCore(std::shared_ptr<ListView> outerListView) : ViewCore(outerListView, createUITableView(outerListView))
		{
            ListViewDataSourceIOS *nativeDataSource = [[ListViewDataSourceIOS alloc] init];
            ((UITableView *)getUIView()).dataSource = nativeDataSource;
            _nativeDataSource = nativeDataSource;
            setDataSource(outerListView->dataSource);
		}

		void ListViewCore::setDataSource(const std::shared_ptr<ListViewDataSource> &dataSource)
		{
			_nativeDataSource.outerDataSource = dataSource;
		}
        
        void ListViewCore::reloadData()
        {
            [((UITableView *)getUIView()) reloadData];
        }

		UITableView *ListViewCore::createUITableView(std::shared_ptr<ListView> outerListView)
		{
            UITableView *uiTableView = [[UITableView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
			return uiTableView;
		}
	}
}
