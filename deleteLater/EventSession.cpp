// void EventSession::addServiceData(ServiceInfo ServiceInfo)
// {
// 	auto dateTime = Wt::WDateTime().fromString(ServiceInfo.dateTime, Emlab::DATETIMEFORMAT);
// 	dbo::Transaction transaction(session_);
// 	dbo::ptr<Event> event = session_.find<Event>().where("id = ?").bind(ServiceInfo.eventId);
// 	std::unique_ptr<Service> service{new Service()};

// 	service->event = event;
// 	service->providerIdentity = ServiceInfo.providerIdentity;
// 	service->providerService = ServiceInfo.providerService;
// 	service->dateTime = dateTime.toTimePoint();
// 	service->duration = ServiceInfo.duration;
// 	service->cost = ServiceInfo.cost;
// 	service->description = ServiceInfo.description;
// 	service->observations = ServiceInfo.observations;

// 	dbo::ptr<Service> servicePtr = session_.add(std::move(service));

// 	transaction.commit();
// }

// int EventSession::regClientInfo(int userId, ClientInfo clientInfo)
// {
// 	std::cout << "\n\n regclientInfo  ------------- START \n\n";

// 	dbo::Transaction transaction(session_);

// 	std::unique_ptr<Client> client{new Client()};
// 	dbo::ptr<User> user = session_.find<User>().where("id = ?").bind(userId);

// 	// see if client with client name and phone already exists
// 	dbo::ptr<Client> clientPtrDbo = session_.find<Client>().where("name = ? AND phone = ?").bind(clientInfo.name).bind(clientInfo.phone);
// 	if (clientPtrDbo)
// 	{
// 		std::cout << "\n\n regclientInfo  ------------- END \n\n";
// 		return clientPtrDbo.id();
// 	}

// 	client->user = user;
// 	client->name = clientInfo.name;
// 	client->phone = clientInfo.phone;
// 	client->specialNote = clientInfo.specialNote;

// 	dbo::ptr<Client> clientPtr = session_.add(std::move(client));
// 	transaction.commit();

// 	std::cout << "\n\n regclientInfo  ------------- END \n\n";
// 	return clientPtr.id();
// }



// SeqClientInfo EventSession::getClientsByName(int userId, std::string name)
// {
// 	std::cout << "\n\n getClientsByName  = " << name << "\n\n";

// 	SeqClientInfo seqClientInfo;
// 	dbo::Transaction transaction(session_);
// 	try
// 	{
// 		Clients clients = session_.find<Client>().where("user_id = ?").bind(userId).where("name LIKE ?").bind("%" + name + "%");

// 		for (const dbo::ptr<Client> client : clients)
// 		{
// 			ClientInfo ClientInfo;
// 			ClientInfo.id = client->id();
// 			ClientInfo.name = client->name;
// 			ClientInfo.phone = client->phone;

// 			seqClientInfo.push_back(ClientInfo);
// 		}
// 	}
// 	catch (dbo::Exception &e)
// 	{
// 		log("info") << e.what();
// 	}

// 	transaction.commit();
// 	return seqClientInfo;
// }

// SeqClientInfo EventSession::getClientsByPhone(int userId, std::string phone)
// {
// 	std::cout << "\n\n getClientsByPhone  = " << phone << "\n\n";

// 	SeqClientInfo seqClientInfo;
// 	dbo::Transaction transaction(session_);
// 	try
// 	{
// 		Clients clients = session_.find<Client>().where("user_id = ?").bind(userId).where("phone LIKE ?").bind(phone + "%");

// 		for (const dbo::ptr<Client> client : clients)
// 		{
// 			ClientInfo ClientInfo;
// 			ClientInfo.id = client->id();
// 			ClientInfo.name = client->name;
// 			ClientInfo.phone = client->phone;

// 			seqClientInfo.push_back(ClientInfo);
// 		}
// 	}
// 	catch (dbo::Exception &e)
// 	{
// 		log("info") << e.what();
// 	}

// 	transaction.commit();
// 	return seqClientInfo;
// }