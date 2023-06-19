#include "include/ServiceInterfaceI.h"

ServiceInterfaceI::ServiceInterfaceI(std::unique_ptr<dbo::SqlConnection> conn, std::shared_ptr<AuthInterfaceI> authInterface)
	: authInterface_(authInterface)
{
	session_.setConnection(std::move(conn));
	session_.mapClass<ProviderService>("provider_service");
	session_.mapClass<Client>("client");
	session_.mapClass<Service>("service");
	session_.mapClass<Event>("event");
	session_.mapClass<User>("user");
	session_.mapClass<UserRole>("user_role");
	session_.mapClass<Review>("review");

}


int ServiceInterfaceI::addServiceToEvent(std::string userToken, ServiceInfo serviceInfo, const Ice::Current &)
{
	int id = 0;
	std::cout << "\n\n addServiceToEvent \n\n";
	return id;
}

void ServiceInterfaceI::delService(std::string userToken, std::string serviceId, const Ice::Current &)
{
	std::cout << "\n\n delService \n\n";
}


// void ServiceInterfaceI::setService(, const Ice::Current &)
// {
// 	std::cout << "\n\n setService \n\n";
// }

// void ServiceInterfaceI::setServiceCost(std::string userToken, std::string serviceId, int cost, const Ice::Current &)
// {
// 	std::cout << "\n\n setServiceCost \n\n";
// }

// void ServiceInterfaceI::setServiceDescription(std::string userToken, std::string serviceId, std::string description, const Ice::Current &)
// {
// 	std::cout << "\n\n setServiceDescription \n\n";
// }


void ServiceInterfaceI::setServiceDate(std::string userToken, std::string serviceId, std::string date, const Ice::Current &)
{
	std::cout << "\n\n setServiceDate \n\n";
}

void ServiceInterfaceI::setServiceTime(std::string userToken, std::string serviceId, std::string time, const Ice::Current &)
{
	std::cout << "\n\n setServiceTime \n\n";
}

void ServiceInterfaceI::setServiceDuration(std::string userToken, std::string serviceId, int duration, const Ice::Current &)
{
	std::cout << "\n\n setServiceDuration \n\n";
}

void ServiceInterfaceI::setServiceObservations(std::string userToken, std::string serviceId, std::string observations, const Ice::Current &)
{
	std::cout << "\n\n setServiceObservations \n\n";
}
