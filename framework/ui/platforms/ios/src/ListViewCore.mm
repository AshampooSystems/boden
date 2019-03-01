#import <UIKit/UIKit.h>
#import <bdn/ios/ListViewCore.hh>
#import <bdn/ios/util.hh>

#import <bdn/ios/ContainerViewCore.hh>

#include <bdn/FixedView.h>
#include <bdn/ListViewDataSource.h>

#include <iostream>

@interface FixedUITableViewCell : UIView <UIViewWithFrameNotification>
@property(nonatomic, assign) std::shared_ptr<bdn::FixedView> fixedView;
@property(nonatomic, assign) bdn::ios::ViewCore *viewCore;
@end

@implementation FixedUITableViewCell

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if (_viewCore) {
        _viewCore->frameChanged();
    }
}

- (void)layoutSubviews
{
    if (_viewCore) {
        if (auto view = _viewCore->getOuterViewIfStillAttached()) {
            if (auto layout = view->getLayout()) {
                layout->layout(view.get());
            }
        }
    }
}

@end

@interface FollowSizeUITableViewCell : UITableViewCell
@end
@implementation FollowSizeUITableViewCell

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    self.contentView.frame = frame;
    for (UIView *subview in self.contentView.subviews) {
        frame.origin.x = 0;
        frame.origin.y = 0;
        subview.frame = frame;
    }
}

@end

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
    FixedUITableViewCell *cellContent;

    auto listView = self.outer.lock();
    std::shared_ptr<bdn::FixedView> fixedView;
    std::shared_ptr<bdn::View> view;
    bool reuse = false;

    if (cell == nil) {
        cell = [[FollowSizeUITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"Cell"];
        cellContent = [[FixedUITableViewCell alloc] init];
        cellContent.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;

        fixedView = std::make_shared<bdn::FixedView>();

        fixedView->setViewCore(listView->getUIProvider(),
                               std::make_unique<bdn::ios::ContainerViewCore>(fixedView, cellContent));

        fixedView->offerLayout(self.outer.lock()->getLayout());

        cellContent.fixedView = fixedView;

        [cell.contentView addSubview:cellContent];
    } else {
        reuse = true;
        cellContent = cell.contentView.subviews.firstObject;
        fixedView = cellContent.fixedView;

        if (!fixedView->getChildViews().empty()) {
            view = fixedView->getChildViews().front();
        }
    }

    if (fixedView) {
        view = self.outerDataSource->viewForRowIndex(indexPath.row, view);
        if (!reuse) {
            fixedView->removeAllChildViews();
            fixedView->addChildView(view);
        }
    }

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

@interface BodenUITableView : UITableView <UIViewWithFrameNotification>
@property(nonatomic, assign) bdn::ios::ViewCore *viewCore;
@end

@implementation BodenUITableView
- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if (_viewCore) {
        _viewCore->frameChanged();
    }
}
@end

namespace bdn
{
    namespace ios
    {
        BodenUITableView *createUITableView(std::shared_ptr<ListView> outer)
        {
            BodenUITableView *uiTableView = [[BodenUITableView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
            return uiTableView;
        }

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
    }
}
