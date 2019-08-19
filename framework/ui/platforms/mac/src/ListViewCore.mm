#import <bdn/mac/ListViewCore.hh>

#include <bdn/log.h>
#include <bdn/mac/ContainerViewCore.hh>
#include <bdn/ui/ContainerView.h>

@interface FixedNSView : NSView <BdnLayoutable>
@property(nonatomic, assign) std::shared_ptr<bdn::ui::ContainerView> view;
@end

@implementation FixedNSView
- (BOOL)isFlipped { return YES; }
- (void)layout
{
    if (auto layout = _view->getLayout()) {
        layout->layout(_view.get());
    }
}
- (void)setFrame:(NSRect)newFrame
{
    _view->geometry = bdn::ui::mac::macRectToRect(newFrame, -1);
    [super setFrame:newFrame];
}
@end

@interface ListViewDelegateMac : NSObject <NSTableViewDataSource, NSTableViewDelegate>
@property(nonatomic, assign) std::weak_ptr<bdn::ui::mac::ListViewCore> listCore;
@end

@implementation ListViewDelegateMac

- (std::shared_ptr<bdn::ui::ListViewDataSource>)outerDataSource
{
    if (auto listCore = self.listCore.lock()) {
        return listCore->dataSource;
    }

    return nullptr;
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
    if (auto dataSource = self.outerDataSource) {
        return (NSInteger)dataSource->numberOfRows();
    }

    return 0;
}

- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    if (auto listCore = self.listCore.lock()) {
        if (auto dataSource = listCore->dataSource.get()) {
            std::shared_ptr<bdn::ui::ContainerView> container;
            std::shared_ptr<bdn::ui::View> view;

            FixedNSView *result = [tableView makeViewWithIdentifier:@"Column" owner:self];

            if (result == nil) {
                result = [[FixedNSView alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
                result.identifier = @"Column";

                container = std::make_shared<bdn::ui::ContainerView>(listCore->viewCoreFactory());
                container->isLayoutRoot = true;
                container->offerLayout(listCore->layout());

                if (auto core = container->core<bdn::ui::mac::ViewCore>()) {
                    [result addSubview:core->nsView()];
                } else {
                    throw std::runtime_error("View did not have the correct core");
                }

                result.view = container;
            } else {
                container = result.view;
                if (!container->childViews().empty()) {
                    view = container->childViews().front();
                }
            }

            view = dataSource->viewForRowIndex(row, view);

            container->removeAllChildViews();
            container->addChildView(view);

            return result;
        }
    }

    return nullptr;
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
    if (auto core = self.listCore.lock()) {
        NSTableView *tableView = (NSTableView *)notification.object;
        if (tableView.selectedRow != -1) {
            core->selectedRowIndex = (size_t)tableView.selectedRow;
        } else {
            core->selectedRowIndex = std::nullopt;
        }
    }
}

- (CGFloat)tableView:(NSTableView *)tableView heightOfRow:(NSInteger)row
{
    if (self.outerDataSource == nullptr) {
        return 20.0f;
    }
    return self.outerDataSource->heightForRowIndex(row);
}

@end

@interface ListScrollView : NSScrollView
@property NSTextView *refreshLabel;
@property(nonatomic, assign) std::weak_ptr<bdn::ui::mac::ListViewCore> listCore;
@property bool isInRefreshStatus;
@property(nonatomic) bool refreshEnabled;
@end

@implementation ListScrollView
- initWithFrame:(CGRect)rect
{
    self = [super initWithFrame:rect];
    if (self != nullptr) {
        self.isInRefreshStatus = NO;
        self.refreshEnabled = NO;

        self.refreshLabel = [[NSTextView alloc] init];
        [self addFloatingSubview:self.refreshLabel forAxis:NSEventGestureAxisVertical];
        self.refreshLabel.string = @"Refresh";

        self.refreshLabel.editable = static_cast<BOOL>(false);
        self.refreshLabel.selectable = static_cast<BOOL>(false);
        self.refreshLabel.richText = static_cast<BOOL>(false);
        self.refreshLabel.verticallyResizable = static_cast<BOOL>(false);

        [self.refreshLabel setWantsLayer:YES];
        [self.refreshLabel.layer setCornerRadius:10.0f];

        [self.refreshLabel setAlignment:NSTextAlignmentCenter range:NSMakeRange(0, 6)];

        self.refreshLabel.alphaValue = 0.0f;

        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(didScroll)
                                                     name:NSScrollViewDidLiveScrollNotification
                                                   object:self];
    }
    return self;
}

- (void)setRefreshEnabled:(bool)enable
{
    _refreshEnabled = enable;
    if (!_refreshEnabled) {
        self.refreshLabel.alphaValue = 0.0;
        self.isInRefreshStatus = NO;
    }
}

