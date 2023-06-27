#pragma once
#include "../../../comunication/comm/DataTypes.h"
#include <Wt/Auth/Dbo/AuthInfo.h>
#include <Wt/Dbo/Types.h>
#include <Wt/WGlobal.h>
#include <Wt/WString.h>
#include <Wt/Dbo/Dbo.h>
#include <chrono>
	


class User;
class UserRole;

class ProviderProfile;
class ProviderService;
class ServiceGalery;

class ServiceAgeGroup;
class ServiceType;

class Event;
class EventService;

class Review;
class ReviewGalery;


namespace dbo = Wt::Dbo;

typedef dbo::collection<dbo::ptr<User>> Users;
typedef dbo::collection<dbo::ptr<UserRole>> UserRoles;

typedef dbo::collection<dbo::ptr<ProviderProfile>> ProviderProfiles;
typedef dbo::collection<dbo::ptr<ProviderService>> ProviderServices;

typedef dbo::collection<dbo::ptr<ServiceGalery>> ServiceGalerys;
typedef dbo::collection<dbo::ptr<ServiceAgeGroup>> ServiceAgeGroups;
typedef dbo::collection<dbo::ptr<ServiceType>> ServiceTypes;

typedef dbo::collection<dbo::ptr<Event>> Events;
typedef dbo::collection<dbo::ptr<EventService>> EventServices;

typedef dbo::collection<dbo::ptr<Review>> Reviews;
typedef dbo::collection<dbo::ptr<ReviewGalery>> ReviewGalerys;



using AuthInfo = Wt::Auth::Dbo::AuthInfo<User>;
namespace Wt::Dbo
{

}

class User : public Wt::Dbo::Dbo<User>
{
public:
	dbo::ptr<UserRole> role;
	dbo::weak_ptr<ProviderProfile> providerProfile;
	Events events;
	std::string name;
	std::string phone;
	std::vector<unsigned char> photo;
	std::chrono::system_clock::time_point joinDate;
	bool darkMode;

	template <class Action>
	void persist(Action &a)
	{
		dbo::belongsTo(a, role, "user_role");
		dbo::hasOne(a, providerProfile);
		dbo::hasMany(a, events, dbo::ManyToOne, "user");
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

class ProviderProfile : public dbo::Dbo<ProviderProfile>
{
	public:
	dbo::ptr<User> user;
	std::string username;
	ProviderServices providerServices;

	template<class Action>
	void persist(Action& a)
	{
		dbo::belongsTo(a, user, "user");
		dbo::field(a, username, "username");
		dbo::hasMany(a, providerServices, dbo::ManyToOne, "provider_profile");
	}
};

class ProviderService : public dbo::Dbo<ProviderService>
{
public:
	dbo::ptr<ProviderProfile> providerProfile;
	ServiceGalerys serviceGalerys;
	dbo::weak_ptr<ServiceAgeGroup> serviceAgeGroup;
	dbo::weak_ptr<ServiceType> serviceType;
	std::string title;
	std::string description;
	std::string price;

	template <class Action>
	void persist(Action &a)
	{
		dbo::belongsTo(a, providerProfile, "provider_profile");
		dbo::hasMany(a, serviceGalerys, dbo::ManyToOne, "provider_service");
		dbo::hasOne(a, serviceType);
		dbo::hasOne(a, serviceAgeGroup);
		dbo::field(a, title, "title");
		dbo::field(a, description, "description");
		dbo::field(a, price, "price");
	}
};

class ServiceGalery : public dbo::Dbo<ServiceGalery>
{
	public:
	dbo::ptr<ProviderService> providerService;
	std::vector<unsigned char> photo;

	template <class Action>
	void persist(Action &a)
	{
		dbo::belongsTo(a, providerService, "provider_service");
		dbo::field(a, photo, "photo");
	}
};

class ServiceAgeGroup : public dbo::Dbo<ServiceAgeGroup>
{
	public:
	dbo::ptr<ProviderService> providerService;
	// 0 3
	// 3 5 
	// 5 8
	// 8 12
	// 12 15
	// 15 18
	// 18 23
	// 23 30
	// 30 40
	// 40 50
	// 50 more
	bool zero_three;
	bool three_five;
	bool five_eight;
	bool eight_twelve;
	bool twelve_fifteen;
	bool fifteen_eighteen;
	bool eighteen_twentythree;
	bool twentythree_thirty;
	bool thirty_fourty;
	bool fourty_fifty;
	bool fifty_more;

	template <class Action>
	void persist(Action &a)
	{
		dbo::belongsTo(a, providerService, "provider_service");
		dbo::field(a, zero_three, "zero_three");
		dbo::field(a, three_five, "three_five");
		dbo::field(a, five_eight, "five_eight");
		dbo::field(a, eight_twelve, "eight_twelve");
		dbo::field(a, twelve_fifteen, "twelve_fifteen");
		dbo::field(a, fifteen_eighteen, "fifteen_eighteen");
		dbo::field(a, eighteen_twentythree, "eighteen_twentythree");
		dbo::field(a, twentythree_thirty, "twentythree_thirty");
		dbo::field(a, thirty_fourty, "thirty_fourty");
		dbo::field(a, fourty_fifty, "fourty_fifty");
		dbo::field(a, fifty_more, "fifty_more");
		
	}
};

class ServiceType : public dbo::Dbo<ServiceType>
{
	public:
	dbo::ptr<ProviderService> providerService;
	// photo video entertainment space_renting other
	bool photo;
	bool video;
	bool entertainment;
	bool space_renting;
	bool other;


	template <class Action>
	void persist(Action &a)
	{
		dbo::belongsTo(a, providerService, "provider_service");
		dbo::field(a, photo, "photo");
		dbo::field(a, video, "video");
		dbo::field(a, entertainment, "entertainment");
		dbo::field(a, space_renting, "space_renting");
		dbo::field(a, other, "other");

	}
};

class Event : public dbo::Dbo<Event>
{
public:
	dbo::ptr<User> user;
	EventServices eventServices;
	std::chrono::system_clock::time_point dateTime;
	double duration;
	std::string location;
	std::string description;

	template <class Action>
	void persist(Action &a)
	{
		dbo::belongsTo(a, user, "user");
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
	dbo::ptr<ProviderService> providerService;
	std::string providerIdentity;
	std::chrono::system_clock::time_point dateTime;
	double duration;
	int cost;
	std::string description;
	std::string observations;

	template <class Action>
	void persist(Action &a)
	{
		dbo::belongsTo(a, event, "event", dbo::OnDeleteCascade);
		dbo::belongsTo(a, providerService, "provider_service");
		dbo::field(a, providerIdentity, "provider_identity");
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
	dbo::ptr<EventService> eventService;
	ReviewGalerys reviewGalerys;
	std::string title;
	std::string content;
	int rating;

	template <class Action>
	void persist(Action &a)
	{
		dbo::belongsTo(a, eventService, "event_service", dbo::OnDeleteCascade);
		dbo::hasMany(a, reviewGalerys, dbo::ManyToOne, "review");
		dbo::field(a, title, "title");
		dbo::field(a, content, "content");
		dbo::field(a, rating, "rating");
	}
};

class ReviewGalery : public dbo::Dbo<ReviewGalery>
{
	public:
	dbo::ptr<Review> review;
	std::vector<unsigned char> photo;

	template <class Action>
	void persist(Action &a)
	{
		dbo::belongsTo(a, review, "review", dbo::OnDeleteCascade);
		dbo::field(a, photo, "photo");
	}
};
