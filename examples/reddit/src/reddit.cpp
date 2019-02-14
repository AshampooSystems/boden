#include <bdn/foundation.h>
#include <bdn/ui.h>
#include <bdn/net.h>
#include <bdn/log.h>

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

class MainViewController : public Base
{
  public:
    MainViewController()
    {
        _window = std::make_shared<Window>();
        _window->title = "UI Demo";

        auto mainColumn = std::make_shared<ColumnView>();
        mainColumn->preferredSizeMinimum = Size(250, 0);

        auto store = std::make_shared<RedditStore>();
        _dataSource = std::make_shared<RedditListViewDataSource>(store);

        auto listView = std::make_shared<ListView>();
        listView->dataSource = _dataSource;

        store->fetchPosts([listView]() { listView->reloadData(); });

        listView->horizontalAlignment = View::HorizontalAlignment::expand;
        listView->verticalAlignment = View::VerticalAlignment::expand;
        listView->preferredSizeMinimum = Size(100, 200);
        listView->margin = UIMargin(15, 15, 15, 15);

        listView->selectedRowIndex.onChange() += [listView, store](auto) {
            size_t index = *listView->selectedRowIndex.get();
            logInfo("selectedRowIndex changed: " + std::to_string(index));
            getAppRunner()->openURL(store->posts.at(index)->url);
        };

        mainColumn->addChildView(listView);

        _window->setContentView(mainColumn);

        _window->requestAutoSize();
        _window->requestCenter();

        _window->visible = true;
    }

  protected:
    std::shared_ptr<Window> _window;
    std::shared_ptr<RedditListViewDataSource> _dataSource;
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
