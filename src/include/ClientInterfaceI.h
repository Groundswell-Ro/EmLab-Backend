#pragma once
#include "AuthInterfaceI.h"
#include "../../../comunication/comm/ClientInterface.h"

class ClientInterfaceI : public Emlab::ClientInterface
{
public:
	ClientInterfaceI(std::unique_ptr<dbo::SqlConnection> conn, std::shared_ptr<AuthInterfaceI> authInterface);

	virtual int addClient(std::string userToken, Emlab::ClientInfo clientInfo, const Ice::Current &) override;
	virtual void delClient(std::string userToken, int clientId, const Ice::Current &) override;
	
	virtual void modifyClinetName(std::string userToken, int clientId, std::string newName, const Ice::Current &) override;
	virtual void modifyClientPhone(std::string userToken, int clientId, std::string newPhone, const Ice::Current &) override;
	virtual void modifyClientSpecialNotes(std::string userToken, int clientId, std::string newSpecialNotes, const Ice::Current &) override;

	virtual Emlab::SeqClientInfo getClientsByName(std::string userToken, std::string partialName, const Ice::Current &) override;
	virtual Emlab::SeqClientInfo getClientsByPhone(std::string userToken, std::string partialPhone, const Ice::Current &) override;

private:
	Wt::Dbo::Session session_;
	std::shared_ptr<AuthInterfaceI> authInterface_;
};
