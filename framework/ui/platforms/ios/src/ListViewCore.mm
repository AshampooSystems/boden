#import <UIKit/UIKit.h>
#import <bdn/ios/ListViewCore.hh>
#import <bdn/ios/util.hh>

#import <bdn/ios/ContainerViewCore.hh>

#include <bdn/ContainerView.h>
#include <bdn/ListViewDataSource.h>

#include <bdn/log.h>

@interface FollowSizeUITableViewCell : UITableViewCell
@property std::shared_ptr<bdn::ContainerView> containerView;
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

        std::shared_ptr<bdn::ContainerView> containerView;
        std::shared_ptr<bdn::View> view;
        bool reuse = false;

        auto core = self.core.lock();

        if (cell == nil) {
            cell =
                [[FollowSizeUITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"Cell"];
            cell.clipsToBounds = YES;

            containerView = std::make_shared<bdn::ContainerView>(core->viewCoreFactory());
            containerView->isLayoutRoot = true;
            containerView->offerLayout(core->layout());

            [cell.contentView addSubview:containerView->core<bdn::ios::ViewCore>()->uiView()];
            cell.containerView = containerView;

        } else {
            reuse = true;
            containerView = cell.containerView;

            if (!containerView->childViews().empty()) {
                view = containerView->childViews().front();
            }
        }

        if (containerView) {
            view = self.outerDataSource->viewForRowIndex(indexPath.row, view);
            if (!reuse) {
                containerView->removeAllChildViews();
                containerView->addChildView(view);
            }
            containerView->scheduleLayout();
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

- (void)handleRefresh
{
    if (auto viewCore = std::dynamic_pointer_cast<bdn::ios::ListViewCore>(self.viewCore.lock())) {
        viewCore->fireRefresh();
    }
}

- (void)updateRefresh:(bool)enable
{
    if (enable && (self.refreshControl == nullptr)) {
        self.refreshControl = [[UIRefreshControl alloc] init];
        [self.refreshControl addTarget:self
                                action:@selector(handleRefresh)
                      forControlEvents:UIControlEventValueChanged];
    } else if (!enable) {
        self.refreshControl = nil;
    }
}
- (void)refreshDone
{
    if (self.refreshControl != nullptr) {
        [self.refreshControl endRefreshing];
    }
}
@end

namespace bdn::detail
{
    CORE_REGISTER(ListView, bdn::ios::ListViewCore, ListView)
}

namespace bdn::ios
{
    BodenUITableView *createUITableView()
    {
        BodenUITableView *uiTableView = [[BodenUITableView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
        return uiTableView;
    }

    ListViewCore::ListViewCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createUITableView())
    {}

    void ListViewCore::init()
    {
        ViewCore::init();

        ListViewDelegateIOS *nativeDelegate = [[ListViewDelegateIOS alloc] init];
        _nativeDelegate = nativeDelegate;
        _nativeDelegate.core = shared_from_this<ListViewCore>();

        UITableView *uiTableView = (UITableView *)uiView();
        uiTableView.dataSource = nativeDelegate;
        uiTableView.delegate = nativeDelegate;

        enableRefresh.onChange() += [=](auto &property) { updateRefresh(property.get()); };
    }

    void ListViewCore::updateRefresh(bool enable) { [((BodenUITableView *)uiView()) updateRefresh:enable]; }

    void ListViewCore::reloadData() { [((UITableView *)uiView())reloadData]; }

    void ListViewCore::refreshDone() { [((BodenUITableView *)uiView())refreshDone]; }

    void ListViewCore::fireRefresh() { _refreshCallback.fire(); }
}
