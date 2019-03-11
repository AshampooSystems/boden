#include <bdn/FixedView.h>
#include <bdn/mac/ContainerViewCore.hh>

#import <bdn/mac/ListViewCore.hh>

@interface FixedNSView : NSView <BdnLayoutable>
@property(nonatomic, assign) std::shared_ptr<bdn::FixedView> view;
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
    _view->geometry = bdn::mac::macRectToRect(newFrame, -1);
    [super setFrame:newFrame];
}
@end

@interface ListViewDelegateMac : NSObject <NSTableViewDataSource, NSTableViewDelegate>
@property(nonatomic, assign) std::weak_ptr<bdn::mac::ListViewCore> listCore;
@end

@implementation ListViewDelegateMac

- (std::shared_ptr<bdn::ListViewDataSource>)outerDataSource
{
    if (auto listCore = self.listCore.lock()) {
        return listCore->dataSource;
    }

    return nullptr;
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
    if (self.outerDataSource == nullptr) {
        return 0;
    }

    return (NSInteger)self.outerDataSource->numberOfRows();
}

- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    if (self.listCore.lock() == nullptr) {
        return nil;
    }

    if (self.outerDataSource == nullptr) {
        return nil;
    }

    std::shared_ptr<bdn::FixedView> fixedView;
    std::shared_ptr<bdn::View> view;

    FixedNSView *result = [tableView makeViewWithIdentifier:@"Column" owner:self];

    if (result == nil) {
        result = [[FixedNSView alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        result.identifier = @"Column";

        fixedView = std::make_shared<bdn::FixedView>(self.listCore.lock()->_uiProvider);
        fixedView->offerLayout(self.listCore.lock()->_layout);

        if (auto core = fixedView->core<bdn::mac::ViewCore>()) {
            [result addSubview:core->nsView()];
        } else {
            throw std::runtime_error("View did not have the correct core");
        }

        result.view = fixedView;
    } else {
        fixedView = result.view;
        if (!fixedView->childViews().empty()) {
            view = fixedView->childViews().front();
        }
    }

    view = self.outerDataSource->viewForRowIndex(row, view);

    fixedView->removeAllChildViews();
    fixedView->addChildView(view);

    return result;
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
    if (auto core = self.listCore.lock()) {
        NSTableView *tableView = (NSTableView *)notification.object;
        core->selectedRowIndex = (size_t)tableView.selectedRow;
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

namespace bdn::mac
{
    ListViewCore::ListViewCore() : ViewCore(createNSTableView()) {}

    ListViewCore::~ListViewCore() { _nativeDelegate.listCore = std::weak_ptr<ListViewCore>(); }

    void ListViewCore::init()
    {
        ListViewDelegateMac *nativeDelegate = [[ListViewDelegateMac alloc] init];
        nativeDelegate.listCore = std::dynamic_pointer_cast<ListViewCore>(shared_from_this());

        _nsTableView = ((NSScrollView *)nsView()).documentView;
        _nsTableView.dataSource = nativeDelegate;
        _nsTableView.delegate = nativeDelegate;
        _nsTableView.headerView = nil;
        _nativeDelegate = nativeDelegate;
    }

    void ListViewCore::reloadData() { [_nsTableView reloadData]; }

    NSScrollView *ListViewCore::createNSTableView()
    {
        NSScrollView *nsScrollView = [[NSScrollView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
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
