#include "include/EventInterfaceI.h"

using namespace EventModule;

EventInterfaceI::EventInterfaceI(std::unique_ptr<dbo::SqlConnection> conn, std::shared_ptr<AuthInterfaceI> authInterface)
	: eventSession_(std::move(conn)),
	  authInterface_(authInterface)
{
}

SeqEventData EventInterfaceI::getEventsData(string userToken, const Ice::Current &)
{
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	return eventSession_.getAllEvents(userId);
}

EventData EventInterfaceI::registerEvent(string userToken, EventData eventData, const Ice::Current &)
{

	int userId = authInterface_->authSession_.processUserTokenForId(userToken);

	int eventID = eventSession_.regEventInfo(userId, eventData.eventInfo);

	for (int x = 0; x < eventData.seqServiceInfo.size(); ++x)
	{
		auto serviceData = eventData.seqServiceInfo.at(x);
		serviceData.eventId = eventID;
		eventSession_.addServiceData(serviceData);
	}

	return eventSession_.getEventData(eventID);
}

void EventInterfaceI::modifyEventIntField(string userToken, int eventId, EventField field, int newValue, const Ice::Current &)
{
	std::cout << "\n modifyEventIntField \n";
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	eventSession_.modifyEventIntField(eventId, field, newValue);
}

void EventInterfaceI::modifyEventDoubleField(string userToken, int eventId, EventField field, double newValue, const Ice::Current &)
{
	std::cout << "\n modifyEventDoubleField \n";
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	eventSession_.modifyEventDoubleField(eventId, field, newValue);
}

void EventInterfaceI::modifyEventStringField(string userToken, int eventId, EventField field, string newValue, const Ice::Current &)
{
	std::cout << "\n modifyEventStringField \n";
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	eventSession_.modifyEventStringField(eventId, field, newValue);
}

void EventInterfaceI::addServiceToEvent(string userToken, ServiceInfo serviceInfo, const Ice::Current &)
{
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	eventSession_.addServiceData(serviceInfo);
}

void EventInterfaceI::modifyServiceIntField(string userToken, int serviceId, ServiceField field, int newValue, const Ice::Current &)
{
	std::cout << "\n modifyServiceIntField \n";
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	eventSession_.modifyServiceIntField(serviceId, field, newValue);
}

void EventInterfaceI::modifyServiceDoubleField(string userToken, int serviceId, ServiceField field, double newValue, const Ice::Current &)
{
	std::cout << "\n modifyServiceDoubleField \n";
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	eventSession_.modifyServiceDoubleField(serviceId, field, newValue);
}

void EventInterfaceI::modifyServiceStringField(string userToken, int serviceId, ServiceField field, string newValue, const Ice::Current &)
{
	std::cout << "\n modifyServiceStringField \n";
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	eventSession_.modifyServiceStringField(serviceId, field, newValue);
}

int EventInterfaceI::registerClient(string userToken, ClientInfo clientInfo, const Ice::Current &)
{
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	int clientId = eventSession_.regClientInfo(userId, clientInfo);
	return clientId;
}

void EventInterfaceI::modifyClient(string userToken, ClientInfo clientInfo, const Ice::Current &)
{
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
}

SeqClientInfo EventInterfaceI::getClientsByName(string userToken, string partialName, const Ice::Current &)
{
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	return eventSession_.getClientsByName(userId, partialName);
}

SeqClientInfo EventInterfaceI::getClientsByPhone(string userToken, string partialPhone, const Ice::Current &)
{
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	return eventSession_.getClientsByPhone(userId, partialPhone);
}

SeqEventData EventInterfaceI::getTenEvents(string userToken, string fromDate, int offset, const Ice::Current &)
{
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);

	SeqEventData seqEventData = eventSession_.getTenEvents(userId, fromDate, offset);
	return seqEventData;
}

EventData EventInterfaceI::getEventData(string userToken, int eventId, const Ice::Current &)
{
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	return eventSession_.getEventData(eventId);
}

void EventInterfaceI::deleteRecord(string userToken, Table table, int id, const Ice::Current &)
{
	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
	eventSession_.deleteRecord(table, id);
}
