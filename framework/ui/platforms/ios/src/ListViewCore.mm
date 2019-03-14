#import <UIKit/UIKit.h>
#import <bdn/ios/ListViewCore.hh>
#import <bdn/ios/util.hh>

#import <bdn/ios/ContainerViewCore.hh>

#include <bdn/FixedView.h>
#include <bdn/ListViewDataSource.h>

#include <bdn/log.h>

/*@interface FixedUITableViewCell : UIView <UIViewWithFrameNotification>
@property(nonatomic, assign) std::shared_ptr<bdn::FixedView> fixedView;
@property(nonatomic, assign) std::weak_ptr<bdn::ios::ViewCore> viewCore;
@end

@implementation FixedUITableViewCell

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if (auto core = _viewCore.lock()) {
        core->frameChanged();
    }
}

- (void)layoutSubviews
{
    if (auto layout = _fixedView->getLayout()) {
        layout->layout(_fixedView.get());
    }
}

@end
 */

@interface FollowSizeUITableViewCell : UITableViewCell
@property std::shared_ptr<bdn::FixedView> fixedView;
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

@property(nonatomic, assign) std::weak_ptr<bdn::ios::ListViewCore> core;
@end

@implementation ListViewDelegateIOS

- (std::shared_ptr<bdn::ListViewDataSource>)outerDataSource
{
    auto core = self.core.lock();
    if (core == nullptr) {
        return nullptr;
    }

    return core->dataSource;
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView { return 1; }

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    if (auto dataSource = self.outerDataSource) {
        return (NSInteger)dataSource->numberOfRows();
    }
    return 0;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (auto dataSource = self.outerDataSource) {
        return dataSource->heightForRowIndex(indexPath.row);
    }

    return 20.0;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (auto dataSource = self.outerDataSource) {
        FollowSizeUITableViewCell *cell =
            (FollowSizeUITableViewCell *)[tableView dequeueReusableCellWithIdentifier:@"Cell"];

        std::shared_ptr<bdn::FixedView> fixedView;
        std::shared_ptr<bdn::View> view;
        bool reuse = false;

        auto core = self.core.lock();

        if (cell == nil) {
            cell =
                [[FollowSizeUITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"Cell"];
            cell.clipsToBounds = YES;

            fixedView = std::make_shared<bdn::FixedView>(core->uiProvider());
            fixedView->offerLayout(core->layout());

            [cell.contentView addSubview:fixedView->core<bdn::ios::ViewCore>()->uiView()];
            cell.fixedView = fixedView;

        } else {
            reuse = true;
            fixedView = cell.fixedView;

            if (!fixedView->childViews().empty()) {
                view = fixedView->childViews().front();
            }
        }

        if (fixedView) {
            view = self.outerDataSource->viewForRowIndex(indexPath.row, view);
            if (!reuse) {
                fixedView->removeAllChildViews();
                fixedView->addChildView(view);
            }
            fixedView->scheduleLayout();
        }

        return cell;
    }
    return nil;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    auto core = self.core.lock();

    if (core == nullptr) {
        return;
    }

    core->selectedRowIndex = (size_t)indexPath.row;
}

@end

@interface BodenUITableView : UITableView <UIViewWithFrameNotification>
@property(nonatomic, assign) std::weak_ptr<bdn::ios::ViewCore> viewCore;
@end

@implementation BodenUITableView
- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if (auto viewCore = self.viewCore.lock()) {
        viewCore->frameChanged();
    }
}
@end

namespace bdn::ios
{
    BodenUITableView *createUITableView()
    {
        BodenUITableView *uiTableView = [[BodenUITableView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
        return uiTableView;
    }

    ListViewCore::ListViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider)
        : ViewCore(uiProvider, createUITableView())
    {}

    void ListViewCore::init()
    {
        ViewCore::init();

        ListViewDelegateIOS *nativeDelegate = [[ListViewDelegateIOS alloc] init];
        _nativeDelegate = nativeDelegate;
        _nativeDelegate.core = std::dynamic_pointer_cast<ListViewCore>(shared_from_this());

        UITableView *uiTableView = (UITableView *)uiView();
        uiTableView.dataSource = nativeDelegate;
        uiTableView.delegate = nativeDelegate;
    }

    void ListViewCore::reloadData() { [((UITableView *)uiView())reloadData]; }
}
