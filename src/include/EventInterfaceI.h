#pragma once
#include "AuthInterfaceI.h"
#include "DboTables.h"
#include <comm/EventInterface.h>


#include <Wt/Dbo/Session.h>
#include <Wt/Dbo/ptr.h>
#include <Wt/Dbo/SqlConnection.h>
#include <Wt/Dbo/Exception.h>

#include <Wt/WDateTime.h>

#include <Ice/Ice.h>
#include <Wt/Dbo/FixedSqlConnectionPool.h>

using namespace Emlab;

class EventInterfaceI : public EventInterface
{
public:
	EventInterfaceI(std::unique_ptr<dbo::SqlConnection> conn, std::shared_ptr<AuthInterfaceI> authInterface);

	virtual int addEventInfo(std::string userToken, EventInfo eventInfo , const Ice::Current &) override;
	virtual int addEventData(std::string userToken, EventData eventData, const Ice::Current &) override;
	virtual void delEvent(std::string userToken, int eventId, const Ice::Current &) override;

	virtual void setEventDateTime(std::string userToken, int eventId, std::string newDateTime, const Ice::Current &) override;
	virtual void setEventDate(std::string userToken, int eventId, std::string newDate, const Ice::Current &) override;
	virtual void setEventTime(std::string userToken, int eventId, std::string newTime, const Ice::Current &) override;
	virtual void setClient(std::string userToken, int eventId, int clientId, const Ice::Current &) override;
	virtual void setDuration(std::string userToken, int eventId, int duration, const Ice::Current &) override;
	virtual void setLocation(std::string userToken, int eventId, std::string location, const Ice::Current &) override;
	virtual void setDescription(std::string userToken, int eventId, std::string description, const Ice::Current &) override;

	virtual SeqEventData getSeqEventData(std::string userToken, const Ice::Current &) override;

	virtual EventData getEventData(std::string userToken, int eventId, const Ice::Current &) override;
	virtual EventInfo getEventInfo(std::string userToken, int eventId, const Ice::Current &) override;
	// virtual SeqClientInfo getClientsByName(string userToken, string partialName, const Ice::Current &) override;
	// virtual SeqClientInfo getClientsByPhone(string userToken, string partialPhone, const Ice::Current &) override;

	// virtual int registerClient(string userToken, ClientInfo clientInfo, const Ice::Current &) override;
	// virtual void modifyClient(string userToken, ClientInfo clientInfo, const Ice::Current &) override;
private:
	Wt::Dbo::Session session_;
	std::shared_ptr<AuthInterfaceI> authInterface_;
};
