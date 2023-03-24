#pragma once
#include "Wt/Auth/Dbo/AuthInfo.h"
#include <Wt/Dbo/Types.h>
#include <Wt/WGlobal.h>
#include <Wt/WString.h>
#include <Wt/Dbo/Dbo.h>
#include <chrono>

class Event;
class Client;
class UserService;
class Service;
class User;

namespace dbo = Wt::Dbo;

typedef dbo::collection<dbo::ptr<User>> Users;
typedef dbo::collection<dbo::ptr<Event>> Events;
typedef dbo::collection<dbo::ptr<Service>> Services;
typedef dbo::collection<dbo::ptr<Client>> Clients;
typedef dbo::collection<dbo::ptr<UserService>> UserServices;

using AuthInfo = Wt::Auth::Dbo::AuthInfo<User>;

enum class Role
{
	Admin = 0,
	User = 1
};

enum class Subscription
{
	None = 0,
	Montly = 1,
	Yearly = 2
};

namespace Wt
{
	namespace Dbo
	{
		template <>
		struct dbo_traits<User> : public dbo_default_traits
		{
			static const char *versionField() { return 0; }
		};

		template <>
		struct dbo_traits<UserService> : public dbo_default_traits
		{
			static const char *versionField() { return 0; }
		};

		template <>
		struct dbo_traits<Event> : public dbo_default_traits
		{
			static const char *versionField() { return 0; }
		};

		template <>
		struct dbo_traits<Client> : public dbo_default_traits
		{
			static const char *versionField() { return 0; }
		};

		template <>
		struct dbo_traits<Service> : public dbo_default_traits
		{
			static const char *versionField() { return 0; }
		};

	}
}

class User : public Wt::Dbo::Dbo<User>
{
public:
	Events events;
	Clients clients;
	UserServices userServices;
	std::string name;
	std::string phone;
	std::vector<unsigned char> photo;
	Role role;
	Subscription subscription;
	bool darkMode;

	template <class Action>
	void persist(Action &a)
	{
		dbo::hasMany(a, events, dbo::ManyToOne, "user");
		dbo::hasMany(a, clients, dbo::ManyToOne, "user");
		dbo::hasMany(a, userServices, dbo::ManyToOne, "user");
		dbo::field(a, name, "name");
		dbo::field(a, phone, "phone");
		dbo::field(a, photo, "photo");
		dbo::field(a, role, "role");
		dbo::field(a, subscription, "subscription");
		dbo::field(a, darkMode, "dark_mode");
	}
};

class UserService : public dbo::Dbo<Services>
{
public:
	dbo::ptr<User> user;
	std::string title;
	std::string description;
	std::string price;

	template <class Action>
	void persist(Action &a)
	{
		dbo::belongsTo(a, user, "user");
		dbo::field(a, title, "title");
		dbo::field(a, description, "description");
		dbo::field(a, price, "price");
	}
};

class Client : public dbo::Dbo<Client>
{
public:
	dbo::ptr<User> user;
	Events events;
	std::string name;
	std::string phone;
	std::string specialNote;

	template <class Action>
	void persist(Action &a)
	{
		dbo::belongsTo(a, user, "user");
		dbo::hasMany(a, events, dbo::ManyToOne, "client");
		dbo::field(a, name, "name");
		dbo::field(a, phone, "phone");
		dbo::field(a, specialNote, "specialNote");
	}
};

class Service : public dbo::Dbo<Service>
{
public:
	dbo::ptr<Event> event;
	std::string providerIdentity;
	std::string providerService;
	std::chrono::system_clock::time_point dateTime;
	double duration;
	int cost;
	std::string description;
	std::string observations;

	template <class Action>
	void persist(Action &a)
	{
		dbo::belongsTo(a, event, "event", dbo::OnDeleteCascade);
		dbo::field(a, providerIdentity, "provider_identity");
		dbo::field(a, providerService, "provider_service");
		dbo::field(a, dateTime, "date_time");
		dbo::field(a, duration, "duration");
		dbo::field(a, cost, "cost");
		dbo::field(a, description, "description");
		dbo::field(a, observations, "observations");
	}
};

class Event : public dbo::Dbo<Event>
{
public:
	dbo::ptr<User> user;
	dbo::ptr<Client> client;
	dbo::collection<dbo::ptr<Service>> services;
	std::chrono::system_clock::time_point dateTime;
	double duration;
	std::string location;
	std::string observations;

	template <class Action>
	void persist(Action &a)
	{
		dbo::belongsTo(a, user, "user");
		dbo::belongsTo(a, client, "client");
		dbo::hasMany(a, services, dbo::ManyToOne, "event");
		dbo::field(a, dateTime, "date_time");
		dbo::field(a, duration, "duration");
		dbo::field(a, location, "location");
		dbo::field(a, observations, "observations");
	}
};
