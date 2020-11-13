#pragma once
#include <map>
#include <string>
#include <memory>

class ServiceManager {
public:
	static ServiceManager* Get();
	~ServiceManager();
	const std::map<std::string, class BaseService*>& GetServices();
	void AddService(std::string name, class BaseService* service);
private:
	static std::unique_ptr<ServiceManager> instance;
	std::map<std::string, class BaseService*> m_services;
};