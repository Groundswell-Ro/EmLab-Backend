#pragma once
#include "AuthInterfaceI.h"
#include "DboTables.h"
#include "../../../comunication/generated/EventInterface.h"


#include <Wt/Dbo/Session.h>
#include <Wt/Dbo/ptr.h>
#include <Wt/Dbo/SqlConnection.h>
#include <Wt/Dbo/Exception.h>

#include <Wt/WDateTime.h>

#include <Ice/Ice.h>
#include <Wt/Dbo/FixedSqlConnectionPool.h>

using namespace std;

using namespace Emlab;

class EventInterfaceI : public EventInterface
{
public:
	EventInterfaceI(std::unique_ptr<dbo::SqlConnection> conn, std::shared_ptr<AuthInterfaceI> authInterface);

	virtual EventData addEvent(string userToken, EventData EventData, const Ice::Current &) override;
	virtual void delEvent(string userToken, int eventId, const Ice::Current &) override;
	virtual SeqEventData getEventsData(string userToken, const Ice::Current &) override;

	// virtual SeqClientInfo getClientsByName(string userToken, string partialName, const Ice::Current &) override;
	// virtual SeqClientInfo getClientsByPhone(string userToken, string partialPhone, const Ice::Current &) override;

	// virtual int registerClient(string userToken, ClientInfo clientInfo, const Ice::Current &) override;
	// virtual void modifyClient(string userToken, ClientInfo clientInfo, const Ice::Current &) override;
private:
	mutable Wt::Dbo::Session session_;
	std::shared_ptr<AuthInterfaceI> authInterface_;
};
