#include "include/EventSession.h"
#include "include/DboTables.h"

#include <Wt/Dbo/Exception.h>

#include <Wt/WText.h>
#include <Wt/WDateTime.h>

#include "Wt/Auth/Dbo/AuthInfo.h" // i dont know why but i need this ????? for mapping tables to session

using namespace Wt;

namespace dbo = Wt::Dbo;

EventSession::EventSession(std::unique_ptr<dbo::SqlConnection> conn)
{
	session_.setConnection(std::move(conn));

	configureSession();
}

EventSession::~EventSession()
{
}

void EventSession::configureSession()
{
	std::cout << "\n\n Configure ---------- EVENT ---------- Session STARTED \n\n";

	session_.mapClass<UserService>("user_service");
	session_.mapClass<Client>("client");
	session_.mapClass<Service>("service");
	session_.mapClass<Event>("event");
	session_.mapClass<User>("user");

	std::cout << "\n\n Configure ---------- EVENT ---------- Session ENDED \n\n";
}

int EventSession::regEventInfo(int userId, EventInfo eventInfo)
{
	std::cout << "\n\n regeventInfo  ------------- START \n\n";

	auto dateTime = Wt::WDateTime().fromString(eventInfo.dateTime, datetime_format_);
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

	transaction.commit();
	std::cout << "\n\n regEventInfo  ------------- END \n\n";
	return eventPtrDbo.id();
}

int EventSession::regClientInfo(int userId, ClientInfo clientInfo)
{
	std::cout << "\n\n regclientInfo  ------------- START \n\n";

	dbo::Transaction transaction(session_);

	std::unique_ptr<Client> client{new Client()};
	dbo::ptr<User> user = session_.find<User>().where("id = ?").bind(userId);

	// see if client with client name and phone already exists
	dbo::ptr<Client> clientPtrDbo = session_.find<Client>().where("name = ? AND phone = ?").bind(clientInfo.name).bind(clientInfo.phone);
	if (clientPtrDbo)
	{
		std::cout << "\n\n regclientInfo  ------------- END \n\n";
		return clientPtrDbo.id();
	}

	client->user = user;
	client->name = clientInfo.name;
	client->phone = clientInfo.phone;
	client->specialNote = clientInfo.specialNote;

	dbo::ptr<Client> clientPtr = session_.add(std::move(client));
	transaction.commit();

	std::cout << "\n\n regclientInfo  ------------- END \n\n";
	return clientPtr.id();
}

void EventSession::addServiceData(ServiceInfo ServiceInfo)
{
	auto dateTime = Wt::WDateTime().fromString(ServiceInfo.dateTime, datetime_format_);
	dbo::Transaction transaction(session_);
	dbo::ptr<Event> event = session_.find<Event>().where("id = ?").bind(ServiceInfo.eventId);
	std::unique_ptr<Service> service{new Service()};

	service->event = event;
	service->providerIdentity = ServiceInfo.providerIdentity;
	service->providerService = ServiceInfo.providerService;
	service->dateTime = dateTime.toTimePoint();
	service->duration = ServiceInfo.duration;
	service->cost = ServiceInfo.cost;
	service->description = ServiceInfo.description;
	service->observations = ServiceInfo.observations;

	dbo::ptr<Service> servicePtr = session_.add(std::move(service));

	transaction.commit();
}

void EventSession::modifyServiceIntField(int serviceId, ServiceField field, int newValue)
{
	dbo::Transaction transaction(session_);
	dbo::ptr<Service> service = session_.find<Service>().where("id = ?").bind(serviceId);

	if (field == ServiceField::cost)
	{
		std::cout << "\n modifyServiceIntField: cost \n";
		service.modify()->cost = newValue;
	}
	else
	{
		std::cout << "\n modifyServiceIntField: invalid field \n";
	}

	transaction.commit();
}

