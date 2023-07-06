#include "include/AuthInterfaceI.h"

#include <Wt/Dbo/Exception.h>
#include <Wt/Dbo/SqlConnection.h>
#include <Wt/Dbo/FixedSqlConnectionPool.h>

#include <Ice/Ice.h>
#include <Wt/Dbo/Session.h>
#include <Wt/Dbo/ptr.h>
#include <Wt/Auth/HashFunction.h>
#include <Wt/Auth/PasswordService.h>
#include <Wt/Auth/PasswordStrengthValidator.h>
#include <Wt/Auth/PasswordVerifier.h>
#include <Wt/Auth/Dbo/AuthInfo.h>
#include <Wt/Auth/Token.h>

#include <Wt/Auth/GoogleService.h>
#include <Wt/Auth/FacebookService.h>


Wt::Auth::AuthService myAuthService;
Wt::Auth::PasswordService myPasswordService(myAuthService);

AuthInterfaceI::AuthInterfaceI(std::unique_ptr<dbo::SqlConnection> conn)
{
	session_.setConnection(std::move(conn));

	// Configure Auth
	myAuthService.setAuthTokensEnabled(true);
	myAuthService.setAuthTokenUpdateEnabled(false);
	myAuthService.setAuthTokenValidity(300);
	myAuthService.setEmailVerificationEnabled(true);
	myAuthService.setIdentityPolicy(Wt::Auth::IdentityPolicy::EmailAddress);

	std::unique_ptr<Wt::Auth::PasswordVerifier> verifier = std::make_unique<Wt::Auth::PasswordVerifier>();
	verifier->addHashFunction(std::make_unique<Wt::Auth::BCryptHashFunction>(7));

	myPasswordService.setVerifier(std::move(verifier));
	myPasswordService.setStrengthValidator(std::make_unique<Wt::Auth::PasswordStrengthValidator>());
	myPasswordService.setAttemptThrottlingEnabled(true);



	// Configure Session
	session_.mapClass<AuthInfo>("auth_info");
	session_.mapClass<AuthInfo::AuthIdentityType>("auth_identity");
	session_.mapClass<AuthInfo::AuthTokenType>("auth_token");

	session_.mapClass<User>("user");
	session_.mapClass<UserRole>("user_role");

	session_.mapClass<ProviderProfile>("provider_profile");
	session_.mapClass<ProviderService>("provider_service");
	session_.mapClass<ServiceGalery>("service_galery");

	session_.mapClass<ServiceAgeGroup>("service_age_group");
	session_.mapClass<ServiceType>("service_type");

	session_.mapClass<Event>("event");
	session_.mapClass<EventService>("event_service");
	

	session_.mapClass<Review>("review");
	session_.mapClass<ReviewGalery>("review_galery");

	users_ = std::make_unique<UserDatabase>(session_, &auth());
	users_->setMaxAuthTokensPerUser(5);

	dbo::Transaction transaction(session_);
	try
	{
		session_.createTables();
		// add user roles to the user_roles table
		Wt::log("info") << "\n\n" << "Database created | mesage from Auth session";
		std::unique_ptr<UserRole> adminRole{new UserRole()};
		std::unique_ptr<UserRole> clientRole{new UserRole()};
		std::unique_ptr<UserRole> organizerRole{new UserRole()};
		
		adminRole->role = Emlab::ADMINROLE;
		clientRole->role = Emlab::CLIENTROLE;
		organizerRole->role = Emlab::PROVIDERROLE;

		dbo::ptr<UserRole> adminRolePtr = session_.add(std::move(adminRole));
		dbo::ptr<UserRole> clientRolePtr = session_.add(std::move(clientRole));
		dbo::ptr<UserRole> organizerRolePtr = session_.add(std::move(organizerRole));

		// create a user
		Emlab::RegistrationInfo registrationInfo;
		registrationInfo.name = "client one";
		registrationInfo.email = "client1@gmail.com";
		registrationInfo.phone = "1234567890";
		registrationInfo.password = "asdfghj1";
		registerUser(registrationInfo, Ice::Current());

	}
	catch (dbo::Exception &e)
	{
		Wt::log("info") << "\n\n" << e.what();
	}
	catch (...)
	{
		Wt::log("info") << "\n\nother exception throw ------- mesage from AUTH session";
	}
	transaction.commit();


}


