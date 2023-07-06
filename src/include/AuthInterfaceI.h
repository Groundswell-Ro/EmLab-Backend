#pragma once
#include "../../../comunication/comm/AuthInterface.h"
#include "DboTables.h"
#include <Wt/Auth/AuthService.h>
#include <Wt/Auth/Dbo/UserDatabase.h>
#include <Wt/Auth/PasswordService.h>

using UserDatabase = Wt::Auth::Dbo::UserDatabase<AuthInfo>;

class AuthInterfaceI : public Emlab::AuthInterface
{
public:
	AuthInterfaceI(std::unique_ptr<dbo::SqlConnection> conn);

	virtual Emlab::LoginReturn loginUser(Emlab::LoginInfo loginInfo, const Ice::Current &) override;
	virtual Emlab::RegistrationResponse registerUser(Emlab::RegistrationInfo registrationInfo, const Ice::Current &) override;
	
    
    virtual Emlab::ChangePasswordResponse changePassword(std::string userToken, std::string oldPassword, std::string newPassword, const Ice::Current &) override;
    virtual Emlab::ChangeUniqueDataResponse changeEmail(std::string userToken, std::string newEmail, const Ice::Current &) override;
    virtual Emlab::ChangeUniqueDataResponse changePhone(std::string userToken, std::string newPhone, const Ice::Current &) override;
    
    virtual void setName(std::string userToken, std::string newName, const Ice::Current &) override;
    virtual void setDarkMode(std::string userToken, bool darkMode, const Ice::Current &) override;
    virtual void setUserPhoto(std::string userToken, Emlab::ImageData photo, const Ice::Current &) override;

	virtual Emlab::ImageData getUserPhotoWithEmail(std::string userEmail, const Ice::Current &) override;
	virtual Emlab::ImageData getUserPhotoWithToken(std::string userToken, const Ice::Current &) override;

    int processUserTokenForId(std::string userToken);
    int processUserIdentityForId(std::string userIdentity);
    std::string processUserTokenForName(std::string userToken);

    Wt::Auth::AbstractUserDatabase &users();
    static const Wt::Auth::AuthService &auth();
    static const Wt::Auth::AbstractPasswordService &passwordAuth();
    void setUserRole(std::string userId, std::string userRole);
private:
    Wt::Dbo::Session session_;
    std::unique_ptr<UserDatabase> users_;

};