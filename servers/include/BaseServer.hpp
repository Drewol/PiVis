#include <nanovg.h>

class BaseServer
{
    public: 
    virtual void Render(struct NVGcontext* vg, float deltaTime) = 0;
};