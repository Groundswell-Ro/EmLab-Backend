#pragma once
#include "../../../comunication/AuthModule.h"

#include <Wt/Auth/Dbo/UserDatabase.h>

#include <Wt/Dbo/SqlConnection.h>

#include <Wt/WString.h>

#include <Wt/Dbo/Session.h>
#include <Wt/Dbo/ptr.h>
#include "DboTables.h"

namespace dbo = Wt::Dbo;
using namespace AuthModule;
using AuthInfo = Wt::Auth::Dbo::AuthInfo<User>;

// using AuthInfo = Wt::Auth::Dbo::AuthInfo<User>; defined in User.h
using UserDatabase = Wt::Auth::Dbo::UserDatabase<AuthInfo>;

class AuthSession
{
public:
    AuthSession(std::unique_ptr<dbo::SqlConnection> conn);
    ~AuthSession();

    static void configureAuth();
    void configureSession();


    // functions used by auth interface
    LoginReturn logUserIn(LoginInfo loginInfo);
    RegistrationResponse registerNewUser(RegistrationInfo registrationInfo);
    int processUserTokenForId(Wt::WString userToken);
    Wt::WString processUserTokenForName(Wt::WString userToken);

    ChangePasswordResponse changeUserPassword(Wt::WString userToken, Wt::WString oldPassword, Wt::WString newPassword);

    Wt::Auth::AbstractUserDatabase &users();
    static const Wt::Auth::AuthService &auth();
    static const Wt::Auth::AbstractPasswordService &passwordAuth();

private:
    mutable Wt::Dbo::Session session_;
    std::unique_ptr<UserDatabase> users_;
};
