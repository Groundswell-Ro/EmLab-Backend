#pragma once
#include <comm/AuthInterface.h>
#include "DboTables.h"

#include <Ice/Ice.h>
#include <Wt/Auth/Dbo/UserDatabase.h>
#include <Wt/Dbo/SqlConnection.h>
#include <Wt/Dbo/Session.h>
#include <Wt/Dbo/ptr.h>
#include <Wt/Auth/AuthService.h>
#include <Wt/Auth/HashFunction.h>
#include <Wt/Auth/PasswordService.h>
#include <Wt/Auth/PasswordStrengthValidator.h>
#include <Wt/Auth/PasswordVerifier.h>
#include <Wt/Auth/Dbo/AuthInfo.h>
#include <Wt/Auth/Token.h>

#include <Wt/Auth/GoogleService.h>
#include <Wt/Auth/FacebookService.h>

#include <Wt/Dbo/Exception.h>
#include <Wt/Dbo/FixedSqlConnectionPool.h>

using namespace Emlab;

using UserDatabase = Wt::Auth::Dbo::UserDatabase<AuthInfo>;

class AuthInterfaceI : public AuthInterface
{
public:
	AuthInterfaceI(std::unique_ptr<dbo::SqlConnection> conn);

	virtual LoginReturn loginUser(LoginInfo loginInfo, const Ice::Current &) override;
	virtual RegistrationResponse registerUser(RegistrationInfo registrationInfo, const Ice::Current &) override;
	virtual ChangePasswordResponse changePassword(std::string userToken, std::string oldPassword, std::string newPassword, const Ice::Current &) override;


    int processUserTokenForId(std::string userToken);
    std::string processUserTokenForName(std::string userToken);


    Wt::Auth::AbstractUserDatabase &users();
    static const Wt::Auth::AuthService &auth();
    static const Wt::Auth::AbstractPasswordService &passwordAuth();

private:
    Wt::Dbo::Session session_;
    std::unique_ptr<UserDatabase> users_;



};