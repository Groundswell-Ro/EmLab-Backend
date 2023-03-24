#include "include/EventsDataSession.h"
#include "include/DboTables.h"

#include <Wt/Dbo/Exception.h>

#include <Wt/WText.h>
#include <Wt/WDateTime.h>

#include "Wt/Auth/Dbo/AuthInfo.h" // i dont know why but i need this ????? for mapping tables to session

using namespace Wt;

namespace dbo = Wt::Dbo;

EventsDataSession::EventsDataSession(std::unique_ptr<dbo::SqlConnection> conn)
{
	session_.setConnection(std::move(conn));

	configureSession();
}

EventsDataSession::~EventsDataSession()
{
}

void EventsDataSession::configureSession()
{
	std::cout << "\n\n Configure ---------- EVENT ---------- Session STARTED \n\n";

	session_.mapClass<UserService>("user_service");
	session_.mapClass<Client>("client");
	session_.mapClass<Service>("service");
	session_.mapClass<Event>("event");
	session_.mapClass<User>("user");

	std::cout << "\n\n Configure ---------- EVENT ---------- Session ENDED \n\n";
}

int EventsDataSession::regEventData(int userId, int clientId, EventDataModule::EventData eventData)
{
	std::cout << "\n\n regEventData  ------------- START \n\n";

	auto dateTime = Wt::WDateTime().fromString(eventData.dateTime, "dd/MM/yyyy HH:mm AP");
	dbo::Transaction transaction(session_);

	dbo::ptr<User> user = session_.find<User>().where("id = ?").bind(userId);
	dbo::ptr<Client> client = session_.find<Client>().where("id = ?").bind(clientId);
	std::unique_ptr<Event> event{new Event()};

	event->user = user;
	event->client = client;

	event->dateTime = dateTime.toTimePoint();
	event->duration = eventData.duration;
	event->location = eventData.location;
	event->observations = eventData.observations;

	dbo::ptr<Event> eventPtrDbo = session_.add(std::move(event));

	transaction.commit();
	std::cout << "\n\n regEventData  ------------- END \n\n";
	return eventPtrDbo.id();
}

int EventsDataSession::regClientData(int userId, EventDataModule::ClientData clientData)
{
	std::cout << "\n\n regClientData  ------------- START \n\n";

	dbo::Transaction transaction(session_);

	std::unique_ptr<Client> client{new Client()};
	dbo::ptr<User> user = session_.find<User>().where("id = ?").bind(userId);

	// see if client with client name and phone already exists
	dbo::ptr<Client> clientPtrDbo = session_.find<Client>().where("name = ? AND phone = ?").bind(clientData.name).bind(clientData.phone);
	if (clientPtrDbo)
	{
		std::cout << "\n\n regClientData  ------------- END \n\n";
		return clientPtrDbo.id();
	}

	client->user = user;
	client->name = clientData.name;
	client->phone = clientData.phone;
	client->specialNote = clientData.specialNote;

	dbo::ptr<Client> clientPtr = session_.add(std::move(client));
	transaction.commit();

	std::cout << "\n\n regClientData  ------------- END \n\n";
	return clientPtr.id();
}

void EventsDataSession::addServiceData(EventDataModule::ServiceData serviceData)
{
	auto dateTime = Wt::WDateTime().fromString(serviceData.dateTime, "dd/MM/yyyy HH:mm AP");
	dbo::Transaction transaction(session_);
	dbo::ptr<Event> event = session_.find<Event>().where("id = ?").bind(serviceData.eventId);
	std::unique_ptr<Service> service{new Service()};

	service->event = event;
	service->providerIdentity = serviceData.providerIdentity;
	service->providerService = serviceData.providerService;
	service->dateTime = dateTime.toTimePoint();
	service->duration = serviceData.duration;
	service->cost = serviceData.cost;
	service->description = serviceData.description;
	service->observations = serviceData.observations;

	dbo::ptr<Service> servicePtr = session_.add(std::move(service));

	transaction.commit();
}

