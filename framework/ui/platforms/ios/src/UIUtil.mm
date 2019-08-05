#import <bdn/ios/UIUtil.hh>

#import <bdn/foundationkit/conversionUtil.hh>

namespace bdn::ui::ios
{
    bool imageFromUrl(const std::string &url, std::function<void(UIImage *)> callback)
    {
        auto uri = App()->uriToBundledFileUri(url);

        if (uri.empty()) {
            uri = url;
        }

        if (cpp20::starts_with(uri, "file:///")) {
            if (auto nsURL = [NSURL URLWithString:fk::stringToNSString(uri)]) {
                if (auto localPath = nsURL.relativePath) {
                    UIImage *image = [[UIImage alloc] initWithContentsOfFile:localPath];
                    callback(image);
                    return true;
                }
            }
        } else {
            NSURLSession *session = [NSURLSession sharedSession];
            NSURL *nsURL = [NSURL URLWithString:fk::stringToNSString(url)];

            if (nsURL == nullptr) {
                return false;
            }

            NSURLSessionDataTask *dataTask =
                [session dataTaskWithURL:nsURL
                       completionHandler:^(NSData *_Nullable nsData, NSURLResponse *_Nullable nsResponse,
                                           NSError *_Nullable error) {
                         if (auto err = error) {
                             logstream() << "Failed loading '" << fk::nsStringToString([nsURL absoluteString]) << "' ("
                                         << fk::nsStringToString([err localizedDescription]) << ")";
                         } else {

                             App()->dispatchQueue()->dispatchAsync([nsData, callback]() {
                                 UIImage *image = [[UIImage alloc] initWithData:nsData];
                                 callback(image);
                             });
                         }
                       }];

            [dataTask resume];
        }

        return false;
    }
}
