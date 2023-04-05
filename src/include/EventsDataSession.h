#pragma once
#include "../../../comunication/EventDataModule.h"

#include <Wt/WString.h>

#include <Wt/Dbo/Session.h>
#include <Wt/Dbo/ptr.h>

#include <Wt/Dbo/SqlConnection.h>

class EventsDataSession
{
public:
    EventsDataSession(std::unique_ptr<Wt::Dbo::SqlConnection> conn);
    ~EventsDataSession();

    void configureSession();

    int regEventData(int userId, int clientId, EventDataModule::EventData eventData);
    int regClientData(int userId, EventDataModule::ClientData clientData);

    void modifyEventIntField(int eventId, EventDataModule::EventField field, int newValue);
    void modifyEventStringField(int eventId, EventDataModule::EventField field, std::string newValue);
    void modifyEventDoubleField(int eventId, EventDataModule::EventField field, double newValue);

    void addServiceData(EventDataModule::ServiceData serviceData);
    void modifyServiceIntField(int serviceId, EventDataModule::ServiceField field, int newValue);
    void modifyServiceStringField(int serviceId, EventDataModule::ServiceField field, std::string newValue);
    void modifyServiceDoubleField(int serviceId, EventDataModule::ServiceField field, double newValue);

    EventDataModule::SeqEventDataPack getAllEvents(int userId);
    EventDataModule::SeqEventDataPack getTenEvents(int userId, std::string fromDate, int offset);
    EventDataModule::SeqClients getAllClients(int userId);
    EventDataModule::SeqClients getClientsByName(int userId, std::string partialName);
    EventDataModule::SeqClients getClientsByPhone(int userId, std::string partialPhone);
    EventDataModule::EventDataPack getEventData(int eventId);
    void deleteRecord(EventDataModule::Table table, int id);

private:
    mutable Wt::Dbo::Session session_;
};
