#pragma once
#include "../../../comunication/EventModule.h"

#include <Wt/WString.h>

#include <Wt/Dbo/Session.h>
#include <Wt/Dbo/ptr.h>

#include <Wt/Dbo/SqlConnection.h>

using namespace EventModule;

class EventSession
{
public:
    EventSession(std::unique_ptr<Wt::Dbo::SqlConnection> conn);
    ~EventSession();

    void configureSession();

    int regEventInfo(int userId, EventInfo eventInfo);
    int regClientInfo(int userId, ClientInfo clientInfo);

    void modifyEventIntField(int eventId, EventField field, int newValue);
    void modifyEventStringField(int eventId, EventField field, std::string newValue);
    void modifyEventDoubleField(int eventId, EventField field, double newValue);

    void addServiceData(ServiceInfo serviceInfo);
    void modifyServiceIntField(int serviceId, ServiceField field, int newValue);
    void modifyServiceStringField(int serviceId, ServiceField field, std::string newValue);
    void modifyServiceDoubleField(int serviceId, ServiceField field, double newValue);

    SeqEventData getAllEvents(int userId);
    SeqEventData getTenEvents(int userId, std::string fromDate, int offset);
    SeqClientInfo getAllClients(int userId);
    SeqClientInfo getClientsByName(int userId, std::string partialName);
    SeqClientInfo getClientsByPhone(int userId, std::string partialPhone);
    EventData getEventData(int eventId);
    void deleteRecord(Table table, int id);

private:
    mutable Wt::Dbo::Session session_;
    std::string time_format_ = "HH:mm AP"; 
    std::string date_format_ = "dd/MMM/yyyy";
    std::string datetime_format_ = date_format_ + " " + time_format_;
};
