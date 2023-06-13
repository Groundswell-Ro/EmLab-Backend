#include "include/EventsDataInterfaceI.h"

EventsDataInterfaceI::EventsDataInterfaceI(std::unique_ptr<dbo::SqlConnection> conn, std::shared_ptr<AuthInterfaceI> authInterface)
	: eventsDataSession_(std::move(conn)),
	  authInterface_(authInterface)
{
}

SeqEventDataPack EventsDataInterfaceI::getEventsData(string userToken, const Ice::Current &)
{
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	return eventsDataSession_.getAllEvents(userId);
}

EventDataModule::EventDataPack EventsDataInterfaceI::registerEvent(string userToken, EventDataPack eventDataPack, const Ice::Current &)
{

	int userId = authInterface_->authSession_.processUserTokenForId(userToken);

	int eventID = eventsDataSession_.regEventData(userId, eventDataPack.eventData);

	for (int x = 0; x < eventDataPack.seqServices.size(); ++x)
	{
		auto serviceData = eventDataPack.seqServices.at(x);
		serviceData.eventId = eventID;
		eventsDataSession_.addServiceData(serviceData);
	}

	return eventsDataSession_.getEventData(eventID);
}

void EventsDataInterfaceI::modifyEventIntField(string userToken, int eventId, EventField field, int newValue, const Ice::Current &)
{
	std::cout << "\n modifyEventIntField \n";
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	eventsDataSession_.modifyEventIntField(eventId, field, newValue);
}

void EventsDataInterfaceI::modifyEventDoubleField(string userToken, int eventId, EventField field, double newValue, const Ice::Current &)
{
	std::cout << "\n modifyEventDoubleField \n";
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	eventsDataSession_.modifyEventDoubleField(eventId, field, newValue);
}

void EventsDataInterfaceI::modifyEventStringField(string userToken, int eventId, EventField field, string newValue, const Ice::Current &)
{
	std::cout << "\n modifyEventStringField \n";
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	eventsDataSession_.modifyEventStringField(eventId, field, newValue);
}

void EventsDataInterfaceI::addServiceToEvent(string userToken, ServiceData serviceData, const Ice::Current &)
{
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	eventsDataSession_.addServiceData(serviceData);
}

void EventsDataInterfaceI::modifyServiceIntField(string userToken, int serviceId, ServiceField field, int newValue, const Ice::Current &)
{
	std::cout << "\n modifyServiceIntField \n";
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	eventsDataSession_.modifyServiceIntField(serviceId, field, newValue);
}

void EventsDataInterfaceI::modifyServiceDoubleField(string userToken, int serviceId, ServiceField field, double newValue, const Ice::Current &)
{
	std::cout << "\n modifyServiceDoubleField \n";
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	eventsDataSession_.modifyServiceDoubleField(serviceId, field, newValue);
}

void EventsDataInterfaceI::modifyServiceStringField(string userToken, int serviceId, ServiceField field, string newValue, const Ice::Current &)
{
	std::cout << "\n modifyServiceStringField \n";
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	eventsDataSession_.modifyServiceStringField(serviceId, field, newValue);
}

int EventsDataInterfaceI::registerClient(string userToken, ClientData clientData, const Ice::Current &)
{
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	int clientId = eventsDataSession_.regClientData(userId, clientData);
	return clientId;
}

void EventsDataInterfaceI::modifyClient(string userToken, ClientData clientData, const Ice::Current &)
{
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
}

SeqClients EventsDataInterfaceI::getClientsByName(string userToken, string partialName, const Ice::Current &)
{
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	return eventsDataSession_.getClientsByName(userId, partialName);
}

SeqClients EventsDataInterfaceI::getClientsByPhone(string userToken, string partialPhone, const Ice::Current &)
{
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	return eventsDataSession_.getClientsByPhone(userId, partialPhone);
}

SeqEventDataPack EventsDataInterfaceI::getTenEvents(string userToken, string fromDate, int offset, const Ice::Current &)
{
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);

	SeqEventDataPack seqEventDataPack = eventsDataSession_.getTenEvents(userId, fromDate, offset);
	return seqEventDataPack;
}

EventDataPack EventsDataInterfaceI::getEventData(string userToken, int eventId, const Ice::Current &)
{
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	return eventsDataSession_.getEventData(eventId);
}

void EventsDataInterfaceI::deleteRecord(string userToken, Table table, int id, const Ice::Current &)
{
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	eventsDataSession_.deleteRecord(table, id);
}
