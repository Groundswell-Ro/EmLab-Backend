#include "include/AuthInterfaceI.h"
#include "include/EventsDataInterfaceI.h"

#include <Wt/Dbo/FixedSqlConnectionPool.h>
#include <Wt/Dbo/backend/Postgres.h>

#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{

	ifstream dboConnFile("/etc/evntmgr/dbo-connection.txt");
	ifstream connFile("comunication.txt");

	if (!dboConnFile)
	{
		std::cout << "\n\n - ERROR - Give me the DBO connection file and i will work :) - \n\n";
		return 0;
	}
	std::string dboConnString((istreambuf_iterator<char>(dboConnFile)), istreambuf_iterator<char>());

	if (!connFile)
	{
		std::cout << "\n\n - ERROR - Give me the connection file ICE and i will work :) - \n\n";
		return 0;
	}
	std::string comAdapter;
	std::string comConnection;

	getline(connFile, comAdapter);
	getline(connFile, comConnection);

	// std::cout << "\n\n"
	// 		  << dboConnString << "\n\n";
	// std::cout << comAdapter << " ------ " << comConnection << "\n\n";

	/*
	Create Connection pool to a dbo and gives every interface a connection
	*/
	auto myPQconnectdb = std::make_unique<dbo::backend::Postgres>(dboConnString);
	myPQconnectdb->setProperty("show-queries", "true");
	auto fixedConnPool = std::make_shared<dbo::FixedSqlConnectionPool>(std::move(myPQconnectdb), 2);

	try
	{
		Ice::CommunicatorHolder ich(argc, argv);
		auto adapter = ich->createObjectAdapterWithEndpoints(comAdapter, comConnection);

		// Authentification Interface HERE
		auto authServant = make_shared<AuthInterfaceI>(std::move(fixedConnPool->getConnection()));

		// Events Data Interface HERE
		auto eventDataServant = make_shared<EventsDataInterfaceI>(std::move(fixedConnPool->getConnection()), authServant);

		// Add Servants to the adaptor
		adapter->add(authServant, Ice::stringToIdentity("Authentification"));
		adapter->add(eventDataServant, Ice::stringToIdentity("EventData"));

		adapter->activate();
		ich->waitForShutdown();
	}
	catch (const std::exception &e)
	{
		cerr << e.what() << endl;
		return 1;
	}
	return 0;
}
