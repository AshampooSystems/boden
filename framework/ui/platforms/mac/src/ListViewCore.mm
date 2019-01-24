#import <bdn/mac/ListViewCore.hh>

@interface ListViewDataSourceMac : NSObject<NSTableViewDataSource, NSTableViewDelegate>

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView;

@property (nonatomic, assign) std::weak_ptr<bdn::ListViewDataSource> outerDataSource;

@end

@implementation ListViewDataSourceMac

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
    std::shared_ptr<bdn::ListViewDataSource> outerDataSource = self.outerDataSource.lock();
    
    if (outerDataSource == nullptr) {
        return 0;
    }
    
    return (NSInteger)outerDataSource->numberOfRows();
}

- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    std::shared_ptr<bdn::ListViewDataSource> outerDataSource = self.outerDataSource.lock();
    
    if (outerDataSource == nullptr) {
        return 0;
    }

    NSTextField *result = [tableView makeViewWithIdentifier:@"Column" owner:self];
 
    if (result == nil) {
        result = [[NSTextField alloc] initWithFrame:NSMakeRect(0,0,0,0)];
        result.identifier = @"Column";
    }
 
	result.stringValue = bdn::mac::stringToNSString(outerDataSource->labelTextForRowIndex(row));
 
	return result; 
}

@end

namespace bdn
{
	namespace mac
	{
		ListViewCore::ListViewCore(std::shared_ptr<ListView> outerListView) : ChildViewCore(outerListView, createNSTableView(outerListView))
		{
            ListViewDataSourceMac *nativeDataSource = [[ListViewDataSourceMac alloc] init];
            _nsTableView = ((NSScrollView *)getNSView()).documentView;
            _nsTableView.dataSource = nativeDataSource;
            _nsTableView.delegate = nativeDataSource;
            _nativeDataSource = nativeDataSource;
            setDataSource(outerListView->dataSource);
		}

		void ListViewCore::setDataSource(const std::shared_ptr<ListViewDataSource> &dataSource)
		{
			_nativeDataSource.outerDataSource = dataSource;
		}
        
        void ListViewCore::reloadData()
        {
            [_nsTableView reloadData];
        }

		NSScrollView *ListViewCore::createNSTableView(std::shared_ptr<ListView> outerListView)
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
