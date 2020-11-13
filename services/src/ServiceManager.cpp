#include "ServiceManager.hpp"
#include "BaseService.hpp"

//pure virtual destructor needs implementation so it'll just be here...
BaseService::~BaseService() {} 

std::unique_ptr<ServiceManager> ServiceManager::instance;


ServiceManager::~ServiceManager() {
    for (auto s : m_services)
    {
        if (s.second)
            delete s.second;
    }
    m_services.clear();
}

ServiceManager* ServiceManager::Get() {
    if (!instance)
    {
        instance = std::make_unique<ServiceManager>();
    }
    return instance.get();
}

void ServiceManager::AddService(std::string name, BaseService* service) {
    m_services.insert(std::make_pair(name, service));
}

const std::map<std::string, class BaseService*>& ServiceManager::GetServices() {
    return m_services;
}