// Login
Emlab::LoginReturn AuthInterfaceI::loginUser(Emlab::LoginInfo loginInfo, const Ice::Current &)
{
	// RETURNED LoginReturnUserData Obj;
	Emlab::LoginReturn loginReturn;

	loginReturn.loginResponse = Emlab::LoginResponse::NotIdentified;
	loginReturn.userInfo.email = loginInfo.email;
	loginReturn.userInfo.darkMode = false;
	dbo::Transaction transaction(session_);

	auto user = users_->findWithIdentity(Wt::Auth::Identity::LoginName, loginInfo.email);	

	if (user.isValid())
	{
		auto userData = session_.find<User>().where("id = ?").bind(user.id()).resultValue();
		loginReturn.loginResponse = Emlab::LoginResponse::Identified;
		loginReturn.userInfo.name = userData->name;
		loginReturn.userInfo.phone = userData->phone;
		loginReturn.userInfo.darkMode = userData->darkMode;
		loginReturn.userInfo.role = userData->role->role;
		std::cout << "\n\n ------------------ USER FOUND ------------------ \n\n";
		auto passwordResult = myPasswordService.verifyPassword(user, loginInfo.password);

		if (passwordResult == Wt::Auth::PasswordResult::PasswordInvalid)
		{
			std::cout << "\n\n ------------------ INCORRECT PASSWORD ------------------ \n\n";
			loginReturn.loginResponse = Emlab::LoginResponse::IncorectPassword;
		}
		else if (passwordResult == Wt::Auth::PasswordResult::LoginThrottling)
		{
			std::cout << "\n\n ------------------ THROTTLING ACTIVATED ------------------ \n\n";
			loginReturn.loginResponse = Emlab::LoginResponse::ThrottlingActivated;
		}
		else if (passwordResult == Wt::Auth::PasswordResult::PasswordValid)
		{
			std::cout << "\n\n ------------------ PASSWORD VALID ------------------ \n\n";
			auto token_string = myAuthService.createAuthToken(user);
			loginReturn.token = token_string;
			loginReturn.loginResponse = Emlab::LoginResponse::LoggedIn;
		}
	}
	else
	{
		std::cout << "\n\n ------------------ USER NOT FOUND ------------------ \n\n";
	}

	transaction.commit();
	std::cout << "\n\n\n ------------------ RETURNING LOGIN RETURN ------------------ \n\n";
	std::cout << "name: " << loginReturn.userInfo.name << "\n";
	std::cout << "phone: " << loginReturn.userInfo.phone << "\n";
	std::cout << "email: " << loginReturn.userInfo.email << "\n";
	std::cout << "role: " << loginReturn.userInfo.role << "\n";
	std::cout << "token: " << loginReturn.token << "\n";
	// std::cout << "darkMode: " << loginReturn.darkMode << "\n";
	switch(loginReturn.loginResponse)
	{
		case Emlab::LoginResponse::NotIdentified:
			std::cout << "loginResponse: NotIdentified\n";
			break;
		case Emlab::LoginResponse::Identified:
			std::cout << "loginResponse: Identified\n";
			break;
		case Emlab::LoginResponse::IncorectPassword:
			std::cout << "loginResponse: IncorectPassword\n";
			break;
		case Emlab::LoginResponse::ThrottlingActivated:
			std::cout << "loginResponse: ThrottlingActivated\n";
			break;
		case Emlab::LoginResponse::LoggedIn:
			std::cout << "loginResponse: LoggedIn\n";
			break;
		default:
			std::cout << "loginResponse: default\n";
			break;
	}
	return loginReturn;
}

