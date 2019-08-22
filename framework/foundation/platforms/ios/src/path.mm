#import <Foundation/Foundation.h>
#include <bdn/path.h>

namespace bdn::path
{
    namespace ios
    {
        std::string pathForDirectoryOfType(NSSearchPathDirectory directoryType)
        {
            NSArray *paths = NSSearchPathForDirectoriesInDomains(directoryType, NSUserDomainMask, YES);
            return [paths.firstObject cStringUsingEncoding:NSUTF8StringEncoding];
        }
    }

    std::string temporaryDirectoryPath() { return ios::pathForDirectoryOfType(NSCachesDirectory); }

    std::string applicationSupportDirectoryPath() { return ios::pathForDirectoryOfType(NSApplicationSupportDirectory); }

    std::string documentDirectoryPath() { return ios::pathForDirectoryOfType(NSDocumentDirectory); }
}
