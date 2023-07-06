#include "include/ProviderInterfaceI.h"
#include <vector>

#include <Wt/Dbo/ptr.h>
#include <Wt/Dbo/SqlConnection.h>
#include <Wt/Dbo/Exception.h>

#include <Wt/WDateTime.h>

#include <Ice/Ice.h>
#include <Wt/Dbo/FixedSqlConnectionPool.h>

ProviderInterfaceI::ProviderInterfaceI(std::unique_ptr<dbo::SqlConnection> conn, std::shared_ptr<AuthInterfaceI> authInterface)
	: authInterface_(authInterface)
{
	session_.setConnection(std::move(conn));
	session_.mapClass<AuthInfo>("auth_info");
	session_.mapClass<AuthInfo::AuthIdentityType>("auth_identity");
	session_.mapClass<AuthInfo::AuthTokenType>("auth_token");

	session_.mapClass<User>("user");
	session_.mapClass<UserRole>("user_role");

	session_.mapClass<ProviderProfile>("provider_profile");
	session_.mapClass<ProviderService>("provider_service");
	session_.mapClass<ServiceGalery>("service_galery");

	session_.mapClass<ServiceAgeGroup>("service_age_group");
	session_.mapClass<ServiceType>("service_type");

	session_.mapClass<Event>("event");
	session_.mapClass<EventService>("event_service");
	

	session_.mapClass<Review>("review");
	session_.mapClass<ReviewGalery>("review_galery");
}

int ProviderInterfaceI::registerProvider(std::string userToken, Emlab::ProviderProfileInfo providerProfileInfo, const Ice::Current &)
{
	std::cout << "\n\n registerProvider \n\n";
	int providerProfileId = 0; 
	auto userId = authInterface_->processUserTokenForId(userToken);

	dbo::Transaction transaction(session_);

	std::unique_ptr<ProviderProfile> providerProfile{new ProviderProfile()};
	providerProfile->username = providerProfileInfo.username;
	providerProfile->description = providerProfileInfo.description;

	dbo::ptr<User> user = session_.find<User>().where("id = ?").bind(userId);
	providerProfile->user = user;
	dbo::ptr<ProviderProfile> providerProfilePtrDbo = session_.add(std::move(providerProfile));
	providerProfileId = providerProfilePtrDbo->id();

	transaction.commit();
	authInterface_->setUserRole(std::to_string(userId), Emlab::PROVIDERROLE);
	std::cout << "\n\n registerProvider \n\n";
	return providerProfileId;
}

int ProviderInterfaceI::registerProviderService(std::string userToken, Emlab::ProviderServiceInfo providerServiceInfo, const Ice::Current &)
{
	std::cout << "\n\n registerProvicerService \n\n";
	int serviceId = 0;
	dbo::Transaction transaction(session_);

	auto userId = authInterface_->processUserTokenForId(userToken);

	std::unique_ptr<ProviderService> providerServiceDbo{new ProviderService()};
	providerServiceDbo->title = providerServiceInfo.title;
	providerServiceDbo->description = providerServiceInfo.description;
	providerServiceDbo->price = providerServiceInfo.price;
	
	dbo::ptr<ProviderProfile> providerProfile = session_.find<ProviderProfile>().where("user_id = ?").bind(userId);
	providerServiceDbo->providerProfile = providerProfile;
	dbo::ptr<ProviderService> providerServicePtrDbo = session_.add(std::move(providerServiceDbo));
	serviceId = providerServicePtrDbo->id();
	
	transaction.commit();
	std::cout << "\n\n registerProvicerService \n\n";
	return serviceId;
}

void ProviderInterfaceI::addPhotoToService(std::string userToken, int serviceId, ImageInfo imageInfo, const Ice::Current &)
{
	std::cout << "\n\n addPhotoToService \n\n";
	dbo::Transaction transaction(session_);

	auto service = session_.find<ProviderService>().where("id = ?").bind(serviceId);
	std::unique_ptr<ServiceGalery> serviceGalery{new ServiceGalery()};
	serviceGalery->photoName = imageInfo.name;
	serviceGalery->photo = imageInfo.data;
	serviceGalery->providerService = service.resultValue();
	
	dbo::ptr<ServiceGalery> serviceGaleryPtrDbo = session_.add(std::move(serviceGalery));
	
	transaction.commit();
	std::cout << "\n\n addPhotoToService \n\n";
}

