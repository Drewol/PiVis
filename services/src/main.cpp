#include <stdio.h>
#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#include <GL/glew.h>
#include <GL/wglew.h>
#else
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#define GLFW_INCLUDE_ES2
#endif
#include <GLFW/glfw3.h>
#include "nanovg.h"
#define NANOVG_GLES2_IMPLEMENTATION	// Use GL2 implementation.
#include "nanovg_gl.h"

#include "BaseService.hpp"
#include "ServiceManager.hpp"
#include "pivis.grpc.pb.h"
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/health_check_service_interface.h>

#include <chrono>
#include <map>

using pivis::Empty;
using pivis::ServiceList;
using pivis::ServiceSelection;

class PiVisService : public pivis::PiVis::Service {
public:
    BaseService* currentService = nullptr;
    PiVisService() = default;
    grpc::Status GetServices(grpc::ServerContext* context, const Empty* request, ServiceList* response) override {
        int i = 0;
        for (auto& s : ServiceManager::Get()->GetServices())
        {
            response->add_services(s.first);
        }
        return grpc::Status::OK;
    }
    grpc::Status SetService(grpc::ServerContext* context, const ServiceSelection* request, Empty* response) override {
        std::string newService = request->service();
        auto serviceMap = ServiceManager::Get()->GetServices();
        if (serviceMap.find(newService.c_str()) != serviceMap.end())
        {
            currentService = serviceMap.at(newService);
        }
        else {
            return grpc::Status::Status(grpc::StatusCode::OUT_OF_RANGE, "No service \"" + newService + "\" exists.");
        }

        return grpc::Status::OK;
    }
};

void errorcb(int error, const char* desc)
{
	printf("GLFW error %d: %s\n", error, desc);
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0,0, width, height);
}

int main(int argc, char const *argv[])
{    
    std::string server_address("127.0.0.1:50051");
    GLFWwindow* window;

	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		return -1;
	}

    /* Create a windowed mode window and its OpenGL context */
    glfwSetErrorCallback(errorcb);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

#ifdef WIN32
    GLint GlewInitResult = glewInit();
    if (GLEW_OK != GlewInitResult)
    {
        printf("ERROR: %s", glewGetErrorString(GlewInitResult));
        exit(EXIT_FAILURE);
    }
#endif // WIN32

    NVGcontext* vg = nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES);

    //start grpc service
    grpc::EnableDefaultHealthCheckService(true);
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    PiVisService pvs;
    builder.RegisterService(&pvs);

    for (auto& s : ServiceManager::Get()->GetServices())
    {
        s.second->Init(vg);
        s.second->Register(builder);
        pvs.currentService = s.second;
    }

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    /* Loop until the user closes the window */
    float frameTime = 0.16;
    std::chrono::high_resolution_clock frameTimer;
    auto lastFrameStart = frameTimer.now();
    glfwSetWindowSizeCallback(window, window_size_callback);
    while (!glfwWindowShouldClose(window))
    {
        frameTime = std::chrono::duration<float>(frameTimer.now() - lastFrameStart).count();
        lastFrameStart = frameTimer.now();
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        int w,h;
        glfwGetWindowSize(window, &w, &h);
        
        nvgBeginFrame(vg, w, h, 1.0);
        pvs.currentService->Render(vg, frameTime);
        nvgEndFrame(vg);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }


    server->Shutdown();
    server->Wait();

    return 0;
}
