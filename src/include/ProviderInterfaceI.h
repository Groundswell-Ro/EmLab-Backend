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

		virtual int registerProvider(string userToken, ProviderProfileInfo providerProfileInfo, const Ice::Current &) override;
		virtual int registerProviderService(string userToken, ProviderServiceInfo providerServiceInfo, const Ice::Current &) override;
		virtual void addPhotoToService(string userToken, int serviceId, ImageInfo imageInfo, const Ice::Current &) override;

		virtual ImageData getProviderProfilePhoto(int profileId, const Ice::Current &) override;
        virtual Emlab::ProviderProfileInfo getProfileAsUser(string userToken, const Ice::Current &) override;
        virtual Emlab::ProviderProfileInfo getProfileAsClient(int profileId, const Ice::Current &) override;
		virtual SeqProviderServiceInfo getSeqProviderServices(int profileId, const Ice::Current &) override;
		virtual ImageInfo getProviderServicePhoto(int servicePhotoId, const Ice::Current &) override;

private:
	Wt::Dbo::Session session_;
	std::shared_ptr<AuthInterfaceI> authInterface_;
};
