#include <bdn/foundation.h>
#include <bdn/log.h>
#include <bdn/net.h>
#include <bdn/ui.h>
#include <bdn/ui/yoga.h>

#include <nlohmann/json.hpp>

#include <bdn/Application.h>

#include <iostream>
#include <utility>

using namespace bdn;

using namespace nlohmann;

class RedditPost
{
  public:
    RedditPost() = default;
    RedditPost(const std::string &title_, const std::string &url_, const std::string &thumbnailUrl_)
        : title(title_), url(url_), thumbnailUrl(thumbnailUrl_)
    {}

    Property<std::string> title;
    Property<std::string> url;
    Property<std::string> thumbnailUrl;
};

class RedditStore
{
  public:
    void fetchPosts(const std::function<void()> &doneHandler)
    {
        net::http::request(
            {bdn::net::http::Method::GET, "https://www.reddit.com/hot.json?limit=100", [this, doneHandler](auto r) {
                 try {
                     json j = json::parse(r->data);

                     for (auto child : j["data"]["children"]) {
                         auto post = std::make_shared<RedditPost>();
                         post->title = child["data"]["title"];
                         post->url = child["data"]["url"];

                         std::string thumbnail = child["data"]["thumbnail"];

                         if (cpp20::starts_with(thumbnail, "https://")) {
                             post->thumbnailUrl = thumbnail;
                         } else {
                             post->thumbnailUrl =
                                 "https://www.redditstatic.com/desktop2x/img/favicon/apple-icon-180x180.png";
                         }
                         posts.push_back(post);
                     }

                     doneHandler();
                 }
                 catch (const json::parse_error &parseError) {
                     bdn::logstream() << "Error parsing json: " << parseError.what();
                 }
             }});
    }

    std::vector<std::shared_ptr<RedditPost>> posts;
};

class RedditListViewDataSource : public ui::ListViewDataSource
{
    class Delegate : public ui::CoreLess<ui::ContainerView>
    {
      public:
        Property<std::string> text;
        Property<std::string> imageUrl;

      public:
        using ui::CoreLess<ui::ContainerView>::CoreLess;

        void init() override
        {
            stylesheet = FlexJsonStringify({
                "direction" : "Row",
                "flexGrow" : 1.0,
                "justifyContent" : "SpaceBetween",
                "alignItems" : "FlexStart",
                "padding" : {"all" : 2.5}
            });

            auto image = std::make_shared<ui::ImageView>();
            image->url.bind(imageUrl, BindMode::unidirectional);
            image->stylesheet = FlexJsonStringify(
                {"maximumSize" : {"width" : 45.0}, "alignSelf" : "Center", "margin" : {"right" : 5.0}});

            addChildView(image);

            auto textView = std::make_shared<ui::Label>();
            textView->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

            addChildView(textView);

            textView->text.bind(text);
            textView->wrap = true;
        }
    };

  public:
    explicit RedditListViewDataSource(std::shared_ptr<RedditStore> store) { _store = std::move(store); }

    size_t numberOfRows(const std::shared_ptr<bdn::ui::ListView> &) override { return _store->posts.size(); }

    float heightForRowIndex(const std::shared_ptr<bdn::ui::ListView> &, size_t rowIndex) override { return 50; }

    std::shared_ptr<ui::View> viewForRowIndex(const std::shared_ptr<ui::ListView> &, size_t rowIndex,
                                              std::shared_ptr<ui::View> reusableView) override
    {
        std::shared_ptr<Delegate> delegate;

        if (reusableView) {
            delegate = std::dynamic_pointer_cast<Delegate>(reusableView);
        } else {
            delegate = std::make_shared<Delegate>();
        }

        std::string text = _store->posts.at(rowIndex)->title;

        delegate->text = text;
        delegate->imageUrl = _store->posts.at(rowIndex)->thumbnailUrl;
        return delegate;
    }

  private:
    std::shared_ptr<RedditStore> _store;
};

class PostListViewController
{
  public:
    using clickNotifier_t = Notifier<std::string, std::string, std::string>;

    PostListViewController() : _listView(std::make_shared<ui::ListView>())
    {
        _listView->enableRefresh = true;
        _listView->stylesheet = FlexJsonStringify(
            {"direction" : "Column", "flexGrow" : 1.0, "flexShrink" : 1.0, "margin" : {"all" : 10.0}});

        // FlexDirection(FlexStylesheet::Direction::Column)
        //                                     << FlexGrow(1.0f) << FlexShrink(1.0f) << FlexMarginAll(10.0f));

        _store = std::make_shared<RedditStore>();
        _dataSource = std::make_shared<RedditListViewDataSource>(_store);

        _listView->dataSource = _dataSource;
        _listView->onRefresh() += [this]() { updatePosts(); };

        updatePosts();

        _listView->selectedRowIndex.onChange() += [this](auto &property) {
            if (property.get()) {
                auto post = _store->posts.at(*property.get());
                _onClicked.notify(post->title, post->url, post->thumbnailUrl);
            }
        };
    }

