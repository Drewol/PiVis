#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#endif
#include "test.hpp"
#include <test.grpc.pb.h>
#include "../../nanovg/src/nanovg.h"

using namespace test;
using namespace grpc;

TestImpl::TestImpl()
{
    font = -1;
    currentTab = TabIndex::HOME;
    message = "";
    g_Services->insert(std::make_pair("test", this));
}

TestImpl* instance = new TestImpl();

void TestImpl::Init(NVGcontext *vg)
{
    font = nvgCreateFont(vg, "default", "fonts/Roboto/Roboto-Regular.ttf");
    currentTab = TabIndex::HOME;
    message = "";
}

void TestImpl::Register(grpc::ServerBuilder& builder)
{
    builder.RegisterService(this);
}

Status TestImpl::SetTab(ServerContext *context, const ChangeTab *tab, EmptyResult *response)
{
    printf("Changing %d->%d\n", currentTab, tab->tab());
    currentTab = tab->tab();
    return Status::OK;
}

Status TestImpl::SetMessage(ServerContext *context, const StringRequest *request, EmptyResult *response)
{
    message = request->text();
    return Status::OK;
}

Status TestImpl::GetTab(ServerContext *context, const EmptyResult *tab, ChangeTab *response)
{
    response->set_tab(currentTab);
    return Status::OK;
}

Status TestImpl::SetHue(ServerContext *context, const HueRequest *hue, EmptyResult *response)
{
    this->hue = hue->hue();
    printf("Changing hue to %.2f\n", this->hue);
    return Status::OK;
}

void TestImpl::Render(NVGcontext *vg, float deltaTime)
{
    std::string text, title;
    char buff[64];
    avgFps = avgFps * 0.98f + deltaTime * 0.02f;
    snprintf(buff, 32, "%.2f FPS", 1.0 / avgFps);
    nvgBeginPath(vg);
    nvgFillColor(vg, nvgHSL(static_cast<float>(hue), 1.0f, 0.5f));
    nvgFontFaceId(vg, font);
    nvgFontSize(vg, 40);
    nvgTextAlign(vg, NVG_ALIGN_TOP | NVG_ALIGN_LEFT);
    switch (currentTab)
    {
    case TabIndex::HOME:
        title = "Tab: Home";
        text = "Welcome to PiVis!";
        break;
    case TabIndex::DATA:
        title = "Tab: Data";
        text = message;
        break;
    case TabIndex::PERF:
        title = "Tab: Performace";
        text = buff;
        break;

    default:
        break;
    }
    nvgText(vg, 50, 10, title.c_str(), nullptr);
    nvgText(vg, 50, 40, text.c_str(), nullptr);
}
