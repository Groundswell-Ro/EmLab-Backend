#pragma once
#include "EventSession.h"
#include "AuthInterfaceI.h"
#include "../../../comunication/EventModule.h"

#include <Ice/Ice.h>
#include "AuthSession.h"
#include <Wt/Dbo/FixedSqlConnectionPool.h>

using namespace std;

using namespace EventModule;

class EventInterfaceI : public EventInterface
{
public:
	EventInterfaceI(std::unique_ptr<dbo::SqlConnection> conn, std::shared_ptr<AuthInterfaceI> authInterface);

	virtual SeqEventData getEventsData(string userToken, const Ice::Current &) override;
	virtual SeqClientInfo getClientsByName(string userToken, string partialName, const Ice::Current &) override;
	virtual SeqClientInfo getClientsByPhone(string userToken, string partialPhone, const Ice::Current &) override;

	virtual EventData getEventData(string userToken, int eventId, const Ice::Current &) override;
	virtual SeqEventData getTenEvents(string userToken, string fromDate, int offset, const Ice::Current &) override; // not finished
	virtual EventData registerEvent(string userToken, EventData EventData, const Ice::Current &) override;

	virtual void modifyEventStringField(string userToken, int eventId, EventField field, string newValue, const Ice::Current &) override;
	virtual void modifyEventIntField(string userToken, int eventId, EventField field, int newValue, const Ice::Current &) override;
	virtual void modifyEventDoubleField(string userToken, int eventId, EventField field, double newValue, const Ice::Current &) override;

	virtual void addServiceToEvent(string userToken, ServiceInfo serviceInfo, const Ice::Current &) override;
	virtual void modifyServiceIntField(string userToken, int serviceId, ServiceField field, int newValue, const Ice::Current &) override;
	virtual void modifyServiceStringField(string userToken, int serviceId, ServiceField field, string newValue, const Ice::Current &) override;
	virtual void modifyServiceDoubleField(string userToken, int serviceId, ServiceField field, double newValue, const Ice::Current &) override;

	virtual int registerClient(string userToken, ClientInfo clientInfo, const Ice::Current &) override;
	virtual void modifyClient(string userToken, ClientInfo clientInfo, const Ice::Current &) override;

	virtual void deleteRecord(string userToken, Table table, int id, const Ice::Current &) override;

private:
	EventSession eventSession_;
	std::shared_ptr<AuthInterfaceI> authInterface_;
};
