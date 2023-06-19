#include "include/EventInterfaceI.h"

EventInterfaceI::EventInterfaceI(std::unique_ptr<dbo::SqlConnection> conn, std::shared_ptr<AuthInterfaceI> authInterface)
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

int EventInterfaceI::addEventInfo(std::string userToken, EventInfo eventInfo, const Ice::Current &)
{
	int userId = authInterface_->processUserTokenForId(userToken);
	auto dateTime = Wt::WDateTime().fromString(eventInfo.dateTime, Emlab::DATETIMEFORMAT);
	dbo::Transaction transaction(session_);

	dbo::ptr<User> user = session_.find<User>().where("id = ?").bind(userId);
	// dbo::ptr<Client> client = session_.find<Client>().where("id = ?").bind(clientId);
	std::unique_ptr<Event> event{new Event()};

	event->user = user;
	// event->client = client;
	event->dateTime = dateTime.toTimePoint();
	event->duration = eventInfo.duration;
	event->location = eventInfo.location;
	event->description = eventInfo.description;
	dbo::ptr<Event> eventPtrDbo = session_.add(std::move(event));
	eventInfo.id = eventPtrDbo->id();
	transaction.commit();
	// for (int x = 0; x < eventData.seqServiceInfo.size(); ++x)
	// {
	// 	auto serviceData = eventData.seqServiceInfo.at(x);
	// 	serviceData.eventId = eventID;
	// 	eventSession_.addServiceData(serviceData);
	// }
	return eventInfo.id;
}

int EventInterfaceI::addEventData(std::string userToken, EventData eventData, const Ice::Current &)
{
	int id = 0;
	std::cout << "\n\n addEventData \n\n";
	return id;
}


void EventInterfaceI::delEvent(std::string userToken, int eventId, const Ice::Current &)
{
	int userId = authInterface_->processUserTokenForId(userToken);
	dbo::Transaction transaction(session_);
	dbo::ptr<Event> event = session_.find<Event>().where("id = ?").bind(eventId);
	event.remove();
	transaction.commit();
}

void EventInterfaceI::setEventDateTime(std::string userToken, int eventId, std::string newDateTime, const Ice::Current &)
{
	std::cout << "\n\n setEventDateTime \n\n";
}

void EventInterfaceI::setEventDate(std::string userToken, int eventId, std::string newDate, const Ice::Current &)
{
	std::cout << "\n\n setEventDate \n\n";
}

void EventInterfaceI::setEventTime(std::string userToken, int eventId, std::string newTime, const Ice::Current &)
{
	std::cout << "\n\n setEventTime \n\n";
}

void EventInterfaceI::setClient(std::string userToken, int eventId, int clientId, const Ice::Current &)
{
	std::cout << "\n\n setClient \n\n";
}

void EventInterfaceI::setDuration(std::string userToken, int eventId, int duration, const Ice::Current &)
{
	std::cout << "\n\n setDuration \n\n";
}

void EventInterfaceI::setLocation(std::string userToken, int eventId, std::string location, const Ice::Current &)
{
	std::cout << "\n\n setLocation \n\n";
}

void EventInterfaceI::setDescription(std::string userToken, int eventId, std::string description, const Ice::Current &)
{
	std::cout << "\n\n setDescription \n\n";
}


SeqEventData EventInterfaceI::getSeqEventData(std::string userToken, const Ice::Current &)
{
	int userId = authInterface_->processUserTokenForId(userToken);
	SeqEventData seqEventData;
	
	dbo::Transaction transaction(session_);

	Events eventsCollection = session_.find<Event>().where("user_id = ?").orderBy("date_time DESC").bind(userId);
	for (const dbo::ptr<Event> &event : eventsCollection)
	{
		EventData eventData;
		auto eventDateTime = Wt::WDateTime(event->dateTime);

		eventData.eventInfo.id = event->id();
		eventData.eventInfo.dateTime = eventDateTime.toString(Emlab::DATETIMEFORMAT).toUTF8();
		eventData.eventInfo.duration = event->duration;
		eventData.eventInfo.location = event->location;
		eventData.eventInfo.description = event->description;

		Services services = session_.find<Service>().where("event_id = ?").orderBy("date_time DESC").bind(event->id());

		for (const dbo::ptr<Service> serviceDb : event->services)
		{
			ServiceInfo service;
			auto serviceDateTime = Wt::WDateTime(serviceDb->dateTime);

			service.id = serviceDb->id();
			service.eventId = serviceDb->event.id();
			service.providerIdentity = serviceDb->providerIdentity;
			service.providerService = serviceDb->providerService;
			service.dateTime = serviceDateTime.toString(Emlab::DATETIMEFORMAT).toUTF8();
			service.cost = serviceDb->cost;
			service.duration = serviceDb->duration;
			service.description = serviceDb->description;
			service.observations = serviceDb->observations;

			eventData.seqServiceInfo.push_back(service);
		}

		seqEventData.push_back(eventData);
	}
	transaction.commit();

	return seqEventData;
}



EventData EventInterfaceI::getEventData(std::string userToken, int eventId, const Ice::Current &)
{
	EventData eventData;
	eventData.eventInfo.description = "test";
	std::cout << "\n\n getEventData \n\n";
	return eventData;
}

EventInfo EventInterfaceI::getEventInfo(std::string userToken, int eventId, const Ice::Current &)
{
	EventInfo eventInfo;
	std::cout << "\n\n getEventInfo \n\n";
	return eventInfo;
}


// void EventInterfaceI::addServiceToEvent(string userToken, ServiceInfo serviceInfo, const Ice::Current &)
// {
// 	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
// 	eventSession_.addServiceData(serviceInfo);
// }

// int EventInterfaceI::registerClient(string userToken, ClientInfo clientInfo, const Ice::Current &)
// {
// 	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
// 	int clientId = eventSession_.regClientInfo(userId, clientInfo);
// 	return clientId;
// }

// void EventInterfaceI::modifyClient(string userToken, ClientInfo clientInfo, const Ice::Current &)
// {
// 	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
// }

// SeqClientInfo EventInterfaceI::getClientsByName(string userToken, string partialName, const Ice::Current &)
// {
// 	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
// 	return eventSession_.getClientsByName(userId, partialName);
// }

// SeqClientInfo EventInterfaceI::getClientsByPhone(string userToken, string partialPhone, const Ice::Current &)
// {
// 	int userId = authInterface_->authSession_.processUserTokenForId(userToken);
// 	return eventSession_.getClientsByPhone(userId, partialPhone);
// }