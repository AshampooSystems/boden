#import <bdn/mac/UIUtil.hh>

namespace bdn::ui::mac
{
    bool imageFromUrl(const String &url, std::function<void(NSImage *)> callback)
    {
        auto uri = App()->uriToBundledFileUri(url);

        if (uri.empty()) {
            uri = url;
        }

        if (cpp20::starts_with(uri, "file:///")) {
            if (auto nsURL = [NSURL URLWithString:fk::stringToNSString(uri)]) {
                if (auto localPath = nsURL.relativePath) {
                    NSImage *image = [[NSImage alloc] initWithContentsOfFile:localPath];
                    callback(image);
                    return true;
                }
            }
        } else {
            NSURLSession *session = [NSURLSession sharedSession];
            NSURL *nsURL = [NSURL URLWithString:fk::stringToNSString(url)];

            if (nsURL != nullptr) {

                NSURLSessionDataTask *dataTask =
                    [session dataTaskWithURL:nsURL
                           completionHandler:^(NSData *_Nullable nsData, NSURLResponse *_Nullable nsResponse,
                                               NSError *_Nullable error) {
                             if (auto err = error) {
                                 logstream() << "Failed loading '" << fk::nsStringToString([nsURL absoluteString])
                                             << "' (" << fk::nsStringToString([err localizedDescription]) << ")";
                             } else {

                                 App()->dispatchQueue()->dispatchAsync([=]() {
                                     NSImage *image = [[NSImage alloc] initWithData:nsData];
                                     callback(image);
                                 });
                             }
                           }];

                [dataTask resume];
            }
        }
        return false;
    }
}
