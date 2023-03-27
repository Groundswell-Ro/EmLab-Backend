#include "include/AuthSession.h"
#include <Wt/WApplication.h>
#include "Wt/Auth/AuthService.h"
#include "Wt/Auth/HashFunction.h"
#include "Wt/Auth/PasswordService.h"
#include "Wt/Auth/PasswordStrengthValidator.h"
#include "Wt/Auth/PasswordVerifier.h"
#include "Wt/Auth/GoogleService.h"
#include "Wt/Auth/FacebookService.h"
#include "Wt/Auth/Dbo/AuthInfo.h"
#include <Wt/Auth/Token.h>
#include <Wt/WRandom.h>

#include <Wt/Dbo/Exception.h>

#include <Wt/Dbo/FixedSqlConnectionPool.h>

using namespace Wt;

namespace dbo = Wt::Dbo;

namespace
{
	Auth::AuthService myAuthService;
	Auth::PasswordService myPasswordService(myAuthService);
}

AuthSession::AuthSession(std::unique_ptr<dbo::SqlConnection> conn)
{
	session_.setConnection(std::move(conn));
	configureAuth();
	configureSession();
}

AuthSession::~AuthSession()
{
}

void AuthSession::configureAuth()
{
	myAuthService.setAuthTokensEnabled(true);
	myAuthService.setAuthTokenUpdateEnabled(false);
	myAuthService.setAuthTokenValidity(300);

	std::unique_ptr<Auth::PasswordVerifier> verifier = std::make_unique<Auth::PasswordVerifier>();
	verifier->addHashFunction(std::make_unique<Auth::BCryptHashFunction>(7));

	myPasswordService.setVerifier(std::move(verifier));
	myPasswordService.setStrengthValidator(std::make_unique<Auth::PasswordStrengthValidator>());
	myPasswordService.setAttemptThrottlingEnabled(true);
}

void AuthSession::configureSession()
{
	session_.mapClass<Event>("event");
	session_.mapClass<Client>("client");
	session_.mapClass<Service>("service");
	session_.mapClass<UserService>("user_service");

	session_.mapClass<User>("user");
	session_.mapClass<AuthInfo>("auth_info");
	session_.mapClass<AuthInfo::AuthIdentityType>("auth_identity");
	session_.mapClass<AuthInfo::AuthTokenType>("auth_token");

	users_ = std::make_unique<UserDatabase>(session_);
	users_->setMaxAuthTokensPerUser(5);

	dbo::Transaction transaction(session_);
	try
	{
		session_.createTables();
		log("info") << "Database created | mesage from Event session";
	}
	catch (dbo::Exception &e)
	{
		log("info") << e.what();
	}
	catch (...)
	{
		log("info") << "other exception throw ------- mesage from AUTH session";
	}
	transaction.commit();
}

RegistrationResponse AuthSession::registerNewUser(RegistrationInfo registrationInfo)
{
	dbo::Transaction transaction(session_);

	auto userFoundByEmail = users_->findWithEmail(registrationInfo.email);

	if (userFoundByEmail.isValid())
	{
		std::cout << "\n\n User Email Alredy exists in dbo \n\n";
		return RegistrationResponse::EmailAlreadyExists;
	}

	// Create a new User in User table
	std::unique_ptr<User> newUserPtr{new User()};
	newUserPtr->name = registrationInfo.name;
	newUserPtr->phone = registrationInfo.phone;
	dbo::ptr<User> userPtr = session_.add(std::move(newUserPtr));

	// Crteate a new AuthInfo record and returns a Wt::Auth::User
	auto newUser = users_->registerNew();
	auto userInfo = users_->find(newUser);
	userInfo.modify()->setUser(userPtr);
	newUser.setIdentity(Auth::Identity::LoginName, registrationInfo.email);
	newUser.setEmail(registrationInfo.email);
	myPasswordService.updatePassword(newUser, registrationInfo.password);
	transaction.commit();

	return AuthModule::RegistrationResponse::RegistrationSuccessful;
}

