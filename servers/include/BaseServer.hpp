#pragma once
#include <grpc/grpc.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/server_builder.h>

#include <nanovg.h>
#include <map>
#include <string>

class BaseServer
{
    public: 
    virtual void Render(struct NVGcontext* vg, float deltaTime) = 0;
    virtual void Init(struct NVGcontext* vg) = 0;
    virtual void Register(grpc::ServerBuilder& builder) = 0;
};

extern class std::map<std::string, BaseServer*>* g_Services;

template<class T>
struct StaticReg {
    const char* name;

    StaticReg(std::string name, T* service) {
        g_Services->insert(std::make_pair(name, service));
    };
};