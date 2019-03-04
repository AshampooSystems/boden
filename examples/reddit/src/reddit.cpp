#include <bdn/foundation.h>
#include <bdn/log.h>
#include <bdn/net.h>
#include <bdn/ui.h>
#include <bdn/yogalayout.h>

#include <nlohmann/json.hpp>

#include <bdn/AppRunnerBase.h>

#include <iostream>

using namespace bdn;

using namespace nlohmann;

class RedditPost
{
  public:
    RedditPost() = default;
    RedditPost(String title_, String url_, String thumbnailUrl_)
    {
        title = title_;
        url = url_;
        thumbnailUrl = thumbnailUrl_;
    }

    Property<String> title;
    Property<String> url;
    Property<String> thumbnailUrl;
};

class RedditStore
{
  public:
    void fetchPosts(std::function<void()> doneHandler)
    {
        auto response = net::http::request(
            {bdn::net::http::Method::GET, "https://www.reddit.com/hot.json?limit=100", [this, doneHandler](auto r) {
                 try {
                     json j = json::parse(r->data);

                     for (auto child : j["data"]["children"]) {
                         auto post = std::make_shared<RedditPost>();
                         post->title = child["data"]["title"];
                         post->url = child["data"]["url"];

                         String thumbnail = child["data"]["thumbnail"];

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

class RedditListViewDataSource : public ListViewDataSource
{
    class Delegate : public ContainerView
    {
      public:
        Property<String> text;
        Property<String> imageUrl;

      public:
        using ContainerView::ContainerView;
        void build()
        {
            setLayoutStylesheet(FlexDirection(FlexStylesheet::Direction::Row)
                                << FlexJustifyContent(FlexStylesheet::Justify::SpaceBetween)
                                << FlexAlignItems(FlexStylesheet::Align::FlexStart) << FlexPaddingAll(2.5)
                                << FlexGrow(1.0));

            auto image = std::make_shared<ImageView>();
            image->url.bind(imageUrl, BindMode::unidirectional);
            image->setLayoutStylesheet(FlexMaximumSizeWidth(45.0)
                                       << FlexAlignSelf(FlexStylesheet::Align::Center) << FlexMarginRight(5.f));

            addChildView(image);

            auto textView = std::make_shared<TextView>();
            textView->setLayoutStylesheet((FlexStylesheet)FlexGrow(1.0f));

            addChildView(textView);

            textView->text.bind(text);
            textView->wrap = true;
        }
    };

  public:
    RedditListViewDataSource(std::shared_ptr<RedditStore> store) { _store = store; }

    size_t numberOfRows() override { return _store->posts.size(); }

    String labelTextForRowIndex(size_t rowIndex) override { return _store->posts.at(rowIndex)->title; }

    float heightForRowIndex(size_t rowIndex) override { return 50; }

    std::shared_ptr<View> viewForRowIndex(size_t rowIndex, std::shared_ptr<View> reusableView) override
    {
        std::shared_ptr<Delegate> delegate;

        if (reusableView) {
            delegate = std::dynamic_pointer_cast<Delegate>(reusableView);
        } else {
            delegate = std::make_shared<Delegate>();
            delegate->build();
        }

        String text = labelTextForRowIndex(rowIndex);

        delegate->text = text;
        delegate->imageUrl = _store->posts.at(rowIndex)->thumbnailUrl;
        return delegate;
    }

  private:
    std::shared_ptr<RedditStore> _store;
};

class PostListViewController : public Base
{
  public:
    using clickNotifier_t = SimpleNotifier<String, String, String>;

    PostListViewController() : _listView(std::make_shared<ListView>())
    {
        _listView->setLayoutStylesheet(FlexDirection(FlexStylesheet::Direction::Column)
                                       << FlexGrow(1.0f) << FlexShrink(1.0f) << FlexMarginAll(10.0f));

        auto store = std::make_shared<RedditStore>();
        _dataSource = std::make_shared<RedditListViewDataSource>(store);

        _listView->dataSource = _dataSource;

        store->fetchPosts([this]() { _listView->reloadData(); });

        _listView->selectedRowIndex.onChange() += [store, this](auto indexAccessor) {
            auto post = store->posts.at(*indexAccessor->get());
            _onClicked.notify(post->title, post->url, post->thumbnailUrl);
        };
    }

    std::shared_ptr<View> view() { return _listView; }

    clickNotifier_t &onClicked() { return _onClicked; }

  private:
    std::shared_ptr<RedditListViewDataSource> _dataSource;

    std::shared_ptr<ListView> _listView;

    clickNotifier_t _onClicked;
};

class PostDetailController : public Base
{
  public:
    PostDetailController(String title, String url, String imageUrl) : _mainColumn(std::make_shared<ContainerView>())
    {
        _mainColumn->setLayoutStylesheet(Flex() << FlexGrow(1.0f));

        auto headerColumn = std::make_shared<ContainerView>();

        headerColumn->setLayoutStylesheet(FlexDirection(FlexStylesheet::Direction::Row)
                                          << FlexAlignItems(FlexStylesheet::Align::FlexStart) << FlexMarginBottom(10.0f)
                                          << FlexWrap(FlexStylesheet::Wrap::Wrap));

        auto image = std::make_shared<ImageView>();
        auto titleField = std::make_shared<TextView>();

        image->setLayoutStylesheet(FlexGrow(0.0f) << FlexMaximumSizeWidth(100.0f) << FlexMaximumSizeHeight(100.0f)
                                                  << FlexShrink(0.0f) << FlexMarginRight(5.f));

        titleField->setLayoutStylesheet(FlexGrow(1.0f) << FlexShrink(1.0f) << FlexSizeHeight(100.));
        headerColumn->addChildView(image);
        headerColumn->addChildView(titleField);

        auto webView = std::make_shared<WebView>();
        auto openButton = std::make_shared<Button>();

        webView->url = url;
        webView->setLayoutStylesheet(Flex() << FlexGrow(1.));

        image->url = imageUrl;
        titleField->text = title;

        openButton->label = "Open in Browser";
        openButton->onClick() += [url](auto) { getAppRunner()->openURL(url); };

        _mainColumn->addChildView(headerColumn);
        _mainColumn->addChildView(webView);
        _mainColumn->addChildView(openButton);
    }

    std::shared_ptr<View> view() { return _mainColumn; }

  private:
    std::shared_ptr<ContainerView> _mainColumn;
};

class MainViewController : public Base
{
  public:
    MainViewController() : _listViewController(std::make_shared<PostListViewController>())
    {
        _window = std::make_shared<Window>();
        _window->title = "UI Demo";
        _window->geometry = Rect{0, 0, 1024, 768};
        _window->setLayout(std::make_shared<yogalayout::Layout>());

        auto stack = std::make_shared<Stack>();
        stack->setLayoutStylesheet(FlexDirection(FlexStylesheet::Direction::Column)
                                   << FlexGrow(1.0f) << FlexShrink(1.0f)
                                   << FlexAlignItems(FlexStylesheet::Align::Stretch) << FlexPaddingAll(20));

        stack->pushView(_listViewController->view(), "Reddit");

        _listViewController->onClicked() += [stack](auto title, auto url, auto imageUrl) {
            auto post = std::make_shared<PostDetailController>(title, url, imageUrl);
            stack->pushView(post->view(), "Details");
        };

        _window->content = stack;
        _window->visible = true;
    }

  protected:
    std::shared_ptr<Window> _window;
    std::shared_ptr<PostListViewController> _listViewController;
};

class AppController : public UIAppControllerBase
{
  public:
    void beginLaunch(const AppLaunchInfo &launchInfo) override
    {
        _mainViewController = std::make_shared<MainViewController>();
    }

  protected:
    std::shared_ptr<MainViewController> _mainViewController;
};

BDN_APP_INIT(AppController)
