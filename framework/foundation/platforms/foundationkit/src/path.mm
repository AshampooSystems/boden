#import <Foundation/Foundation.h>
#include <bdn/path.h>

namespace bdn::path
{
    namespace fk
    {
        std::string pathForDirectoryOfType(NSSearchPathDirectory directoryType)
        {
            NSArray *paths = NSSearchPathForDirectoriesInDomains(directoryType, NSUserDomainMask, YES);
            return [paths.firstObject cStringUsingEncoding:NSUTF8StringEncoding];
        }
    }

    std::string temporaryDirectoryPath() { return fk::pathForDirectoryOfType(NSCachesDirectory); }

    std::string applicationSupportDirectoryPath() { return fk::pathForDirectoryOfType(NSApplicationSupportDirectory); }

    std::string documentDirectoryPath() { return fk::pathForDirectoryOfType(NSDocumentDirectory); }
}
