#include <stdio.h>
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include "nanovg.h"
#define NANOVG_GLES2_IMPLEMENTATION	// Use GL2 implementation.
#include "nanovg_gl.h"

#include "test.hpp"
#include <test.grpc.pb.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/health_check_service_interface.h>
#include <chrono>

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
    std::string server_address("0.0.0.0:50051");
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
    NVGcontext* vg = nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES);

    //start grpc service
    TestImpl service(vg);
    grpc::EnableDefaultHealthCheckService(true);
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
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
        service.Render(vg, frameTime);
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
