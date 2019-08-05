#include <gtest/gtest.h>

#include <bdn/Application.h>
#include <bdn/appInit.h>
#include <bdn/log.h>
#include <bdn/ui/ContainerView.h>
#include <bdn/ui/Label.h>
#include <bdn/ui/UIApplicationController.h>
#include <bdn/ui/Window.h>
#include <bdn/ui/yoga.h>

#include <sstream>
#include <thread>

using namespace std::chrono_literals;

class TestApplicationController : public bdn::ui::UIApplicationController, public testing::EmptyTestEventListener
{
  public:
    ~TestApplicationController() override
    {
        _testThread->join();
        _testThread.reset();
    }

    void createTestUI()
    {
        _window = std::make_shared<bdn::ui::Window>();
        _window->geometry = {0, 0, 300, 400};
        _window->setLayout(std::make_shared<bdn::ui::yoga::Layout>());
        _window->stylesheet = FlexJsonStringify({"justifyContent" : "Center", "alignItems" : "Center"});

        _progressLabel = std::make_shared<bdn::ui::Label>();
        _testNameLabel = std::make_shared<bdn::ui::Label>();

        auto container = std::make_shared<bdn::ui::ContainerView>();
        container->stylesheet = FlexJsonStringify({"justifyContent" : "Center", "alignItems" : "Center"});

        container->addChildView(_progressLabel);
        container->addChildView(_testNameLabel);

        _window->contentView = container;

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
        bool hasFailed = false;
        if (testinfo) {
            ssTestInfo << testinfo->test_suite_name() << "." << testinfo->name();

            if (testinfo->result()) {
                if (testinfo->result()->Failed()) {
                    hasFailed = true;
                }
            }
        }

        std::string sProgress = ssProgress.str();
        std::string sTestInfo = ssTestInfo.str();
        bdn::App()->dispatchQueue()->dispatchAsync([this, sProgress, sTestInfo, hasFailed]() {
            if (_progressLabel)
                _progressLabel->text = sProgress;
            if (_testNameLabel)
                _testNameLabel->text = sTestInfo;
            if (hasFailed) {
                _window->backgroundColor = bdn::Color(1, 0, 0, 1);
            }
        });
    }

    void beginLaunch() override
    {
        bdn::logstream() << "Arguments:";
        for (auto &s : bdn::App()->commandLineArguments.get()) {
            bdn::logstream() << s;
        }
        bdn::logstream() << "---------------";

        int argc = bdn::App()->argc();
        testing::InitGoogleTest(&argc, bdn::App()->argv());

        testing::UnitTest::GetInstance()->listeners().Append(this);

        createTestUI();

        _testThread = std::make_unique<std::thread>(&RUN_ALL_TESTS);
    }

    void exit(int code)
    {
        _testThread->join();
        _testThread.reset();

        std::exit(code);
    }

  public:
    void OnTestSuiteStart(const testing::TestSuite &suite) override { updateProgressLabel(); }
    void OnTestStart(const testing::TestInfo &info) override
    {
        nCasesRun++;
        updateProgressLabel();
    }

    void OnTestEnd(const testing::TestInfo & /*testInfo*/) override { updateProgressLabel(); }
    void OnTestProgramEnd(const testing::UnitTest &unit_test) override
    {
        testing::UnitTest::GetInstance()->listeners().Release(this);

        bdn::App()->dispatchQueue()->dispatchAsync([]() {
            ((TestApplicationController *)bdn::App()->applicationController().get())
                ->exit(testing::UnitTest::GetInstance()->failed_test_count() > 0 ? 1 : 0);
        });
    }

  private:
    std::shared_ptr<bdn::ui::Window> _window;
    std::shared_ptr<bdn::ui::Label> _progressLabel;
    std::shared_ptr<bdn::ui::Label> _testNameLabel;
    std::unique_ptr<std::thread> _testThread;

    int nCasesRun = 0;
};

BDN_APP_INIT(TestApplicationController)