- (void)didScroll
{
    NSRect visibleRect = [[self contentView] documentVisibleRect];

    if (self.refreshEnabled) {
        if (!self.isInRefreshStatus) {
            self.refreshLabel.alphaValue = std::max(0.0, std::min(1.0, (-1.0 * (visibleRect.origin.y + 10)) / 25.0));
        }

        if (self.refreshLabel.alphaValue >= 1.0 && !self.isInRefreshStatus) {
            self.isInRefreshStatus = YES;
            self.refreshLabel.string = @"Refreshing";

            if (auto listCore = self.listCore.lock()) {
                listCore->fireRefresh();
            }
        } else {
            int numDots = (int)(self.refreshLabel.alphaValue * 4.0);
            switch (numDots) {
            case 1:
                self.refreshLabel.string = @"Refresh.";
                break;
            case 2:
                self.refreshLabel.string = @"Refresh..";
                break;
            case 3:
                self.refreshLabel.string = @"Refresh...";
                break;
            default:
                self.refreshLabel.string = @"Refresh...";
                break;
            }
        }
    }
}

- (void)setFrameSize:(NSSize)newSize
{
    [super setFrameSize:newSize];

    CGSize boundingRect = CGSizeMake(newSize.width, 50);

    CGRect r = [[self.refreshLabel textStorage]
        boundingRectWithSize:boundingRect
                     options:NSStringDrawingUsesLineFragmentOrigin | NSStringDrawingUsesFontLeading
                     context:nil];

    [self.refreshLabel.layer setCornerRadius:r.size.height / 3];

    self.refreshLabel.textContainerInset = CGSizeMake(0, 10);

    r.size.height += 20;

    [self.refreshLabel
        setFrame:CGRectMake((newSize.width / 2) - (r.size.width), newSize.height - (25 + (r.size.height / 2)),
                            (r.size.width) * 2, r.size.height)];
}

- (void)endRefreshing
{
    if (self.isInRefreshStatus) {
        self.isInRefreshStatus = NO;
        self.refreshLabel.alphaValue = 0.0;
        self.refreshLabel.string = @"Refresh";
    }
}
@end

namespace bdn::ui::detail
{
    CORE_REGISTER(ListView, bdn::ui::mac::ListViewCore, ListView)
}

namespace bdn::ui::mac
{
    ListViewCore::ListViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory)
        : ViewCore(viewCoreFactory, createNSTableView())
    {}

    ListViewCore::~ListViewCore() { _nativeDelegate.listCore = std::weak_ptr<ListViewCore>(); }

    void ListViewCore::init()
    {
        ListViewDelegateMac *nativeDelegate = [[ListViewDelegateMac alloc] init];
        nativeDelegate.listCore = shared_from_this<ListViewCore>();

        ListScrollView *scrollView = (ListScrollView *)nsView();
        scrollView.listCore = nativeDelegate.listCore;

        enableRefresh.onChange() += [=](auto &property) {
            ListScrollView *scrollView = (ListScrollView *)nsView();
            scrollView.refreshEnabled = property.get();
        };

        selectedRowIndex.onChange() += [=](auto &p) {
            if (!p.get()) {
                [_nsTableView deselectAll:nullptr];
            } else {
                NSIndexSet *newSet = [NSIndexSet indexSetWithIndex:*p.get()];
                [_nsTableView selectRowIndexes:(NSIndexSet *)newSet byExtendingSelection:NO];
            }
        };

        dataSource.onChange() += [=](auto prop) { reloadData(); };

        _nsTableView = (scrollView).documentView;
        _nsTableView.dataSource = nativeDelegate;
        _nsTableView.delegate = nativeDelegate;
        _nsTableView.headerView = nil;
        _nativeDelegate = nativeDelegate;
    }

    void ListViewCore::reloadData() { [_nsTableView reloadData]; }

    void ListViewCore::refreshDone()
    {
        ListScrollView *scrollView = (ListScrollView *)nsView();
        [scrollView endRefreshing];
    }

    void ListViewCore::fireRefresh() { _refreshCallback.fire(); }

    NSScrollView *ListViewCore::createNSTableView()
    {
        ListScrollView *nsScrollView = [[ListScrollView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
        NSTableView *nsTableView = [[NSTableView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];

        NSTableColumn *column = [[NSTableColumn alloc] initWithIdentifier:@"Column"];
        column.resizingMask = NSTableColumnAutoresizingMask;
        [nsTableView addTableColumn:column];

        nsTableView.rowSizeStyle = NSTableViewRowSizeStyleDefault;

        [nsScrollView setDocumentView:nsTableView];
        nsScrollView.hasVerticalScroller = YES;

        return nsScrollView;
    }
}