void EventSession::modifyServiceStringField(int serviceId, ServiceField field, std::string newValue)
{
	dbo::Transaction transaction(session_);
	dbo::ptr<Service> service = session_.find<Service>().where("id = ?").bind(serviceId);

	if (field == ServiceField::providerIdentity)
	{
		std::cout << "\n modifyServiceStringField: providerIdentity \n";
		service.modify()->providerIdentity = newValue;
	}
	else if (field == ServiceField::providerService)
	{
		std::cout << "\n modifyServiceStringField: providerService \n";
		service.modify()->providerService = newValue;
	}
	else if (field == ServiceField::dateTime)
	{
		std::cout << "\n modifyServiceStringField: dateTime \n";
		Wt::WDateTime dateTime = Wt::WDateTime().fromString(newValue, datetime_format_);
		service.modify()->dateTime = dateTime.toTimePoint();
	}
	else if (field == ServiceField::description)
	{
		std::cout << "\n modifyServiceStringField: description \n";
		service.modify()->description = newValue;
	}
	else if (field == ServiceField::observations)
	{
		std::cout << "\n modifyServiceStringField: observations \n";
		service.modify()->observations = newValue;
	}
	else
	{
		std::cout << "modifyServiceStringField: invalid field";
	}

	transaction.commit();
}

void EventSession::modifyServiceDoubleField(int serviceId, ServiceField field, double newValue)
{
	dbo::Transaction transaction(session_);
	dbo::ptr<Service> service = session_.find<Service>().where("id = ?").bind(serviceId);

	if (field == ServiceField::duration)
	{
		std::cout << "\n modifyServiceDoubleField: duration \n";
		service.modify()->duration = newValue;
	}
	else
	{

		std::cout << "\n modifyServiceDoubleField: invalid field \n";
	}

	transaction.commit();
}

