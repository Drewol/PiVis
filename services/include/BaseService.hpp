#pragma once
#include <grpc/grpc.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/server_builder.h>

#include <nanovg.h>
#include <map>
#include <string>

#include "ServiceManager.hpp"

class BaseService
{
    public: 
    virtual ~BaseService() = 0;
    virtual void Render(struct NVGcontext* vg, float deltaTime) = 0;
    virtual void Init(struct NVGcontext* vg) = 0;
    virtual void Register(grpc::ServerBuilder& builder) = 0;
};

template<class T>
class ServiceEntry
{
public:
    ServiceEntry(std::string name) {
        ServiceManager::Get()->AddService(name, new T());
    }
};

#define REGISTER_SERVICE(name, service) static ServiceEntry<service> service_obj(name)