// Registration
Emlab::RegistrationResponse AuthInterfaceI::registerUser(Emlab::RegistrationInfo registrationInfo, const Ice::Current &)
{
	std::cout << "\n\n ------------------ REGISTER USER ------------------ \n\n";
	dbo::Transaction transaction(session_);

	auto userFoundByEmail = users_->findWithEmail(registrationInfo.email);

	if (userFoundByEmail.isValid())
	{
		std::cout << "\n\n User Email Alredy exists in dbo \n\n";
		return Emlab::RegistrationResponse::EmailAlreadyExists;
	}

	// Create a new User in User table
	std::unique_ptr<User> newUserPtr{new User()};
	newUserPtr->name = registrationInfo.name;
	newUserPtr->phone = registrationInfo.phone;
	newUserPtr->photo = registrationInfo.photo;
	newUserPtr->joinDate = Wt::WDateTime::currentDateTime().toTimePoint();
	newUserPtr->darkMode = false;
	newUserPtr->role = session_.find<UserRole>().where("role = ?").bind(Emlab::CLIENTROLE).resultValue();
	
	if(!registrationInfo.photo.empty())
		newUserPtr->photo = registrationInfo.photo;
	dbo::ptr<User> userPtr = session_.add(std::move(newUserPtr));

	// Crteate a new AuthInfo record and returns a Wt::Auth::User
	auto newUser = users_->registerNew();
	users_->setIdentity(newUser, Wt::Auth::Identity::LoginName, registrationInfo.email);
	newUser.setEmail(registrationInfo.email);
	auto userInfo = users_->find(newUser);

	userInfo.modify()->setUser(userPtr);
	// newUser.setIdentity(Wt::Auth::Identity::LoginName, registrationInfo.email);
	myPasswordService.updatePassword(newUser, registrationInfo.password);
	transaction.commit();
	std::cout << "\n\n ------------------ REGISTER USER DONE ------------------ \n\n";

	return Emlab::RegistrationResponse::RegistrationSuccessful;
}

// get user id or name from token
std::string AuthInterfaceI::processUserTokenForName(std::string userToken)
{
	std::string userName;
	dbo::Transaction transaction(session_);
	auto userResultToken = myAuthService.processAuthToken(userToken, users());
	auto user = users_->find(userResultToken.user());
	userName = user->user()->name;
	transaction.commit();
	return userName;
}

int AuthInterfaceI::processUserTokenForId(std::string userToken)
{
	dbo::Transaction transaction(session_);

	auto userResultToken = myAuthService.processAuthToken(userToken, users());
	auto user = users_->find(userResultToken.user());
	int userId = user->user()->id();

	transaction.commit();

	return userId;
}

int AuthInterfaceI::processUserIdentityForId(std::string userIdentity)
{
	int userId = 0;
	dbo::Transaction transaction(session_);

	auto user = users_->findWithEmail(userIdentity);
	userId = std::stoi(user.id());

	transaction.commit();
	return userId;
}

// Change Password
Emlab::ChangePasswordResponse AuthInterfaceI::changePassword(std::string userToken, std::string oldPassword, std::string newPassword, const Ice::Current &)
{
	Emlab::ChangePasswordResponse changePasswordResponse = Emlab::ChangePasswordResponse::PasswordNotChanged;

	dbo::Transaction transaction(session_);
	auto userId = processUserTokenForId(userToken);
	auto user = users_->findWithId(std::to_string(userId));
	if (!user.isValid())
	{
		std::cout << "\n\n ------------------ USER NOT FOUND ------------------ \n\n";
		return changePasswordResponse;
	}
	auto passwordResult = myPasswordService.verifyPassword(user, oldPassword);

	if (passwordResult == Wt::Auth::PasswordResult::PasswordInvalid)
	{
		std::cout << "\n\n ------------------ PASSWORD INVALID ------------------ \n\n";
		changePasswordResponse = Emlab::ChangePasswordResponse::OldPasswordIncorrect;
	}
	else if (passwordResult == Wt::Auth::PasswordResult::PasswordValid)
	{
		std::cout << "\n\n ------------------ PASSWORD VALID  and changing password------------------ \n\n";
		myPasswordService.updatePassword(user, newPassword);
		changePasswordResponse = Emlab::ChangePasswordResponse::PasswordChanged;
	}

	transaction.commit();

	return changePasswordResponse;
}


Emlab::ChangeUniqueDataResponse AuthInterfaceI::changeEmail(std::string userToken, std::string newEmail, const Ice::Current &)
{
	std::cout << "\n\n change email started \n";
	Emlab::ChangeUniqueDataResponse changeEmailResponse;
	dbo::Transaction transaction(session_);
	auto userFoundByEmail = users_->findWithEmail(newEmail);
	if (userFoundByEmail.isValid())
	{
		std::cout << "\n\n User Email Alredy exists in dbo \n\n";
		return Emlab::ChangeUniqueDataResponse::AllreadyExists;
	}
	std::cout << "\n\nchange email PASSED checking if other accounts have the same email\n\n";
	auto user = myAuthService.processAuthToken(userToken, users()).user();

	if(!user.isValid()){
		std::cout << "\n\nchange email USER NOT VALID \n\n";
		return Emlab::ChangeUniqueDataResponse::NotChanged;
	}
	users_->setEmail(user, newEmail);
	users_->setIdentity(user, Wt::Auth::Identity::LoginName, newEmail);
	changeEmailResponse = Emlab::ChangeUniqueDataResponse::Changed;
	std::cout << "\n\n before transaction comit \n\n";
	transaction.commit();
	std::cout << "\n\n after transaction comit \n\n";
	return changeEmailResponse;
}