ImageData ProviderInterfaceI::getProviderProfilePhoto(int profileId, const Ice::Current &)
{
	dbo::Transaction transaction(session_);
	auto profile = session_.find<ProviderProfile>().where("id = ?").bind(profileId).resultValue();
	auto photo = profile->user->photo;
	transaction.commit();
	return photo;
}

Emlab::ProviderProfileInfo ProviderInterfaceI::getProfileAsUser(string userToken, const Ice::Current &)
{
	Emlab::ProviderProfileInfo providerProfileInfo;
	dbo::Transaction transaction(session_);

	auto user = authInterface_->processUserTokenForId(userToken);
	auto profileId = session_.find<ProviderProfile>().where("user_id = ?").bind(user).resultValue()->id();
	providerProfileInfo = getProfileAsClient(profileId, Ice::Current());

	transaction.commit();
	return providerProfileInfo;
}

Emlab::ProviderProfileInfo ProviderInterfaceI::getProfileAsClient(int profileId, const Ice::Current &)
{
	Emlab::ProviderProfileInfo providerProfileInfo;
	
	dbo::Transaction transaction(session_);
	
	auto profilePtr = session_.find<ProviderProfile>().where("id = ?").bind(profileId).resultValue();
	auto user = authInterface_->users().findWithId(std::to_string(profilePtr->user->id()));
	
	// servicePhotos
	providerProfileInfo.id = profilePtr->id();
	providerProfileInfo.username = profilePtr->username;
	providerProfileInfo.name = profilePtr->user->name;
	providerProfileInfo.phone = profilePtr->user->phone;
	providerProfileInfo.description = profilePtr->description;
	providerProfileInfo.email = authInterface_->users().email(user);
	transaction.commit();

	// std::cout << "\n\n getProfile \n\n";
	// std::cout << "\n profileId: " << profileId;
	// std::cout << "\n providerProfileInfo.id: " << providerProfileInfo.id;
	// std::cout << "\n providerProfileInfo.username: " << providerProfileInfo.username;
	// std::cout << "\n providerProfileInfo.name: " << providerProfileInfo.name;
	// std::cout << "\n providerProfileInfo.phone: " << providerProfileInfo.phone;
	// std::cout << "\n providerProfileInfo.description: " << providerProfileInfo.description;
	// std::cout << "\n providerProfileInfo.email: " << providerProfileInfo.email;
	// std::cout << "\n\n getProfile \n\n";

	return providerProfileInfo;
}

SeqProviderServiceInfo ProviderInterfaceI::getSeqProviderServices(int profileId, const Ice::Current &)
{
	std::cout << "\n\n get services \n\n";
	dbo::Transaction transaction(session_);
	SeqProviderServiceInfo seqProviderServiceInfo;
	int servicesCount = session_.find<ProviderService>().where("provider_profile_id = ?").bind(profileId).resultList().size();

	if(servicesCount == 1){
		Emlab::ProviderServiceInfo providerServiceInfo;
		auto service = session_.find<ProviderService>().where("provider_profile_id = ?").bind(profileId).resultValue();
		auto servicePhotos = session_.query<int>("select id from service_galery").where("provider_service_id = ?").bind(service->id()).resultList();
		providerServiceInfo.profileId = service->providerProfile->id();
		providerServiceInfo.id = service->id();
		providerServiceInfo.title = service->title;
		providerServiceInfo.description = service->description;
		providerServiceInfo.price = service->price;
		
		for(const auto& photoId : servicePhotos)
		{
			providerServiceInfo.photoIds.push_back(photoId);
		}
		
		seqProviderServiceInfo.push_back(providerServiceInfo);
		
	}else {
		ProviderServices providerServices = session_.find<ProviderService>().where("provider_profile_id = ?").bind(profileId).resultList();	
		for(const dbo::ptr<ProviderService> &providerService : providerServices)
		{
			Emlab::ProviderServiceInfo providerServiceInfo;
			providerServiceInfo.id = providerService->id();
			providerServiceInfo.title = providerService->title;
			providerServiceInfo.description = providerService->description;
			providerServiceInfo.price = providerService->price;
			seqProviderServiceInfo.push_back(providerServiceInfo);
		}

	}
	transaction.commit();
	return seqProviderServiceInfo;
}

ImageInfo ProviderInterfaceI::getProviderServicePhoto(int servicePhotoId, const Ice::Current &)
{
	ImageInfo imageInfo;
	dbo::Transaction transaction(session_);

	auto photo = session_.find<ServiceGalery>().where("id = ?").bind(servicePhotoId).resultValue();
	imageInfo.name = photo->photoName;
	imageInfo.data = photo->photo;

	transaction.commit();
	return imageInfo;
}