void EventsDataSession::modifyServiceIntField(int serviceId, EventDataModule::ServiceField field, int newValue)
{
	dbo::Transaction transaction(session_);
	dbo::ptr<Service> service = session_.find<Service>().where("id = ?").bind(serviceId);

	if (field == EventDataModule::ServiceField::cost)
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

void EventsDataSession::modifyServiceStringField(int serviceId, EventDataModule::ServiceField field, std::string newValue)
{
	dbo::Transaction transaction(session_);
	dbo::ptr<Service> service = session_.find<Service>().where("id = ?").bind(serviceId);

	if (field == EventDataModule::ServiceField::providerIdentity)
	{
		std::cout << "\n modifyServiceStringField: providerIdentity \n";
		service.modify()->providerIdentity = newValue;
	}
	else if (field == EventDataModule::ServiceField::providerService)
	{
		std::cout << "\n modifyServiceStringField: providerService \n";
		service.modify()->providerService = newValue;
	}
	else if (field == EventDataModule::ServiceField::dateTime)
	{
		std::cout << "\n modifyServiceStringField: dateTime \n";
		Wt::WDateTime dateTime = Wt::WDateTime().fromString(newValue, "dd/MM/yyyy HH:mm AP");
		service.modify()->dateTime = dateTime.toTimePoint();
	}
	else if (field == EventDataModule::ServiceField::description)
	{
		std::cout << "\n modifyServiceStringField: description \n";
		service.modify()->description = newValue;
	}
	else if (field == EventDataModule::ServiceField::observations)
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

void EventsDataSession::modifyServiceDoubleField(int serviceId, EventDataModule::ServiceField field, double newValue)
{
	dbo::Transaction transaction(session_);
	dbo::ptr<Service> service = session_.find<Service>().where("id = ?").bind(serviceId);

	if (field == EventDataModule::ServiceField::duration)
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

EventDataModule::SeqEventDataPack EventsDataSession::getAllEvents(int userId)
{
	EventDataModule::SeqEventDataPack seqEventDataPack;

	dbo::Transaction transaction(session_);

	Events eventsCollection = session_.find<Event>().where("user_id = ?").orderBy("date_time DESC").bind(userId);
	for (const dbo::ptr<Event> &event : eventsCollection)
	{
		EventDataModule::EventDataPack eventDataPack;
		auto eventDateTime = Wt::WDateTime(event->dateTime);

		eventDataPack.eventData.id = event->id();
		eventDataPack.eventData.clientId = event->client.id();
		eventDataPack.eventData.dateTime = eventDateTime.toString("dd/MM/yyyy HH:mm AP").toUTF8();
		eventDataPack.eventData.duration = event->duration;
		eventDataPack.eventData.location = event->location;
		eventDataPack.eventData.observations = event->observations;

		eventDataPack.clientData.name = event->client->name;
		eventDataPack.clientData.phone = event->client->phone;
		eventDataPack.clientData.specialNote = event->client->specialNote;

		Services services = session_.find<Service>().where("event_id = ?").orderBy("date_time DESC").bind(event->id());

		for (const dbo::ptr<Service> serviceDb : event->services)
		{
			EventDataModule::ServiceData service;

			auto serviceDateTime = Wt::WDateTime(serviceDb->dateTime);

			service.id = serviceDb->id();
			service.eventId = serviceDb->event.id();
			service.providerIdentity = serviceDb->providerIdentity;
			service.providerService = serviceDb->providerService;
			service.dateTime = serviceDateTime.toString("dd/MM/yyyy HH:mm AP").toUTF8();
			service.cost = serviceDb->cost;
			service.duration = serviceDb->duration;
			service.description = serviceDb->description;
			service.observations = serviceDb->observations;

			eventDataPack.seqServices.push_back(service);
		}

		seqEventDataPack.push_back(eventDataPack);
	}

	transaction.commit();

	return seqEventDataPack;
}

EventDataModule::SeqClients EventsDataSession::getAllClients(int userId)
{
	EventDataModule::SeqClients seqClients;
	dbo::Transaction transaction(session_);
	try
	{
		Clients clients = session_.find<Client>().where("user_id = ?").bind(userId);

		for (const dbo::ptr<Client> client : clients)
		{
			EventDataModule::ClientData clientData;
			clientData.id = client->id();
			clientData.name = client->name;
			clientData.phone = client->phone;

			seqClients.push_back(clientData);
		}
	}
	catch (dbo::Exception &e)
	{
		log("info") << e.what();
	}

	transaction.commit();
	return seqClients;
}

EventDataModule::SeqClients EventsDataSession::getClientsByName(int userId, std::string name)
{
	std::cout << "\n\n getClientsByName  = " << name << "\n\n";

	EventDataModule::SeqClients seqClients;
	dbo::Transaction transaction(session_);
	try
	{
		Clients clients = session_.find<Client>().where("user_id = ?").bind(userId).where("name LIKE ?").bind("%" + name + "%");

		for (const dbo::ptr<Client> client : clients)
		{
			EventDataModule::ClientData clientData;
			clientData.id = client->id();
			clientData.name = client->name;
			clientData.phone = client->phone;

			seqClients.push_back(clientData);
		}
	}
	catch (dbo::Exception &e)
	{
		log("info") << e.what();
	}

	transaction.commit();
	return seqClients;
}

EventDataModule::SeqClients EventsDataSession::getClientsByPhone(int userId, std::string phone)
{
	std::cout << "\n\n getClientsByPhone  = " << phone << "\n\n";

	EventDataModule::SeqClients seqClients;
	dbo::Transaction transaction(session_);
	try
	{
		Clients clients = session_.find<Client>().where("user_id = ?").bind(userId).where("phone LIKE ?").bind(phone + "%");

		for (const dbo::ptr<Client> client : clients)
		{
			EventDataModule::ClientData clientData;
			clientData.id = client->id();
			clientData.name = client->name;
			clientData.phone = client->phone;

			seqClients.push_back(clientData);
		}
	}
	catch (dbo::Exception &e)
	{
		log("info") << e.what();
	}

	transaction.commit();
	return seqClients;
}

void EventsDataSession::deleteRecord(EventDataModule::Table table, int id)
{
	std::cout << "\n\n deleteRecord  = " << id << "\n\n";
	dbo::Transaction transaction(session_);

	if (table == EventDataModule::Table::clients)
	{
		dbo::ptr<Client> client = session_.find<Client>().where("id = ?").bind(id);
		client.remove();
	}
	else if (table == EventDataModule::Table::events)
	{
		dbo::ptr<Event> event = session_.find<Event>().where("id = ?").bind(id);
		event.remove();
	}
	else if (table == EventDataModule::Table::services)
	{
		dbo::ptr<Service> service = session_.find<Service>().where("id = ?").bind(id);
		service.remove();
	}

	transaction.commit();
}

void EventsDataSession::modifyEventIntField(int eventId, EventDataModule::EventField field, int newValue)
{
	dbo::Transaction transaction(session_);
	if (field == EventDataModule::EventField::clientId)
	{
		std::cout << "\n\n modifyEventIntField: clientId = " << newValue << "\n\n";
		dbo::ptr<Event> event = session_.find<Event>().where("id = ?").bind(eventId);
		dbo::ptr<Client> client = session_.find<Client>().where("id = ?").bind(newValue);
		event.modify()->client = client;
	}
	else
	{
		std::cout << "\n modifyEventIntField: default case \n";
	}
	transaction.commit();
}

void EventsDataSession::modifyEventDoubleField(int eventId, EventDataModule::EventField field, double newValue)
{
	dbo::Transaction transaction(session_);
	if (field == EventDataModule::EventField::duration)
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

void EventsDataSession::modifyEventStringField(int eventId, EventDataModule::EventField field, std::string newValue)
{
	dbo::Transaction transaction(session_);
	dbo::ptr<Event> event = session_.find<Event>().where("id = ?").bind(eventId);

	if (field == EventDataModule::EventField::dateTime)
	{
		std::cout << "\n\n modifyEventStringField: dateTime = " << newValue << "\n\n";
		auto dateTime = Wt::WDateTime().fromString(newValue, "dd/MM/yyyy HH:mm AP");
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
	else if (field == EventDataModule::EventField::location)
	{
		std::cout << "\n\n modifyEventStringField: location = " << newValue << "\n\n";
		event.modify()->location = newValue;
	}
	else if (field == EventDataModule::EventField::observations)
	{
		std::cout << "\n\n modifyEventStringField: observations = " << newValue << "\n\n";
		event.modify()->observations = newValue;
	}
	else
	{
		std::cout << "\n modifyEventStringField: default case \n";
	}

	transaction.commit();
}

EventDataModule::SeqEventDataPack EventsDataSession::getTenEvents(int userId, int offset)
{
	dbo::Transaction transaction(session_);
	EventDataModule::SeqEventDataPack seqEventDataPack;

	Events eventsCollection = session_.find<Event>().where("user_id = ?").bind(userId).orderBy("id DESC").limit(10).offset(offset);

	for (const dbo::ptr<Event> &event : eventsCollection)
	{

		EventDataModule::EventDataPack eventDataPack;

		auto eventDateTime = Wt::WDateTime(event->dateTime);

		eventDataPack.eventData.id = event->id();
		eventDataPack.eventData.clientId = event->client.id();
		eventDataPack.eventData.dateTime = eventDateTime.toString("dd/MM/yyyy HH:mm AP").toUTF8();
		eventDataPack.eventData.duration = event->duration;
		eventDataPack.eventData.location = event->location;
		eventDataPack.eventData.observations = event->observations;

		for (const dbo::ptr<Service> &serviceDb : event->services)
		{
			EventDataModule::ServiceData service;

			auto serviceDateTime = Wt::WDateTime(serviceDb->dateTime);

			service.id = serviceDb->id();
			service.eventId = serviceDb->event.id();
			service.providerIdentity = serviceDb->providerIdentity;
			service.providerService = serviceDb->providerService;
			service.dateTime = serviceDateTime.toString("dd/MM/yyyy HH:mm AP").toUTF8();
			service.duration = serviceDb->duration;
			service.cost = serviceDb->cost;
			service.description = serviceDb->description;
			service.observations = serviceDb->observations;

			eventDataPack.seqServices.push_back(service);
		}
	}

	transaction.commit();
	return seqEventDataPack;
}

EventDataModule::EventDataPack EventsDataSession::getEventData(int eventId)
{
	std::cout << "\n\n getEventData: eventId = " << eventId << "\n\n";
	dbo::Transaction transaction(session_);
	EventDataModule::EventDataPack eventDataPack;

	dbo::ptr<Event> event = session_.find<Event>().where("id = ?").bind(eventId);

	auto eventDateTime = Wt::WDateTime(event->dateTime);

	eventDataPack.eventData.clientId = event->client.id();
	eventDataPack.clientData.name = event->client->name;
	eventDataPack.clientData.phone = event->client->phone;
	eventDataPack.clientData.specialNote = event->client->specialNote;

	eventDataPack.eventData.id = event->id();
	eventDataPack.eventData.dateTime = eventDateTime.toString("dd/MM/yyyy HH:mm AP").toUTF8();
	eventDataPack.eventData.duration = event->duration;
	eventDataPack.eventData.location = event->location;
	eventDataPack.eventData.observations = event->observations;

	for (const dbo::ptr<Service> &serviceDb : event->services)
	{
		EventDataModule::ServiceData service;

		auto serviceDateTime = Wt::WDateTime(serviceDb->dateTime);

		service.id = serviceDb->id();
		service.eventId = serviceDb->event.id();
		service.providerIdentity = serviceDb->providerIdentity;
		service.providerService = serviceDb->providerService;
		service.dateTime = serviceDateTime.toString("dd/MM/yyyy HH:mm AP").toUTF8();
		service.duration = serviceDb->duration;
		service.cost = serviceDb->cost;
		service.description = serviceDb->description;
		service.observations = serviceDb->observations;

		eventDataPack.seqServices.push_back(service);
	}

	transaction.commit();
	return eventDataPack;
}