Emlab::ChangeUniqueDataResponse AuthInterfaceI::changePhone(std::string userToken, std::string newPhone, const Ice::Current &)
{
	Emlab::ChangeUniqueDataResponse changePhoneResponse;
	dbo::Transaction transaction(session_);
	auto user = session_.find<User>().where("id = ?").bind(processUserTokenForId(userToken)).resultValue();
	if (!user)
	{
		std::cout << "\n\n ------------------ USER NOT FOUND ------------------ \n\n";
		return changePhoneResponse;
	}
	user.modify()->phone = newPhone;
	changePhoneResponse = Emlab::ChangeUniqueDataResponse::Changed;
	transaction.commit();
	return changePhoneResponse;
}

void AuthInterfaceI::setName(std::string userToken, std::string newName, const Ice::Current &)
{

	dbo::Transaction transaction(session_);
	auto userResult = myAuthService.processAuthToken(userToken, users()).user();

	if (!userResult.isValid()){
		std::cout << "\n\n ------------------ USER NOT FOUND ------------------ \n\n";
	}
	
	auto user = users_->find(userResult)->user();
	user.modify()->name = newName;
	transaction.commit();
}
    
void AuthInterfaceI::setDarkMode(std::string userToken, bool darkMode, const Ice::Current &)
{
	dbo::Transaction transaction(session_);
	auto user = session_.find<User>().where("id = ?").bind(processUserTokenForId(userToken)).resultValue();
	if (!user)
	{
		std::cout << "\n\n ------------------ USER NOT FOUND ------------------ \n\n";
		return;
	}
	user.modify()->darkMode = darkMode;
	transaction.commit();
}

void AuthInterfaceI::setUserPhoto(std::string userToken, Emlab::ImageData photo, const Ice::Current &)
{
	dbo::Transaction transaction(session_);
	auto userId = processUserTokenForId(userToken);
	auto user = session_.find<User>().where("id = ?").bind(userId).resultValue();
	if (!user)
	{
		std::cout << "\n\n ------------------ USER NOT FOUND ------------------ \n\n";
		return;
	}
	user.modify()->photo = photo;
	transaction.commit();
}

// Get User Info
Emlab::ImageData AuthInterfaceI::getUserPhotoWithEmail(std::string userEmail, const Ice::Current &)
{
	dbo::Transaction transaction(session_);
	auto user = users_->findWithEmail(userEmail);
	if (!user.isValid())
	{
		std::cout << "\n\n ------------------ USER NOT FOUND ------------------ \n\n";
		return Emlab::ImageData();
	}
	auto userData = session_.find<User>().where("id = ?").bind(user.id()).resultValue();
	transaction.commit();
	return userData->photo;
}

Emlab::ImageData AuthInterfaceI::getUserPhotoWithToken(std::string userToken, const Ice::Current &)
{
	dbo::Transaction transaction(session_);
	auto userId = processUserTokenForId(userToken);
	auto user = session_.find<User>().where("id = ?").bind(userId).resultValue();
	if (!user)
	{
		std::cout << "\n\n ------------------ USER NOT FOUND ------------------ \n\n";
		return Emlab::ImageData();
	}
	transaction.commit();
	return user->photo;
}

// ...
Wt::Auth::AbstractUserDatabase &AuthInterfaceI::users()
{
	return *users_;
}

const Wt::Auth::AuthService &AuthInterfaceI::auth()
{
	return myAuthService;
}

const Wt::Auth::AbstractPasswordService &AuthInterfaceI::passwordAuth()
{
	return myPasswordService;
}

void AuthInterfaceI::setUserRole(std::string userId, std::string userRole)
{
	std::cout << "\n\n setUserRole() started \n\n";
	Wt::Dbo::Transaction transaction(session_);


	auto user = users_->findWithId(userId);
	if(!user.isValid())
	{
		std::cout << "\n\n ------------------ USER NOT FOUND ------------------ \n\n";
		return;
	}
	auto authInfo = users_->find(user);
	auto userPtr = authInfo->user();
	auto role = session_.find<UserRole>().where("role = ?").bind(userRole);
	userPtr.modify()->role = role;
	
	transaction.commit();
	std::cout << "\n\n setUserRole() ended \n\n";
}
