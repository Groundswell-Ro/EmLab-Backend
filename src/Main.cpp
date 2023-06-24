#include "include/AuthInterfaceI.h"
#include "include/EventInterfaceI.h"
#include "include/ClientInterfaceI.h"
#include "include/ServiceInterfaceI.h"
#include "include/ReviewInterfaceI.h"

#include <Wt/Dbo/FixedSqlConnectionPool.h>
#include <Wt/Dbo/backend/Sqlite3.h>

#include <iostream>
#include <fstream>
#include <Ice/Ice.h>


int main(int argc, char *argv[])
{

	// ifstream dbocommFile("/etc/evntmgr/dbo-connection.txt");

	// if (!dbocommFile)
	// {
	// 	std::cout << "\n\n - ERROR - Give me the DBO connection file and i will work :) - \n\n";
	// 	return 0;
	// }
	// std::string dboConnString((istreambuf_iterator<char>(dbocommFile)), istreambuf_iterator<char>());

	std::string comAdapter = "EventManagerAdapter";
	std::string comConnection = "default -p 10000";

	/*
	Create Connection pool to a dbo and gives every interface a connection
	*/ 
	// dboConnString to connect to postgress / at the moment im using sqlite for dev
	auto myPQconnectdb = std::make_unique<dbo::backend::Sqlite3>("/mnt/c/Users/croit/Desktop/testSqlite.db");
	myPQconnectdb->setProperty("show-queries", "true");
	auto fixedConnPool = std::make_shared<dbo::FixedSqlConnectionPool>(std::move(myPQconnectdb), 5);

	try
	{
		Ice::CommunicatorHolder ich(argc, argv);
		auto adapter = ich->createObjectAdapterWithEndpoints(comAdapter, comConnection);

		// Interfaces HERE
		auto authServant = std::make_shared<AuthInterfaceI>(std::move(fixedConnPool->getConnection()));
		auto eventServant = std::make_shared<EventInterfaceI>(std::move(fixedConnPool->getConnection()), authServant);
		auto clientServant = std::make_shared<ClientInterfaceI>(std::move(fixedConnPool->getConnection()), authServant);
		auto serviceServant = std::make_shared<ServiceInterfaceI>(std::move(fixedConnPool->getConnection()), authServant);
		auto reviewServant = std::make_shared<ReviewInterfaceI>(std::move(fixedConnPool->getConnection()), authServant);
		
		// Add Servants to the adaptor
		adapter->add(authServant, Ice::stringToIdentity(AUTHADAPTER));
		adapter->add(eventServant, Ice::stringToIdentity(EVENTADAPTER));
		adapter->add(clientServant, Ice::stringToIdentity(CLIENTADAPTER));
		adapter->add(serviceServant, Ice::stringToIdentity(SERVICEADAPTER));
		adapter->add(reviewServant, Ice::stringToIdentity(REVIEWADAPTER));

		adapter->activate();
		ich->waitForShutdown();
	}
	catch (const std::exception &e)
	{
		Wt::log(e.what()) << "\n - ERROR - \n";
		return 1;
	}
	return 0;
}
