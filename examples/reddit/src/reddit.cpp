#include <bdn/foundation.h>
#include <bdn/log.h>
#include <bdn/net.h>
#include <bdn/ui.h>

#include <nlohmann/json.hpp>

#include <bdn/AppRunnerBase.h>

#include <iostream>

using namespace bdn;

using namespace nlohmann;

class RedditPost
{
  public:
    RedditPost() = default;
    RedditPost(String title_, String url_)
    {
        title = title_;
        url = url_;
    }

    Property<String> title;
    Property<String> url;
};

class RedditStore
{
  public:
    void fetchPosts(std::function<void()> doneHandler)
    {
        auto response = net::http::request(
            {bdn::net::http::Method::GET, "https://www.reddit.com/hot.json?limit=100", [this, doneHandler](auto r) {
                 json j = json::parse(r->data);

                 for (auto child : j["data"]["children"]) {
                     auto post = std::make_shared<RedditPost>();
                     post->title = child["data"]["title"];
                     post->url = child["data"]["url"];
                     posts.push_back(post);
                 }

                 doneHandler();
             }});
    }

    std::vector<std::shared_ptr<RedditPost>> posts;
};

class RedditListViewDataSource : public ListViewDataSource
{
  public:
    RedditListViewDataSource(std::shared_ptr<RedditStore> store) { _store = store; }

    size_t numberOfRows() override { return _store->posts.size(); }

    String labelTextForRowIndex(size_t rowIndex) override { return _store->posts.at(rowIndex)->title; }

  private:
    std::shared_ptr<RedditStore> _store;
};

class PostListViewController : public Base
{
  public:
    using clickNotifier_t = SimpleNotifier<String, String>;

    PostListViewController() : _listView(std::make_shared<ListView>())
    {
        auto store = std::make_shared<RedditStore>();
        _dataSource = std::make_shared<RedditListViewDataSource>(store);

        _listView->dataSource = _dataSource;

        store->fetchPosts([this]() { _listView->reloadData(); });

        _listView->horizontalAlignment = View::HorizontalAlignment::expand;
        _listView->verticalAlignment = View::VerticalAlignment::expand;
        _listView->preferredSizeMinimum = Size(100, 200);
        _listView->margin = UIMargin(15, 15, 15, 15);

        _listView->selectedRowIndex.onChange() += [store, this](auto indexAccessor) {
            auto post = store->posts.at(*indexAccessor->get());
            _onClicked.notify(post->title, post->url);
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
    PostDetailController(String title, String url) : _mainColumn(std::make_shared<ColumnView>())
    {
        auto titleField = std::make_shared<TextView>();
        auto urlField = std::make_shared<TextView>();
        auto openButton = std::make_shared<Button>();

        titleField->text = "Title: " + title;
        urlField->text = "URL: " + url;

        openButton->label = "Open in Browser";
        openButton->onClick() += [url](auto) { getAppRunner()->openURL(url); };

        _mainColumn->addChildView(titleField);
        _mainColumn->addChildView(urlField);
        _mainColumn->addChildView(openButton);
    }

    std::shared_ptr<View> view() { return _mainColumn; }

  private:
    std::shared_ptr<ColumnView> _mainColumn;
};

class MainViewController : public Base
{
  public:
    MainViewController() : _listViewController(std::make_shared<PostListViewController>())
    {
        _window = std::make_shared<Window>();
        _window->title = "UI Demo";

        auto stack = std::make_shared<Stack>();
        stack->preferredSizeMinimum = Size(250, 0);

        stack->pushView(_listViewController->view(), "Reddit");

        _listViewController->onClicked() += [stack](auto title, auto url) {
            auto post = std::make_shared<PostDetailController>(title, url);
            stack->pushView(post->view(), "Details");
        };

        _window->setContentView(stack);

        _window->requestAutoSize();
        _window->requestCenter();

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