LoginReturn AuthSession::logUserIn(LoginInfo loginInfo)
{
	// RETURNED LoginReturnUserData Obj;
	LoginReturn loginReturn;
	loginReturn.loginResponse = LoginResponse::NotIdentified;
	loginReturn.name = "";
	loginReturn.token = "";

	dbo::Transaction transaction(session_);

	auto user = users().findWithEmail(loginInfo.email);

	if (user.isValid())
	{
		loginReturn.loginResponse = LoginResponse::Identified;
		loginReturn.name = session_.find<User>().where("id = ?").bind(user.id()).resultValue()->name;

		auto passwordResult = myPasswordService.verifyPassword(user, loginInfo.password);

		if (passwordResult == Wt::Auth::PasswordResult::PasswordInvalid)
		{
			loginReturn.loginResponse = LoginResponse::IncorectPassword;
		}
		else if (passwordResult == Wt::Auth::PasswordResult::LoginThrottling)
		{
			loginReturn.loginResponse = LoginResponse::ThrottlingActivated;
		}
		else if (passwordResult == Wt::Auth::PasswordResult::PasswordValid)
		{
			loginReturn.token = auth().createAuthToken(user);
			std::cout << "\n\n ------------------ processUserTokenForServices ------------------ \n\n";
			loginReturn.servicesInfoSq = processUserTokenForServices(loginReturn.token);
			std::cout << "\n\n ------------------ processUserTokenForServices ------------------ \n\n";
			loginReturn.loginResponse = LoginResponse::LoggedIn;
		}
	}
	else
	{
		std::cout << "\n\n ------------------ USER NOT FOUND ------------------ \n\n";
	}

	transaction.commit();
	return loginReturn;
}

AuthModule::ServicesInfoSq AuthSession::processUserTokenForServices(Wt::WString userToken)
{
	AuthModule::ServicesInfoSq servicesInfoSq;

	dbo::Transaction transaction(session_);

	auto userResultToken = myAuthService.processAuthToken(userToken.toUTF8(), users());

	auto user = users_->find(userResultToken.user());
	auto userServices = user->user()->userServices;

	for (Wt::Dbo::ptr<UserService> &userService : userServices)
	{
		AuthModule::ServiceInfo structUserServiceInfo;

		structUserServiceInfo.id = userService.id();
		structUserServiceInfo.title = userService->title;
		structUserServiceInfo.description = userService->description;
		structUserServiceInfo.price = std::stoi(userService->price);

		servicesInfoSq.push_back(structUserServiceInfo);
	}

	transaction.commit();

	return servicesInfoSq;
}

AuthModule::UserServices AuthSession::processUserEmailForServices(Wt::WString email)
{
	AuthModule::UserServices userServices;

	dbo::Transaction transaction(session_);

	auto user = users_->findWithEmail(email.toUTF8());

	if (user.isValid())
	{
		dbo::ptr<User> userPtr = session_.find<User>().where("id = ?").bind(user.id());

		auto userServicesPtr = userPtr->userServices;

		for (Wt::Dbo::ptr<UserService> &userService : userServicesPtr)
		{
			AuthModule::ServiceInfo serviceInfo;

			serviceInfo.id = userService.id();
			serviceInfo.title = userService->title;
			serviceInfo.description = userService->description;
			serviceInfo.price = std::stoi(userService->price);

			userServices.servicesInfoSq.push_back(serviceInfo);
		}
	}
	return userServices;
}

Wt::WString AuthSession::processUserTokenForName(Wt::WString userToken)
{
	std::string userName;

	dbo::Transaction transaction(session_);

	auto userResultToken = myAuthService.processAuthToken(userToken.toUTF8(), users());

	auto user = users_->find(userResultToken.user());
	userName = user->user()->name;

	transaction.commit();

	return userName;
}

Auth::AbstractUserDatabase &AuthSession::users()
{
	return *users_;
}

const Auth::AuthService &AuthSession::auth()
{
	return myAuthService;
}