SeqEventData EventSession::getAllEvents(int userId)
{
	SeqEventData seqEventData;

	dbo::Transaction transaction(session_);

	Events eventsCollection = session_.find<Event>().where("user_id = ?").orderBy("date_time DESC").bind(userId);
	for (const dbo::ptr<Event> &event : eventsCollection)
	{
		EventData eventData;
		auto eventDateTime = Wt::WDateTime(event->dateTime);

		eventData.eventInfo.id = event->id();
		// eventData.eventInfo.clientId = event->client.id();
		eventData.eventInfo.dateTime = eventDateTime.toString(datetime_format_).toUTF8();
		eventData.eventInfo.duration = event->duration;
		eventData.eventInfo.location = event->location;
		eventData.eventInfo.description = event->description;

		// eventData.ClientInfo.name = event->client->name;
		// eventData.ClientInfo.phone = event->client->phone;
		// eventData.ClientInfo.specialNote = event->client->specialNote;

		Services services = session_.find<Service>().where("event_id = ?").orderBy("date_time DESC").bind(event->id());

		for (const dbo::ptr<Service> serviceDb : event->services)
		{
			ServiceInfo service;

			auto serviceDateTime = Wt::WDateTime(serviceDb->dateTime);

			service.id = serviceDb->id();
			service.eventId = serviceDb->event.id();
			service.providerIdentity = serviceDb->providerIdentity;
			service.providerService = serviceDb->providerService;
			service.dateTime = serviceDateTime.toString(datetime_format_).toUTF8();
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

SeqClientInfo EventSession::getAllClients(int userId)
{
	SeqClientInfo seqClientInfo;
	dbo::Transaction transaction(session_);
	try
	{
		Clients clients = session_.find<Client>().where("user_id = ?").bind(userId);

		for (const dbo::ptr<Client> client : clients)
		{
			ClientInfo ClientInfo;
			ClientInfo.id = client->id();
			ClientInfo.name = client->name;
			ClientInfo.phone = client->phone;

			seqClientInfo.push_back(ClientInfo);
		}
	}
	catch (dbo::Exception &e)
	{
		log("info") << e.what();
	}

	transaction.commit();
	return seqClientInfo;
}

SeqClientInfo EventSession::getClientsByName(int userId, std::string name)
{
	std::cout << "\n\n getClientsByName  = " << name << "\n\n";

	SeqClientInfo seqClientInfo;
	dbo::Transaction transaction(session_);
	try
	{
		Clients clients = session_.find<Client>().where("user_id = ?").bind(userId).where("name LIKE ?").bind("%" + name + "%");

		for (const dbo::ptr<Client> client : clients)
		{
			ClientInfo ClientInfo;
			ClientInfo.id = client->id();
			ClientInfo.name = client->name;
			ClientInfo.phone = client->phone;

			seqClientInfo.push_back(ClientInfo);
		}
	}
	catch (dbo::Exception &e)
	{
		log("info") << e.what();
	}

	transaction.commit();
	return seqClientInfo;
}

SeqClientInfo EventSession::getClientsByPhone(int userId, std::string phone)
{
	std::cout << "\n\n getClientsByPhone  = " << phone << "\n\n";

	SeqClientInfo seqClientInfo;
	dbo::Transaction transaction(session_);
	try
	{
		Clients clients = session_.find<Client>().where("user_id = ?").bind(userId).where("phone LIKE ?").bind(phone + "%");

		for (const dbo::ptr<Client> client : clients)
		{
			ClientInfo ClientInfo;
			ClientInfo.id = client->id();
			ClientInfo.name = client->name;
			ClientInfo.phone = client->phone;

			seqClientInfo.push_back(ClientInfo);
		}
	}
	catch (dbo::Exception &e)
	{
		log("info") << e.what();
	}

	transaction.commit();
	return seqClientInfo;
}

void EventSession::deleteRecord(Table table, int id)
{
	std::cout << "\n\n deleteRecord  = " << id << "\n\n";
	dbo::Transaction transaction(session_);

	if (table == Table::clients)
	{
		dbo::ptr<Client> client = session_.find<Client>().where("id = ?").bind(id);
		client.remove();
	}
	else if (table == Table::events)
	{
		dbo::ptr<Event> event = session_.find<Event>().where("id = ?").bind(id);
		event.remove();
	}
	else if (table == Table::services)
	{
		dbo::ptr<Service> service = session_.find<Service>().where("id = ?").bind(id);
		service.remove();
	}

	transaction.commit();
}

void EventSession::modifyEventIntField(int eventId, EventField field, int newValue)
{
	dbo::Transaction transaction(session_);
	if (field == EventField::clientId)
	{
		std::cout << "\n\n modifyEventIntField: clientId = " << newValue << "\n\n";
		dbo::ptr<Event> event = session_.find<Event>().where("id = ?").bind(eventId);
		dbo::ptr<Client> client = session_.find<Client>().where("id = ?").bind(newValue);
		// event.modify()->client = client;
	}
	else
	{
		std::cout << "\n modifyEventIntField: default case \n";
	}
	transaction.commit();
}

void EventSession::modifyEventDoubleField(int eventId, EventField field, double newValue)
{
	dbo::Transaction transaction(session_);
	if (field == EventField::duration)
	{

		std::cout << "\n\n modifyEventDoubleField: duration = " << newValue << "\n\n";
		dbo::ptr<Event> event = session_.find<Event>().where("id = ?").bind(eventId);
		event.modify()->duration = newValue;
	}
	else
	{
		std::cout << "\n modifyEventDoubleField: default case \n";
	}

	transaction.commit();
}

void EventSession::modifyEventStringField(int eventId, EventField field, std::string newValue){
	dbo::Transaction transaction(session_);
	dbo::ptr<Event> event = session_.find<Event>().where("id = ?").bind(eventId);

	if (field == EventField::dateTime)
	{
		std::cout << "\n\n modifyEventStringField: dateTime = " << newValue << "\n\n";
		auto dateTime = Wt::WDateTime().fromString(newValue, datetime_format_);
		event.modify()->dateTime = dateTime.toTimePoint();
		auto services = event->services;

		// update services dateTime
		for (const dbo::ptr<Service> service : services)
		{
			auto serviceDateTime = Wt::WDateTime(service->dateTime);
			serviceDateTime.setDate(dateTime.date());
			service.modify()->dateTime = serviceDateTime.toTimePoint();
		}
	}
	else if (field == EventField::location)
	{
		std::cout << "\n\n modifyEventStringField: location = " << newValue << "\n\n";
		event.modify()->location = newValue;
	}
	else if (field == EventField::description)
	{
		std::cout << "\n\n modifyEventStringField: description = " << newValue << "\n\n";
		event.modify()->description = newValue;
	}
	else
	{
		std::cout << "\n modifyEventStringField: default case \n";
	}

	transaction.commit();
}

SeqEventData EventSession::getTenEvents(int userId, std::string fromDate, int offset)
{
	// Events eventsCollection = session_.find<Event>().where("user_id = ?").bind(userId).orderBy("id DESC").limit(10).offset(offset);
	SeqEventData seqEventData;

	dbo::Transaction transaction(session_);
	auto dateTime = Wt::WDateTime().fromString(fromDate, date_format_);

	auto eventsQuery = session_.find<Event>();
	eventsQuery.where("user_id = ?").bind(userId);
	eventsQuery.where("date_time > ?").bind(dateTime.toTimePoint());
	eventsQuery.where("date_time < ?").bind(dateTime.addDays(1).toTimePoint());
	eventsQuery.orderBy("date_time ASC");
	eventsQuery.limit(10);
	eventsQuery.offset(offset);
	
	Events eventsCollection = eventsQuery.resultList();

	// Events eventsCollection = session_.find<Event>().where("user_id = ?").orderBy("date_time DESC").bind(userId).limit(10).offset(offset);
	for (const dbo::ptr<Event> &event : eventsCollection)
	{
		auto eventDateTime = Wt::WDateTime(event->dateTime);
		EventData eventData;
		eventData.eventInfo.id = event->id();
		// eventData.eventInfo.clientId = event->client.id();
		eventData.eventInfo.dateTime = eventDateTime.toString(datetime_format_).toUTF8();
		eventData.eventInfo.duration = event->duration;
		eventData.eventInfo.location = event->location;
		eventData.eventInfo.description = event->description;

		// eventData.elientInfo.name = event->client->name;
		// eventData.elientInfo.phone = event->client->phone;
		// eventData.elientInfo.specialNote = event->client->specialNote;

		Services services = session_.find<Service>().where("event_id = ?").orderBy("date_time DESC").bind(event->id());

		for (const dbo::ptr<Service> serviceDb : event->services)
		{
			ServiceInfo service;

			auto serviceDateTime = Wt::WDateTime(serviceDb->dateTime);

			service.id = serviceDb->id();
			service.eventId = serviceDb->event.id();
			service.providerIdentity = serviceDb->providerIdentity;
			service.providerService = serviceDb->providerService;
			service.dateTime = serviceDateTime.toString(datetime_format_).toUTF8();
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

EventData EventSession::getEventData(int eventId)
{
	std::cout << "\n\n getEventInfo: eventId = " << eventId << "\n\n";
	dbo::Transaction transaction(session_);
	EventData eventData;

	dbo::ptr<Event> event = session_.find<Event>().where("id = ?").bind(eventId);

	auto eventDateTime = Wt::WDateTime(event->dateTime);

	// EventInfoPack.EventInfo.clientId = event->client.id();
	// EventInfoPack.ClientInfo.name = event->client->name;
	// EventInfoPack.ClientInfo.phone = event->client->phone;
	// EventInfoPack.ClientInfo.specialNote = event->client->specialNote;

	eventData.eventInfo.id = event->id();
	eventData.eventInfo.dateTime = eventDateTime.toString(datetime_format_).toUTF8();
	eventData.eventInfo.duration = event->duration;
	eventData.eventInfo.location = event->location;
	eventData.eventInfo.description = event->description;

	for (const dbo::ptr<Service> &serviceDb : event->services)
	{
		ServiceInfo service;

		auto serviceDateTime = Wt::WDateTime(serviceDb->dateTime);

		service.id = serviceDb->id();
		service.eventId = serviceDb->event.id();
		service.providerIdentity = serviceDb->providerIdentity;
		service.providerService = serviceDb->providerService;
		service.dateTime = serviceDateTime.toString(datetime_format_).toUTF8();
		service.duration = serviceDb->duration;
		service.cost = serviceDb->cost;
		service.description = serviceDb->description;
		service.observations = serviceDb->observations;

		eventData.seqServiceInfo.push_back(service);
	}

	transaction.commit();
	return eventData;
}