#include "include/AuthInterfaceI.h"

namespace
{
	Wt::Auth::AuthService myAuthService;
	Wt::Auth::PasswordService myPasswordService(myAuthService);
}

AuthInterfaceI::AuthInterfaceI(std::unique_ptr<dbo::SqlConnection> conn)
{
	session_.setConnection(std::move(conn));

	// Configure Auth
	myAuthService.setAuthTokensEnabled(true);
	myAuthService.setAuthTokenUpdateEnabled(false);
	myAuthService.setAuthTokenValidity(300);

	std::unique_ptr<Wt::Auth::PasswordVerifier> verifier = std::make_unique<Wt::Auth::PasswordVerifier>();
	verifier->addHashFunction(std::make_unique<Wt::Auth::BCryptHashFunction>(7));

	myPasswordService.setVerifier(std::move(verifier));
	myPasswordService.setStrengthValidator(std::make_unique<Wt::Auth::PasswordStrengthValidator>());
	myPasswordService.setAttemptThrottlingEnabled(true);

	// Configure Session
	session_.mapClass<Event>("event");
	session_.mapClass<Client>("client");
	session_.mapClass<Service>("service");
	session_.mapClass<UserService>("user_service");

	session_.mapClass<UserRole>("user_role");
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
		// add user roles to the user_roles table
		Wt::log("info") << "Database created | mesage from Event session";
		std::unique_ptr<UserRole> adminRole{new UserRole()};
		std::unique_ptr<UserRole> clientRole{new UserRole()};
		std::unique_ptr<UserRole> organizerRole{new UserRole()};
		
		adminRole->role = Emlab::ADMIN;
		clientRole->role = Emlab::CLIENT;
		organizerRole->role = Emlab::PROVIDER;

		dbo::ptr<UserRole> adminRolePtr = session_.add(std::move(adminRole));
		dbo::ptr<UserRole> clientRolePtr = session_.add(std::move(clientRole));
		dbo::ptr<UserRole> organizerRolePtr = session_.add(std::move(organizerRole));

	}
	catch (dbo::Exception &e)
	{
		Wt::log("info") << e.what();
	}
	catch (...)
	{
		Wt::log("info") << "other exception throw ------- mesage from AUTH session";
	}
	transaction.commit();
}

LoginReturn AuthInterfaceI::loginUser(LoginInfo loginInfo, const Ice::Current &)
{
	// RETURNED LoginReturnUserData Obj;
	LoginReturn loginReturn;
	loginReturn.loginResponse = LoginResponse::NotIdentified;
	loginReturn.name = "";
	loginReturn.email = loginInfo.email;
	loginReturn.token = "";

	dbo::Transaction transaction(session_);

	auto user = users().findWithEmail(loginInfo.email);

	if (user.isValid())
	{
		auto userData = session_.find<User>().where("id = ?").bind(user.id()).resultValue();
		loginReturn.loginResponse = LoginResponse::Identified;
		loginReturn.photo = userData->photo;
		loginReturn.name = userData->name;
		loginReturn.role = userData->role->role;

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

RegistrationResponse AuthInterfaceI::registerUser(RegistrationInfo registrationInfo, const Ice::Current &)
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
	newUserPtr->photo = registrationInfo.photo;

	if(registrationInfo.role == Emlab::CLIENT){
		auto rolePtr = session_.find<UserRole>().where("role = ?").bind(Emlab::CLIENT);
		newUserPtr->role = rolePtr;
	}else if (registrationInfo.role == Emlab::PROVIDER){
		auto rolePtr = session_.find<UserRole>().where("role = ?").bind(Emlab::PROVIDER);
		newUserPtr->role = rolePtr;
	}else if (registrationInfo.role == Emlab::ADMIN){
		std::cout << "\n\n you cannot create a user with Admin Role privilages \n\n";
	}else {
		std::cout << "\n\n i dont know, something is broken in registerNewUser \n\n";
	}
	dbo::ptr<User> userPtr = session_.add(std::move(newUserPtr));

	// Crteate a new AuthInfo record and returns a Wt::Auth::User
	auto newUser = users_->registerNew();
	auto userInfo = users_->find(newUser);
	userInfo.modify()->setUser(userPtr);
	newUser.setIdentity(Wt::Auth::Identity::LoginName, registrationInfo.email);
	newUser.setEmail(registrationInfo.email);
	myPasswordService.updatePassword(newUser, registrationInfo.password);
	transaction.commit();

	return Emlab::RegistrationResponse::RegistrationSuccessful;
}

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

ChangePasswordResponse AuthInterfaceI::changePassword(std::string userToken, std::string oldPassword, std::string newPassword, const Ice::Current &)
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
	auto passwordResult = myPasswordService.verifyPassword(user, oldPassword);

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
		myPasswordService.updatePassword(user, newPassword);
		changePasswordResponse = ChangePasswordResponse::PasswordChanged;
	}

	transaction.commit();

	return changePasswordResponse;
}

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

