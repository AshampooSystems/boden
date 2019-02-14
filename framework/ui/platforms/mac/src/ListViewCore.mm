#import <bdn/mac/ListViewCore.hh>

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

    NSTextField *result = [tableView makeViewWithIdentifier:@"Column" owner:self];

    if (result == nil) {
        result = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        result.identifier = @"Column";
    }

    result.stringValue = bdn::mac::stringToNSString(self.outerDataSource->labelTextForRowIndex(row));

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

            _nsTableView = ((NSScrollView *)getNSView()).documentView;
            _nsTableView.dataSource = nativeDelegate;
            _nsTableView.delegate = nativeDelegate;
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
