#include "include/ServiceInterfaceI.h"

#include <Wt/Dbo/ptr.h>
#include <Wt/Dbo/SqlConnection.h>
#include <Wt/Dbo/Exception.h>

#include <Wt/WDateTime.h>

#include <Ice/Ice.h>
#include <Wt/Dbo/FixedSqlConnectionPool.h>

ServiceInterfaceI::ServiceInterfaceI(std::unique_ptr<dbo::SqlConnection> conn, std::shared_ptr<AuthInterfaceI> authInterface)
	: authInterface_(authInterface)
{
	session_.setConnection(std::move(conn));
	session_.mapClass<User>("user");

	session_.mapClass<ProviderProfile>("provider_profile");
	session_.mapClass<ProfileService>("profile_service");
	session_.mapClass<ProfileGalery>("profile_galery");
	session_.mapClass<ServiceAgeGroup>("service_age_group");

	session_.mapClass<Event>("event");
	session_.mapClass<EventService>("event_service");
	

	session_.mapClass<Review>("review");
	session_.mapClass<ReviewGalery>("review_galery");
}


int ServiceInterfaceI::addServiceToEvent(std::string userToken, Emlab::ServiceInfo serviceInfo, const Ice::Current &)
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
