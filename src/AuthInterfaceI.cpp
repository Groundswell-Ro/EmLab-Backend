#include "include/AuthInterfaceI.h"

AuthInterfaceI::AuthInterfaceI(std::unique_ptr<dbo::SqlConnection> conn)
	: authSession_(std::move(conn))
{
}

LoginReturn AuthInterfaceI::loginUser(LoginInfo loginInfo, const Ice::Current &)
{
	return authSession_.logUserIn(loginInfo);
}

RegistrationResponse AuthInterfaceI::registerUser(RegistrationInfo registrationInfo, const Ice::Current &)
{
	return authSession_.registerNewUser(registrationInfo);
}

string AuthInterfaceI::getUserName(string userToken, const Ice::Current &)
{
	Wt::WString userTokenWS(userToken);
	auto userName = authSession_.processUserTokenForName(userTokenWS).toUTF8();
	return userName;
}

ChangePasswordResponse AuthInterfaceI::changePassword(string userToken, string oldPassword, string newPassword, const Ice::Current &)
{
	return authSession_.changeUserPassword(userToken, oldPassword, newPassword);
}

void AuthInterfaceI::addUserService(string userToken, ServiceInfo userServiceInfo, const Ice::Current &)
{
	authSession_.addUserService(userToken, userServiceInfo);
}

void AuthInterfaceI::removeUserService(string userToken, int userServiceId, const Ice::Current &)
{
	authSession_.removeUserService(userToken, userServiceId);
}

void AuthInterfaceI::updateUserService(string userToken, ServiceInfo userServiceInfo, const Ice::Current &)
{
	authSession_.updateUserService(userToken, userServiceInfo);
}

ServicesInfoSq AuthInterfaceI::getSelfServices(string userToken, const Ice::Current &)
{
	return authSession_.processUserTokenForServices(userToken);
}

AuthModule::UserServices AuthInterfaceI::getUserServicesByEmail(string email, const Ice::Current &)
{
	return authSession_.processUserEmailForServices(email);
}