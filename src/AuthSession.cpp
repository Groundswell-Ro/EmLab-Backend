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

StructRegistrationReturn AuthSession::tryRegisterNewUser(StructRegistrationInfo structRegistrationInfo)
{
	dbo::Transaction transaction(session_);

	// RETURNED RegistrationReturnUserData Object
	StructRegistrationReturn resultUser;
	resultUser.structLoginReturn.userName = structRegistrationInfo.userName;
	resultUser.structLoginReturn.loginResponse = LoginResponse::NotIdentified;
	resultUser.structLoginReturn.userToken = "";

	auto userAlreadyExist = users_->findWithEmail(structRegistrationInfo.structLoginInfo.userEmail);
	if (userAlreadyExist.isValid())
	{

		std::cout << "\n\n User Alredy exists in dbo \n\n";
		resultUser.registrationResponse = RegistrationResponse::UserEmailAlreadyExists;
		resultUser.structLoginReturn.loginResponse = LoginResponse::Identified;
		resultUser.structLoginReturn.userName = userAlreadyExist.identity(Auth::Identity::LoginName).toUTF8();

		return resultUser;
	}

	std::unique_ptr<User> newUserPtr{new User()};
	newUserPtr->name = structRegistrationInfo.userName;
	dbo::ptr<User> userPtr = session_.add(std::move(newUserPtr));

	// Crteate a new AuthInfo record and returns a Wt::Auth::User
	auto newUser = users_->registerNew();
	// Create a new User
	auto userInfo = users_->find(newUser);
	userInfo.modify()->setUser(userPtr);
	newUser.setIdentity(Auth::Identity::LoginName, structRegistrationInfo.structLoginInfo.userEmail);
	newUser.setEmail(structRegistrationInfo.structLoginInfo.userEmail);
	myPasswordService.updatePassword(newUser, structRegistrationInfo.structLoginInfo.userPassword);
	auto token = auth().createAuthToken(newUser);

	resultUser.structLoginReturn.loginResponse = LoginResponse::LoggedIn;
	resultUser.structLoginReturn.userToken = token;
	resultUser.registrationResponse = RegistrationResponse::UserRegistrationSuccessful;

	transaction.commit();

	return resultUser;
}

StructLoginReturn AuthSession::tryLoginUser(StructLoginInfo structLoginInfo)
{
	// // RETURNED LoginReturnUserData Obj;
	StructLoginReturn resultUser;
	resultUser.loginResponse = LoginResponse::NotIdentified;
	resultUser.userName = "";
	resultUser.userToken = "";

	dbo::Transaction transaction(session_);

	auto user = users().findWithEmail(structLoginInfo.userEmail);

	if (user.isValid())
	{
		resultUser.loginResponse = LoginResponse::Identified;
		resultUser.userName = session_.find<User>().where("id = ?").bind(user.id()).resultValue()->name;

		auto userToken = "";

		auto passwordResult = myPasswordService.verifyPassword(user, structLoginInfo.userPassword);
		if (passwordResult == Wt::Auth::PasswordResult::PasswordInvalid)
		{
			resultUser.userToken = Wt::WString(userToken).toUTF8();
			resultUser.loginResponse = LoginResponse::IncorectPassword;
		}
		else if (passwordResult == Wt::Auth::PasswordResult::LoginThrottling)
		{
			resultUser.loginResponse = LoginResponse::ThrottlingActivated;
		}
		else if (passwordResult == Wt::Auth::PasswordResult::PasswordValid)
		{
			auto userToken = auth().createAuthToken(user);
			resultUser.userToken = Wt::WString(userToken).toUTF8();
			resultUser.loginResponse = LoginResponse::LoggedIn;
		}
	}
	else
	{
		std::cout << "\n\n ------------------ USER NOT FOUND ------------------ \n\n";
	}
	transaction.commit();
	return resultUser;
}

AuthModule::ServiceInfoSq AuthSession::processUserTokenForServices(Wt::WString userToken)
{
	AuthModule::ServiceInfoSq serviceInfoSq;

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

		serviceInfoSq.push_back(structUserServiceInfo);
	}

	transaction.commit();

	return serviceInfoSq;
}

AuthModule::UserServices AuthSession::processUserNameForServices(Wt::WString userName)
{
	AuthModule::UserServices userServices;
	userServices.userName = userName.toUTF8();

	dbo::Transaction transaction(session_);

	auto user = session_.find<User>().where("name = ?").bind(userName.toUTF8()).resultValue();

	auto userServicesPtr = user->userServices;

	for (Wt::Dbo::ptr<UserService> &userService : userServicesPtr)
	{
		AuthModule::ServiceInfo structUserServiceInfo;

		structUserServiceInfo.id = userService.id();
		structUserServiceInfo.title = userService->title;
		structUserServiceInfo.description = userService->description;
		structUserServiceInfo.price = std::stoi(userService->price);

		userServices.userServices.push_back(structUserServiceInfo);
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

ChangePasswordResponse AuthSession::tryChangePassword(Wt::WString userToken, Wt::WString oldPassword, Wt::WString newPassword)
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