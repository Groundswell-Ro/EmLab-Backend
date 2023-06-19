#pragma once
#include "Wt/Auth/Dbo/AuthInfo.h"
#include <Wt/Dbo/Types.h>
#include <Wt/WGlobal.h>
#include <Wt/WString.h>
#include <Wt/Dbo/Dbo.h>
#include <chrono>

class User;
class UserRole;
class ProviderService;

class Event;
class Service;
class Client;

class Review;

namespace dbo = Wt::Dbo;

typedef dbo::collection<dbo::ptr<User>> Users;
typedef dbo::collection<dbo::ptr<Event>> Events;
typedef dbo::collection<dbo::ptr<Service>> Services;
typedef dbo::collection<dbo::ptr<Client>> Clients;
typedef dbo::collection<dbo::ptr<ProviderService>> ProviderServices;
typedef dbo::collection<dbo::ptr<Review>> Reviews;

using AuthInfo = Wt::Auth::Dbo::AuthInfo<User>;
namespace Wt::Dbo
{
	template <>
	struct dbo_traits<UserRole> : public dbo_default_traits
	{
		static const char *versionField() { return 0; }
	};

	template <>
	struct dbo_traits<User> : public dbo_default_traits
	{
		static const char *versionField() { return 0; }
	};

	// template <>
	// struct dbo_traits<ProviderService> : public dbo_default_traits
	// {
	// 	static const char *versionField() { return 0; }
	// };

	// template <>
	// struct dbo_traits<Event> : public dbo_default_traits
	// {
	// 	static const char *versionField() { return 0; }
	// };

	// template <>
	// struct dbo_traits<Client> : public dbo_default_traits
	// {
	// 	static const char *versionField() { return 0; }
	// };

	// template <>
	// struct dbo_traits<Service> : public dbo_default_traits
	// {
	// 	static const char *versionField() { return 0; }
	// };

}

class UserRole : public Wt::Dbo::Dbo<UserRole>
{
	public:
	std::string role;
	dbo::collection< dbo::ptr<User> > users;

	template <class Action>
	void persist(Action &a)
	{
		dbo::field(a, role, "role");
		dbo::hasMany(a, users, dbo::ManyToOne, "role");
	}
};

class User : public Wt::Dbo::Dbo<User>
{
public:
	Events events;
	Reviews reviews;
	Clients clients;
	std::string name;
	std::string phone;
	std::vector<unsigned char> photo;
	dbo::ptr<UserRole> role;
	bool darkMode;

	template <class Action>
	void persist(Action &a)
	{
		dbo::hasMany(a, events, dbo::ManyToOne, "user");
		dbo::hasMany(a, clients, dbo::ManyToOne, "user");
		dbo::hasMany(a, reviews, dbo::ManyToOne, "user");
		dbo::field(a, name, "name");
		dbo::field(a, phone, "phone");
		dbo::field(a, photo, "photo");
		dbo::belongsTo(a, role, "role");
		dbo::field(a, darkMode, "dark_mode");
	}
};

class ProviderService : public dbo::Dbo<Services>
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
	dbo::ptr<Review> review;
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
		dbo::belongsTo(a, review, "review");
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
	std::string description;

	template <class Action>
	void persist(Action &a)
	{
		dbo::belongsTo(a, user, "user");
		dbo::belongsTo(a, client, "client");
		dbo::hasMany(a, services, dbo::ManyToOne, "event");
		dbo::field(a, dateTime, "date_time");
		dbo::field(a, duration, "duration");
		dbo::field(a, location, "location");
		dbo::field(a, description, "description");
	}
};

class Review : public dbo::Dbo<Review>
{
	public:
	dbo::ptr<User> user;
	dbo::ptr<ProviderService> providerService;
	dbo::ptr<Service> service;
	std::string title;
	std::string content;
	int rating;
	// std::vector<int> reviePhotoIds;

	template <class Action>
	void persist(Action &a)
	{
		dbo::belongsTo(a, user, "user");
		dbo::belongsTo(a, providerService, "provider_service");
		dbo::belongsTo(a, service, "service");
		dbo::field(a, title, "title");
		dbo::field(a, content, "content");
		dbo::field(a, rating, "rating");
		// dbo::field(a, reviePhotoIds, "review_photo_ids");
	}
};
