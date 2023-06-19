#include "include/ClientInterfaceI.h"

ClientInterfaceI::ClientInterfaceI(std::unique_ptr<dbo::SqlConnection> conn, std::shared_ptr<AuthInterfaceI> authInterface)
	: authInterface_(authInterface)
{
	session_.setConnection(std::move(conn));
	session_.mapClass<ProviderService>("provider_service");
	session_.mapClass<Client>("client");
	session_.mapClass<Service>("service");
	session_.mapClass<Event>("event");
	session_.mapClass<User>("user");
	session_.mapClass<UserRole>("user_role");
	session_.mapClass<Review>("review");

}


int ClientInterfaceI::addClient(std::string userToken, ClientInfo clientInfo, const Ice::Current &)
{
	int id = 0;
	std::cout << "\n\n addClient \n\n";
	return id;
}

void ClientInterfaceI::delClient(std::string userToken, int clientId, const Ice::Current &)
{
	std::cout << "\n\n delClient \n\n";
}

	
void ClientInterfaceI::modifyClinetName(std::string userToken, int clientId, std::string newName, const Ice::Current &)
{
	std::cout << "\n\n modifyClinetName \n\n";
}

void ClientInterfaceI::modifyClientPhone(std::string userToken, int clientId, std::string newPhone, const Ice::Current &)
{
	std::cout << "\n\n modifyClientPhone \n\n";
}

void ClientInterfaceI::modifyClientSpecialNotes(std::string userToken, int clientId, std::string newSpecialNotes, const Ice::Current &)
{
	std::cout << "\n\n modifyClientSpecialNotes \n\n";
}


SeqClientInfo ClientInterfaceI::getClientsByName(std::string userToken, std::string partialName, const Ice::Current &)
{
	SeqClientInfo seqClientInfo;
	std::cout << "\n\n getClientsByName \n\n";
	return seqClientInfo;
}

SeqClientInfo ClientInterfaceI::getClientsByPhone(std::string userToken, std::string partialPhone, const Ice::Current &)
{
	SeqClientInfo seqClientInfo;
	std::cout << "\n\n getClientsByPhone \n\n";
	return seqClientInfo;
}

