#pragma once
#include "AuthInterfaceI.h"
#include "DboTables.h"
#include "../../../comunication/comm/ServiceInterface.h"


#include <Wt/Dbo/Session.h>
#include <Wt/Dbo/ptr.h>
#include <Wt/Dbo/SqlConnection.h>
#include <Wt/Dbo/Exception.h>

#include <Wt/WDateTime.h>

#include <Ice/Ice.h>
#include <Wt/Dbo/FixedSqlConnectionPool.h>

class ServiceInterfaceI : public ServiceInterface
{
public:
	ServiceInterfaceI(std::unique_ptr<dbo::SqlConnection> conn, std::shared_ptr<AuthInterfaceI> authInterface);

		virtual int addServiceToEvent(std::string userToken, ServiceInfo serviceInfo, const Ice::Current &) override;
        virtual void delService(std::string userToken, std::string serviceId, const Ice::Current &) override;

        // virtual void setService(, const Ice::Current &) override;
        // virtual void setServiceCost(std::string userToken, std::string serviceId, int cost, const Ice::Current &) override;
        // virtual void setServiceDescription(std::string userToken, std::string serviceId, std::string description, const Ice::Current &) override;

        virtual void setServiceDate(std::string userToken, std::string serviceId, std::string date, const Ice::Current &) override;
        virtual void setServiceTime(std::string userToken, std::string serviceId, std::string time, const Ice::Current &) override;
        virtual void setServiceDuration(std::string userToken, std::string serviceId, int duration, const Ice::Current &) override;
        virtual void setServiceObservations(std::string userToken, std::string serviceId, std::string observations, const Ice::Current &) override;

private:
	Wt::Dbo::Session session_;
	std::shared_ptr<AuthInterfaceI> authInterface_;
};
