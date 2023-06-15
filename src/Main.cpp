#include "include/AuthInterfaceI.h"
#include "include/EventInterfaceI.h"

#include <Wt/Dbo/FixedSqlConnectionPool.h>
#include <Wt/Dbo/backend/Sqlite3.h>

#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{

	// ifstream dbocommFile("/etc/evntmgr/dbo-connection.txt");
	ifstream commFile("comunication.txt");

	// if (!dbocommFile)
	// {
	// 	std::cout << "\n\n - ERROR - Give me the DBO connection file and i will work :) - \n\n";
	// 	return 0;
	// }
	// std::string dboConnString((istreambuf_iterator<char>(dbocommFile)), istreambuf_iterator<char>());

	if (!commFile)
	{
		std::cout << "\n\n - ERROR - Give me the connection file ICE and i will work :) - \n\n";
		return 0;
	}
	std::string comAdapter;
	std::string comConnection;

	getline(commFile, comAdapter);
	getline(commFile, comConnection);

	// std::cout << "\n\n"
	// 		  << dboConnString << "\n\n";
	// std::cout << comAdapter << " ------ " << comConnection << "\n\n";

	/*
	Create Connection pool to a dbo and gives every interface a connection
	*/ 
	// dboConnString to connect to postgress / at the moment im using sqlite for dev
	auto myPQconnectdb = std::make_unique<dbo::backend::Sqlite3>("text.db");
	myPQconnectdb->setProperty("show-queries", "true");
	auto fixedConnPool = std::make_shared<dbo::FixedSqlConnectionPool>(std::move(myPQconnectdb), 2);

	try
	{
		Ice::CommunicatorHolder ich(argc, argv);
		auto adapter = ich->createObjectAdapterWithEndpoints(comAdapter, comConnection);

		// Authentification Interface HERE
		auto authServant = make_shared<AuthInterfaceI>(std::move(fixedConnPool->getConnection()));

		// Events Data Interface HERE
		auto eventDataServant = make_shared<EventInterfaceI>(std::move(fixedConnPool->getConnection()), authServant);

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
