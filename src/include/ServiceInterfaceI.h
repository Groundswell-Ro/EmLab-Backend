#pragma once
#include "AuthInterfaceI.h"
#include "../../../comunication/comm/ServiceInterface.h"
#include <Wt/Dbo/Session.h>


class ServiceInterfaceI : public Emlab::ServiceInterface
{
public:
	ServiceInterfaceI(std::unique_ptr<dbo::SqlConnection> conn, std::shared_ptr<AuthInterfaceI> authInterface);

		virtual int addServiceToEvent(std::string userToken, Emlab::ServiceInfo serviceInfo, const Ice::Current &) override;
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
