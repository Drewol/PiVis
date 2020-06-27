#pragma once
#ifdef WIN32
#include <winsdkver.h>
#endif
#include <test.grpc.pb.h>
#include "BaseServer.hpp"
using namespace test;
using namespace grpc;
class TestImpl final : public TestService::Service, public BaseServer
{
public:
    TestImpl();
    Status SetTab(ServerContext *context, const ChangeTab *tab, EmptyResult *response) override;
    Status GetTab(ServerContext *context, const EmptyResult *tab, ChangeTab *response) override;
    Status SetMessage(ServerContext *context, const StringRequest *request, EmptyResult *response) override;
    Status SetHue(ServerContext *context, const HueRequest *hue, EmptyResult *response) override;
    void Render(struct NVGcontext *vg, float deltaTime) override;
    void Init(struct NVGcontext* vg) override;
    void Register(grpc::ServerBuilder& builder) override;

private:
    test::TabIndex currentTab;
    int font;
    float avgFps = 0.016f;
    double hue = 0.0f;
    std::string message;
};
