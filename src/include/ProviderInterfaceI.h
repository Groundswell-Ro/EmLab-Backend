#pragma once
#include "AuthInterfaceI.h"
#include "../../../comunication/comm/ProviderInterface.h"

#include <Wt/Dbo/Session.h>

using namespace std;
using namespace Emlab;

class ProviderInterfaceI : public Emlab::ProviderInterface
{
public:
	ProviderInterfaceI(std::unique_ptr<dbo::SqlConnection> conn, std::shared_ptr<AuthInterfaceI> authInterface);

		virtual void setProviderProfile(string email, ProfileData profileData, const Ice::Current &) override;

        virtual ProviderProfileInfo getProfileInfo(string email, const Ice::Current &) override;
		virtual ProfileData getProfileData(string email, const Ice::Current &) override;
private:
	Wt::Dbo::Session session_;
	std::shared_ptr<AuthInterfaceI> authInterface_;
};
