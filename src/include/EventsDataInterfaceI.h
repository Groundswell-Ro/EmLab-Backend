#pragma once
#include "EventsDataSession.h"
#include "AuthInterfaceI.h"
#include "../../../comunication/EventDataModule.h"

#include <Ice/Ice.h>
#include "AuthSession.h"
#include <Wt/Dbo/FixedSqlConnectionPool.h>

using namespace std;

using namespace EventDataModule;

class EventsDataInterfaceI : public EventsDataInterface
{
public:
	EventsDataInterfaceI(std::unique_ptr<dbo::SqlConnection> conn, std::shared_ptr<AuthInterfaceI> authInterface);

	virtual SeqEventDataPack getEventsData(string userToken, const Ice::Current &) override;
	virtual SeqClients getClientsByName(string userToken, string partialName, const Ice::Current &) override;
	virtual SeqClients getClientsByPhone(string userToken, string partialPhone, const Ice::Current &) override;

	virtual EventDataPack getEventData(string userToken, int eventId, const Ice::Current &) override;
	virtual SeqEventDataPack getTenEvents(string userToken, string fromDate, int offset, const Ice::Current &) override; // not finished
	virtual EventDataModule::EventDataPack registerEvent(string userToken, EventDataPack eventDataPack, const Ice::Current &) override;

	virtual void modifyEventStringField(string userToken, int eventId, EventField field, string newValue, const Ice::Current &) override;
	virtual void modifyEventIntField(string userToken, int eventId, EventField field, int newValue, const Ice::Current &) override;
	virtual void modifyEventDoubleField(string userToken, int eventId, EventField field, double newValue, const Ice::Current &) override;

	virtual void addServiceToEvent(string userToken, ServiceData serviceData, const Ice::Current &) override;
	virtual void modifyServiceIntField(string userToken, int serviceId, ServiceField field, int newValue, const Ice::Current &) override;
	virtual void modifyServiceStringField(string userToken, int serviceId, ServiceField field, string newValue, const Ice::Current &) override;
	virtual void modifyServiceDoubleField(string userToken, int serviceId, ServiceField field, double newValue, const Ice::Current &) override;

	virtual int registerClient(string userToken, ClientData clientData, const Ice::Current &) override;
	virtual void modifyClient(string userToken, ClientData clientData, const Ice::Current &) override;

	virtual void deleteRecord(string userToken, Table table, int id, const Ice::Current &) override;

private:
	EventsDataSession eventsDataSession_;
	std::shared_ptr<AuthInterfaceI> authInterface_;
};
