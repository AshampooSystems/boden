#include <gtest/gtest.h>

#include <bdn/Application.h>
#include <bdn/ContainerView.h>
#include <bdn/Label.h>
#include <bdn/Window.h>
#include <bdn/appInit.h>
#include <bdn/log.h>
#include <bdn/yogalayout.h>

#include <sstream>
#include <thread>

using namespace std::chrono_literals;

class TestApplicationController : public bdn::ApplicationController, public testing::EmptyTestEventListener
{
  public:
    ~TestApplicationController() override
    {
        _testThread->join();
        _testThread.reset();
    }

    void createTestUI()
    {
        _window = std::make_shared<bdn::Window>();
        _window->geometry = {0, 0, 300, 400};
        _window->setLayout(std::make_shared<bdn::yogalayout::Layout>());
        _window->setLayoutStylesheet(FlexJsonStringify({"justifyContent" : "Center", "alignItems" : "Center"}));

        _progressLabel = std::make_shared<bdn::Label>();
        _testNameLabel = std::make_shared<bdn::Label>();

        auto container = std::make_shared<bdn::ContainerView>();
        container->setLayoutStylesheet(FlexJsonStringify({"justifyContent" : "Center", "alignItems" : "Center"}));

        container->addChildView(_progressLabel);
        container->addChildView(_testNameLabel);

        _window->content = container;

        updateProgressLabel();

        _window->visible = true;
    }

    void updateProgressLabel()
    {
        std::ostringstream ssProgress;

        auto testinfo = ::testing::UnitTest::GetInstance()->current_test_info();
        int nRun = nCasesRun;

        ssProgress << "Running test " << nRun << " of " << ::testing::UnitTest::GetInstance()->total_test_count();
        std::ostringstream ssTestInfo;

        if (testinfo) {
            ssTestInfo << testinfo->test_suite_name() << "." << testinfo->name();
        }

        bdn::String sProgress = ssProgress.str();
        bdn::String sTestInfo = ssTestInfo.str();
        bdn::App()->dispatcher()->enqueue([this, sProgress, sTestInfo]() {
            _progressLabel->text = sProgress;
            _testNameLabel->text = sTestInfo;
        });
    }

    void beginLaunch() override
    {
        int argc = bdn::App()->argc();
        testing::InitGoogleTest(&argc, bdn::App()->argv());

        testing::UnitTest::GetInstance()->listeners().Append(this);

        createTestUI();

        _testThread = std::make_unique<std::thread>(&RUN_ALL_TESTS);
    }

  public:
    void OnTestSuiteStart(const testing::TestSuite &suite) override { updateProgressLabel(); }
    void OnTestStart(const testing::TestInfo &info) override
    {
        nCasesRun++;
        updateProgressLabel();
    }
    void OnTestEnd(const testing::TestInfo &) override { updateProgressLabel(); }
    void OnTestProgramEnd(const testing::UnitTest &unit_test) override
    {
        testing::UnitTest::GetInstance()->listeners().Release(this);

        bdn::App()->dispatcher()->enqueue(
            []() { std::exit(testing::UnitTest::GetInstance()->failed_test_count() > 0 ? 1 : 0); });
    }

  private:
    std::shared_ptr<bdn::Window> _window;
    std::shared_ptr<bdn::Label> _progressLabel;
    std::shared_ptr<bdn::Label> _testNameLabel;
    std::unique_ptr<std::thread> _testThread;

    int nCasesRun = 0;
};

BDN_APP_INIT(TestApplicationController)
