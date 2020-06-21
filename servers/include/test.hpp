#include <test.grpc.pb.h>
#include "BaseServer.hpp"
using namespace test;
using namespace grpc;
class TestImpl final : public TestService::Service, public BaseServer {
public:
    TestImpl(struct NVGcontext* vg);
    Status SetTab(ServerContext* context, const ChangeTab* tab, EmptyResult* response) override;
    Status GetTab(ServerContext* context, const EmptyResult* tab, ChangeTab* response) override;
    void Render(struct NVGcontext* vg, float deltaTime) override;
private:
    test::TabIndex currentTab;
    int font;
    float avgFps = 0.016f;
};
