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

	virtual LoginReturn loginUser(LoginInfo loginInfo, const Ice::Current &) override;
	virtual RegistrationResponse registerUser(RegistrationInfo registrationInfo, const Ice::Current &) override;
	virtual ChangePasswordResponse changePassword(string userToken, string oldPassword, string newPassword, const Ice::Current &) override;

	virtual string getUserName(string userToken, const Ice::Current &) override;


	AuthSession authSession_;

private:
};