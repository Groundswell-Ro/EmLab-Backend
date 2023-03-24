#include "include/AuthInterfaceI.h"

AuthInterfaceI::AuthInterfaceI(std::unique_ptr<dbo::SqlConnection> conn)
	: authSession_(std::move(conn))
{
}

StructLoginReturn AuthInterfaceI::tryLogin(StructLoginInfo structLoginInfo, const Ice::Current &)
{
	StructLoginReturn userResponse = authSession_.tryLoginUser(structLoginInfo);
	return userResponse;
}

StructRegistrationReturn AuthInterfaceI::tryRegister(StructRegistrationInfo structRegistrationInfo, const Ice::Current &)
{
	StructRegistrationReturn userResponse;
	userResponse = authSession_.tryRegisterNewUser(structRegistrationInfo);
	return userResponse;
}

string AuthInterfaceI::getUserName(string userToken, const Ice::Current &)
{
	Wt::WString userTokenWS(userToken);
	auto userName = authSession_.processUserTokenForName(userTokenWS).toUTF8();
	return userName;
}

ChangePasswordResponse AuthInterfaceI::tryChangePassword(string userToken, string oldPassword, string newPassword, const Ice::Current &)
{
	return authSession_.tryChangePassword(userToken, oldPassword, newPassword);
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

AuthModule::ServiceInfoSq AuthInterfaceI::getSelfServices(string userToken, const Ice::Current &)
{
	return authSession_.processUserTokenForServices(userToken);
}

AuthModule::UserServices AuthInterfaceI::getUserServicesFromUserName(string userName, const Ice::Current &)
{
	return authSession_.processUserNameForServices(userName);
}