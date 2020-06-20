#include <test.grpc.pb.h>
#include "BaseServer.hpp"
class TestImpl final : public test::TestService::Service, public BaseServer {
public:
    TestImpl(struct NVGcontext* vg);
    test::EmptyResult SetTab(test::ChangeTab changeTab);
    test::ChangeTab GetTab(test::EmptyResult);
    void Render(struct NVGcontext* vg, float deltaTime) override;
private:
    test::TabIndex currentTab;
    int font;
    float avgFps = 0.016f;
};