const Auth::AbstractPasswordService &AuthSession::passwordAuth()
{
	return myPasswordService;
}

int AuthSession::processUserTokenForId(Wt::WString userToken)
{
	dbo::Transaction transaction(session_);

	auto userResultToken = myAuthService.processAuthToken(userToken.toUTF8(), users());
	auto user = users_->find(userResultToken.user());
	int userId = user->user()->id();

	transaction.commit();

	return userId;
}

ChangePasswordResponse AuthSession::changeUserPassword(Wt::WString userToken, Wt::WString oldPassword, Wt::WString newPassword)
{
	ChangePasswordResponse changePasswordResponse = ChangePasswordResponse::PasswordNotChanged;

	dbo::Transaction transaction(session_);
	auto userId = processUserTokenForId(userToken);
	auto user = users_->findWithId(std::to_string(userId));
	if (!user.isValid())
	{
		std::cout << "\n\n ------------------ USER NOT FOUND ------------------ \n\n";
		return changePasswordResponse;
	}
	auto passwordResult = myPasswordService.verifyPassword(user, oldPassword.toUTF8());

	if (passwordResult == Wt::Auth::PasswordResult::PasswordInvalid)
	{
		std::cout << "\n\n ------------------ PASSWORD INVALID ------------------ \n\n";
		changePasswordResponse = ChangePasswordResponse::OldPasswordIncorrect;
	}
	else if (passwordResult == Wt::Auth::PasswordResult::LoginThrottling)
	{
		std::cout << "\n\n ------------------ THROTTLING ACTIVATED ------------------ \n\n";
		changePasswordResponse = ChangePasswordResponse::ThrottlingActivated;
	}
	else if (passwordResult == Wt::Auth::PasswordResult::PasswordValid)
	{
		std::cout << "\n\n ------------------ PASSWORD VALID  and changing password------------------ \n\n";
		myPasswordService.updatePassword(user, newPassword.toUTF8());
		changePasswordResponse = ChangePasswordResponse::PasswordChanged;
	}

	transaction.commit();

	return changePasswordResponse;
}

void AuthSession::addUserService(Wt::WString userToken, AuthModule::ServiceInfo structServiceInfo)
{
	dbo::Transaction transaction(session_);
	auto userId = processUserTokenForId(userToken);
	dbo::ptr<User> user = session_.find<User>().where("id = ?").bind(userId);

	dbo::ptr<UserService> userService = session_.add(std::make_unique<UserService>());
	userService.modify()->title = structServiceInfo.title;
	userService.modify()->description = structServiceInfo.description;
	userService.modify()->price = std::to_string(structServiceInfo.price);
	userService.modify()->user = user;

	transaction.commit();
}

void AuthSession::removeUserService(Wt::WString userToken, int serviceId)
{
	dbo::Transaction transaction(session_);
	auto userId = processUserTokenForId(userToken);
	dbo::ptr<User> user = session_.find<User>().where("id = ?").bind(userId);

	dbo::ptr<UserService> userService = session_.find<UserService>().where("id = ?").bind(serviceId);
	userService.remove();

	transaction.commit();
}

void AuthSession::updateUserService(Wt::WString userToken, AuthModule::ServiceInfo structServiceInfo)
{
	std::cout << "\n\n ------------------ updateUserService ------------------ \n\n";
	std::cout << structServiceInfo.id << "\n";
	std::cout << structServiceInfo.title << "\n";
	std::cout << structServiceInfo.description << "\n";
	std::cout << structServiceInfo.price << "\n\n";

	dbo::Transaction transaction(session_);
	auto userId = processUserTokenForId(userToken);
	dbo::ptr<User> user = session_.find<User>().where("id = ?").bind(userId);

	dbo::ptr<UserService> userService = session_.find<UserService>().where("id = ?").bind(structServiceInfo.id);

	userService.modify()->title = structServiceInfo.title;
	userService.modify()->description = structServiceInfo.description;
	userService.modify()->price = std::to_string(structServiceInfo.price);

	transaction.commit();
}