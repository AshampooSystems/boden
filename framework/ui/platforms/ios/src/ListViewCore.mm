#import <UIKit/UIKit.h>
#import <bdn/ios/ListViewCore.hh>
#import <bdn/ios/util.hh>

#include <bdn/ListViewDataSource.h>

@interface ListViewDelegateIOS : NSObject <UITableViewDataSource, UITableViewDelegate>
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView;
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section;
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath;

@property(nonatomic, assign) std::weak_ptr<bdn::ListView> outer;
@end

@implementation ListViewDelegateIOS

- (std::shared_ptr<bdn::ListViewDataSource>)outerDataSource
{
    std::shared_ptr<bdn::ListView> outer = self.outer.lock();
    if (outer == nullptr) {
        return nullptr;
    }

    return outer->dataSource;
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView { return 1; }

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    if (self.outerDataSource == nullptr) {
        return 0;
    }

    return (NSInteger)self.outerDataSource->numberOfRows();
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (self.outerDataSource == nullptr) {
        return nil;
    }

    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"Cell"];

    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"Cell"];
    }

    cell.textLabel.text = bdn::ios::stringToNSString(self.outerDataSource->labelTextForRowIndex(indexPath.row));

    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    std::shared_ptr<bdn::ListView> outer = self.outer.lock();

    if (outer == nullptr) {
        return;
    }

    outer->selectedRowIndex = (size_t)indexPath.row;
}

@end

namespace bdn
{
    namespace ios
    {
        ListViewCore::ListViewCore(std::shared_ptr<ListView> outer) : ViewCore(outer, createUITableView(outer))
        {
            ListViewDelegateIOS *nativeDelegate = [[ListViewDelegateIOS alloc] init];
            nativeDelegate.outer = outer;
            _nativeDelegate = nativeDelegate;

            UITableView *uiTableView = (UITableView *)getUIView();
            uiTableView.dataSource = nativeDelegate;
            uiTableView.delegate = nativeDelegate;
        }

        void ListViewCore::reloadData() { [((UITableView *)getUIView())reloadData]; }

        UITableView *ListViewCore::createUITableView(std::shared_ptr<ListView> outer)
        {
            UITableView *uiTableView = [[UITableView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
            return uiTableView;
        }
    }
}
