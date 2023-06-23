#pragma once
#include "Wt/Auth/Dbo/AuthInfo.h"
#include <Wt/Dbo/Types.h>
#include <Wt/WGlobal.h>
#include <Wt/WString.h>
#include <Wt/Dbo/Dbo.h>
#include <chrono>

class User;
class UserRole;

class Profile;
class ProfileService;
class ProfileGalery;
class ServiceAgeGroup;

class Event;
class EventService;

class Review;
class ReviewGalery;


namespace dbo = Wt::Dbo;

typedef dbo::collection<dbo::ptr<User>> Users;
typedef dbo::collection<dbo::ptr<UserRole>> UserRoles;

typedef dbo::collection<dbo::ptr<Profile>> Profiles;
typedef dbo::collection<dbo::ptr<ProfileService>> ProfileServices;
typedef dbo::collection<dbo::ptr<ProfileGalery>> ProfileGalerys;
typedef dbo::collection<dbo::ptr<ServiceAgeGroup>> ServiceAgeGroups;

typedef dbo::collection<dbo::ptr<Event>> Events;
typedef dbo::collection<dbo::ptr<EventService>> EventServices;

typedef dbo::collection<dbo::ptr<Review>> Reviews;
typedef dbo::collection<dbo::ptr<ReviewGalery>> ReviewGaleries;



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
	// struct dbo_traits<Service> : public dbo_default_traits
	// {
	// 	static const char *versionField() { return 0; }
	// };

}


class User : public Wt::Dbo::Dbo<User>
{
public:
	dbo::ptr<UserRole> role;
	dbo::weak_ptr<Profile> profile;
	Events eventsAsClient;
	Events events;
	Reviews reviews;
	std::string name;
	std::string phone;
	std::vector<unsigned char> photo;
	std::chrono::system_clock::time_point joinDate;
	bool darkMode;

	template <class Action>
	void persist(Action &a)
	{
		dbo::belongsTo(a, role, "user_role");
		dbo::hasOne(a, profile);
		dbo::hasMany(a, eventsAsClient, dbo::ManyToOne, "user_client");
		dbo::hasMany(a, events, dbo::ManyToOne, "user");
		dbo::hasMany(a, reviews, dbo::ManyToOne, "user");
		dbo::field(a, name, "name");
		dbo::field(a, phone, "phone");
		dbo::field(a, photo, "photo");
		dbo::field(a, joinDate, "join_date");
		dbo::field(a, darkMode, "dark_mode");
	}
};

class UserRole : public Wt::Dbo::Dbo<UserRole>
{
	public:
	std::string role;
	Users users;

	template <class Action>
	void persist(Action &a)
	{
		dbo::field(a, role, "role");
		dbo::hasMany(a, users, dbo::ManyToOne, "user_role");
	}
};


class Profile : public dbo::Dbo<Profile>
{
	public:
	dbo::ptr<User> user;
	std::string username;

	template<class Action>
	void persist(Action& a)
	{
		dbo::belongsTo(a, user, "user");
		dbo::field(a, username, "username");
	}
};

class ProfileService : public dbo::Dbo<ProfileService>
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

class ProfileGalery : public dbo::Dbo<ProfileGalery>
{
	public:

	template <class Action>
	void persist(Action &a)
	{
		
	}
};

class ServiceAgeGroup : public dbo::Dbo<ServiceAgeGroup>
{
	public:

	template <class Action>
	void persist(Action &a)
	{
		
	}
};


class Event : public dbo::Dbo<Event>
{
public:
	dbo::ptr<User> user;
	dbo::ptr<User> client;
	EventServices eventServices;
	std::chrono::system_clock::time_point dateTime;
	double duration;
	std::string location;
	std::string description;

	template <class Action>
	void persist(Action &a)
	{
		dbo::belongsTo(a, user, "user");
		dbo::belongsTo(a, client, "user_client");
		dbo::hasMany(a, eventServices, dbo::ManyToOne, "event");
		dbo::field(a, dateTime, "date_time");
		dbo::field(a, duration, "duration");
		dbo::field(a, location, "location");
		dbo::field(a, description, "description");
	}
};

class EventService : public dbo::Dbo<EventService>
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


class Review : public dbo::Dbo<Review>
{
	public:
	dbo::ptr<User> user;
	dbo::ptr<ProfileService> profileService;
	std::string title;
	std::string content;
	int rating;
	// std::vector<int> reviePhotoIds;

	template <class Action>
	void persist(Action &a)
	{
		dbo::belongsTo(a, user, "user");
		dbo::belongsTo(a, profileService, "profile_service");
		dbo::field(a, title, "title");
		dbo::field(a, content, "content");
		dbo::field(a, rating, "rating");
		// dbo::field(a, reviePhotoIds, "review_photo_ids");
	}
};

class ReviewGalery : public dbo::Dbo<ReviewGalery>
{
	public:

	template <class Action>
	void persist(Action &a)
	{

	}
};