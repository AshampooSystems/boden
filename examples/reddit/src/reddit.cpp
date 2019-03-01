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
                 try {
                     json j = json::parse(r->data);

                     for (auto child : j["data"]["children"]) {
                         auto post = std::make_shared<RedditPost>();
                         post->title = child["data"]["title"];
                         post->url = child["data"]["url"];
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

      public:
        using ContainerView::ContainerView;
        void build()
        {
            setLayoutStylesheet(FlexDirection(FlexStylesheet::Direction::Row)
                                << FlexJustifyContent(FlexStylesheet::Justify::SpaceBetween)
                                << FlexAlignItems(FlexStylesheet::Align::Center));

            auto textView = std::make_shared<TextView>();
            textView->setLayoutStylesheet((FlexStylesheet)FlexGrow(1.0f));

            addChildView(textView);

            textView->text.bind(text);
            textView->wrap = false;

            auto button = std::make_shared<Button>();
            button->label = "Hello";
            button->setLayoutStylesheet(FlexGrow(0.0f) << FlexShrink(0.0) << FlexMarginAll(5.0));

            addChildView(button);
        }
    };

  public:
    RedditListViewDataSource(std::shared_ptr<RedditStore> store) { _store = store; }

    size_t numberOfRows() override { return _store->posts.size(); }

    String labelTextForRowIndex(size_t rowIndex) override { return _store->posts.at(rowIndex)->title; }

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
        return delegate;
    }

  private:
    std::shared_ptr<RedditStore> _store;
};

class PostListViewController : public Base
{
  public:
    using clickNotifier_t = SimpleNotifier<String, String>;

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
    PostDetailController(String title, String url) : _mainColumn(std::make_shared<ContainerView>())
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

        _listViewController->onClicked() += [stack](auto title, auto url) {
            auto post = std::make_shared<PostDetailController>(title, url);
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
