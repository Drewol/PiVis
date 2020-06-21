#include "test.hpp"
#include <test.grpc.pb.h>
#include "../../nanovg/src/nanovg.h"

using namespace test;
using namespace grpc;

TestImpl::TestImpl(NVGcontext* vg)
{
    font = nvgCreateFont(vg, "default", "fonts/Roboto/Roboto-Regular.ttf");
    currentTab = TabIndex::HOME;
}

Status TestImpl::SetTab(ServerContext* context, const ChangeTab* tab, EmptyResult* response) {
    printf("Changing %d->%d\n", currentTab, tab->tab());
    currentTab = tab->tab();
    return Status::OK;
}

void TestImpl::Render(NVGcontext* vg, float deltaTime)
{
    std::string fpsText;
    char buff[64];
    avgFps = avgFps * 0.98f + deltaTime * 0.02f;
    snprintf(buff , 32, "%.2f FPS", 1.0 / avgFps);
    fpsText = buff;
    nvgBeginPath(vg);
    nvgFillColor(vg, nvgRGB(255,255,255));
    nvgFontFaceId(vg, font);
    nvgFontSize(vg, 200);
    nvgTextAlign(vg, NVG_ALIGN_TOP | NVG_ALIGN_LEFT);
    nvgText(vg, 50, 50, fpsText.c_str(), nullptr);
    switch (currentTab)
    {
    case TabIndex::HOME:
        fpsText = "Tab: Home";
        break;
        case TabIndex::DATA:
        fpsText = "Tab: Data";
        break;
        case TabIndex::PERF:
        fpsText = "Tab: Performace";
        break;
    
    default:
        break;
    }
    nvgText(vg, 50, 300, fpsText.c_str(), nullptr);
}

Status TestImpl::GetTab(ServerContext* context, const EmptyResult* tab, ChangeTab* response) {
    response->set_tab(currentTab);
    return Status::OK;
}
