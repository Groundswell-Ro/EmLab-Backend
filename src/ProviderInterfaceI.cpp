#include "include/ProviderInterfaceI.h"

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
	session_.mapClass<User>("user");
	session_.mapClass<ProviderProfile>("provider_profile");

	session_.mapClass<ProviderService>("provider_service");
	session_.mapClass<ServiceGalery>("service_galery");
	session_.mapClass<ServiceAgeGroup>("service_age_group");

	dbo::Transaction transaction(session_);
	try
	{
		session_.createTables();

	}
	catch (dbo::Exception &e)
	{
		Wt::log("info") << e.what();
	}
	catch (...)
	{
		Wt::log("info") << "other exception throw ------- mesage from AUTH session";
	}
	transaction.commit();
}


void ProviderInterfaceI::setProviderProfile(string email, ProfileData profileData, const Ice::Current &)
{

}

ProviderProfileInfo ProviderInterfaceI::getProfileInfo(string email, const Ice::Current &)
{
	ProviderProfileInfo providerProfileInfo;
	return providerProfileInfo;
}

ProfileData ProviderInterfaceI::getProfileData(string email, const Ice::Current &)
{
	ProfileData profileData;
	return profileData;
}

