#pragma once
#include "../../../comunication/AuthModule.h"
#include <Ice/Ice.h>
#include "AuthSession.h"
#include <Wt/Dbo/FixedSqlConnectionPool.h>

using namespace std;
using namespace AuthModule;

class AuthInterfaceI : public AuthInterface
{
public:
	AuthInterfaceI(std::unique_ptr<dbo::SqlConnection> conn);

	virtual StructLoginReturn tryLogin(StructLoginInfo structLoginInfo, const Ice::Current &) override;
	virtual StructRegistrationReturn tryRegister(StructRegistrationInfo structRegistrationInfo, const Ice::Current &) override;
	virtual ChangePasswordResponse tryChangePassword(string userToken, string oldPassword, string newPassword, const Ice::Current &) override;

	virtual string getUserName(string userToken, const Ice::Current &) override;

	virtual void addUserService(string userToken, ServiceInfo userServiceInfo, const Ice::Current &) override;
	virtual void removeUserService(string userToken, int userServiceId, const Ice::Current &) override;
	virtual void updateUserService(string userToken, ServiceInfo userServiceInfo, const Ice::Current &) override;
	virtual ServiceInfoSq getSelfServices(string userToken, const Ice::Current &) override;
	virtual AuthModule::UserServices getUserServicesFromUserName(string userName, const Ice::Current &) override;

	AuthSession authSession_;

private:
};