    void updatePosts()
    {
        _store->fetchPosts([this]() {
            _listView->reloadData();
            _listView->refreshDone();
        });
    }

    void deselect() { _listView->selectedRowIndex = std::nullopt; }

    std::shared_ptr<ui::View> view() { return _listView; }

    clickNotifier_t &onClicked() { return _onClicked; }

  private:
    std::shared_ptr<RedditListViewDataSource> _dataSource;

    std::shared_ptr<ui::ListView> _listView;
    std::shared_ptr<RedditStore> _store;

    clickNotifier_t _onClicked;
};

class PostDetailController
{
  public:
    PostDetailController(const std::string &title, const std::string &url, const std::string &imageUrl)
        : _mainColumn(std::make_shared<ui::ContainerView>())
    {
        _mainColumn->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        auto headerColumn = std::make_shared<ui::ContainerView>();

        headerColumn->stylesheet = FlexJsonStringify(
            {"direction" : "Row", "alignItems" : "FlexStart", "margin" : {"bottom" : 10.0}, "flexWrap" : "Wrap"});

        /*FlexDirection(FlexStylesheet::Direction::Row)
                                      << FlexAlignItems(FlexStylesheet::Align::FlexStart) << FlexMarginBottom(10.0f)
                                      << FlexWrap(FlexStylesheet::Wrap::Wrap));
*/
        auto image = std::make_shared<ui::ImageView>();
        auto titleField = std::make_shared<ui::Label>();

        image->stylesheet = FlexJsonStringify({
            "flexGrow" : 0.0,
            "maximumSize" : {"width" : 100.0, "height" : 100.0},
            "flexShrink" : 0.0,
            "margin" : {"right" : 5}
        });

        titleField->stylesheet =
            FlexJsonStringify({"flexGrow" : 1.0, "flexShrink" : 1.0, "maximumSize" : {"height" : 100.0}});

        headerColumn->addChildView(image);
        headerColumn->addChildView(titleField);

        auto webView = std::make_shared<ui::WebView>();
        auto openButton = std::make_shared<ui::Button>();

        webView->userAgent = "boden-reddit/0.1";
        webView->url = url;
        webView->stylesheet = FlexJsonStringify({"flexGrow" : 1.0});

        image->url = imageUrl;
        titleField->text = title;

        openButton->label = "Open in Browser";
        openButton->onClick() += [url](auto) { App()->openURL(url); };

        _mainColumn->addChildView(headerColumn);
        _mainColumn->addChildView(webView);
        _mainColumn->addChildView(openButton);
    }

    std::shared_ptr<ui::View> view() { return _mainColumn; }

  private:
    std::shared_ptr<ui::ContainerView> _mainColumn;
};

class MainViewController
{
  public:
    MainViewController() : _listViewController(std::make_shared<PostListViewController>())
    {
        _window = std::make_shared<ui::Window>();
        _window->title = "UI Demo";
        _window->geometry = Rect{0, 0, 1024, 768};
        _window->setLayout(std::make_shared<ui::yoga::Layout>());

        auto navigationView = std::make_shared<ui::NavigationView>();
        navigationView->stylesheet = FlexJsonStringify({
            "direction" : "Column",
            "flexGrow" : 1.0,
            "flexShrink" : 1.0,
            "alignItems" : "Stretch",
            "padding" : {"all" : 20}
        });

        _listViewController->view()->visible.onChange() += [&](auto &p) {
            if (p.get()) {
                _listViewController->deselect();
            }
        };

        navigationView->pushView(_listViewController->view(), "Reddit");

        _listViewController->onClicked() += [navigationView](auto title, auto url, auto imageUrl) {
            auto post = std::make_shared<PostDetailController>(title, url, imageUrl);
            navigationView->pushView(post->view(), "Details");
        };

        _window->contentView = navigationView;
        _window->visible = true;
    }

  protected:
    std::shared_ptr<ui::Window> _window;
    std::shared_ptr<PostListViewController> _listViewController;
};

class RedditApplicationController : public ui::UIApplicationController
{
  public:
    void beginLaunch() override { _mainViewController = std::make_shared<MainViewController>(); }

  protected:
    std::shared_ptr<MainViewController> _mainViewController;
};

BDN_APP_INIT(RedditApplicationController)
