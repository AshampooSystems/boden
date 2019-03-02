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
@end

@interface ListViewDelegateMac : NSObject <NSTableViewDataSource, NSTableViewDelegate>
@property(nonatomic, assign) std::weak_ptr<bdn::ListView> outer;
@end

@implementation ListViewDelegateMac

- (std::shared_ptr<bdn::ListViewDataSource>)outerDataSource
{
    std::shared_ptr<bdn::ListView> outer = self.outer.lock();
    if (outer == nullptr) {
        return nullptr;
    }

    return outer->dataSource;
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
    if (self.outerDataSource == nullptr) {
        return nil;
    }

    auto listView = self.outer.lock();
    std::shared_ptr<bdn::FixedView> fixedView;
    std::shared_ptr<bdn::View> view;

    FixedNSView *result = [tableView makeViewWithIdentifier:@"Column" owner:self];

    if (result == nil) {
        result = [[FixedNSView alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        result.identifier = @"Column";

        fixedView = std::make_shared<bdn::FixedView>();

        fixedView->_setParentView(listView);

        fixedView->setViewCore(listView->uiProvider(),
                               std::make_unique<bdn::mac::ContainerViewCore>(fixedView, result));

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
    std::shared_ptr<bdn::ListView> outer = self.outer.lock();

    if (outer == nullptr) {
        return;
    }

    NSTableView *tableView = (NSTableView *)notification.object;
    outer->selectedRowIndex = (size_t)tableView.selectedRow;
}

@end

namespace bdn
{
    namespace mac
    {
        ListViewCore::ListViewCore(std::shared_ptr<ListView> outer) : ChildViewCore(outer, createNSTableView(outer))
        {
            ListViewDelegateMac *nativeDelegate = [[ListViewDelegateMac alloc] init];
            nativeDelegate.outer = outer;

            _nsTableView = ((NSScrollView *)nsView()).documentView;
            _nsTableView.dataSource = nativeDelegate;
            _nsTableView.delegate = nativeDelegate;
            _nsTableView.headerView = nil;
            _nativeDelegate = nativeDelegate;
        }

        void ListViewCore::reloadData() { [_nsTableView reloadData]; }

        NSScrollView *ListViewCore::createNSTableView(std::shared_ptr<ListView> outer)
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
}
