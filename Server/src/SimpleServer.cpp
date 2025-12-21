/*
   © 2025 Hamdan Tariq

   LAN Chat App with File Transfer

   Licensed under the Apache License, Version 2.0.
   You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

   Distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND.
   
   File: SimpleServer.cpp
   Description: Implements the server component of the LAN Chat App, handling client connections, message routing, and file transfer management.
*/

#include <iostream>
#include <string>
#include <vector>
#include "olc_net.h" 
#include <map>


enum class MessageType : uint32_t {
	login,
	loginSuccessful,
	loginUnSuccessful,
	registeration,
	usernameTaken,
	registerationSuccessful,
	validationSuccessful,
	validationUnsuccessful,
	message,
	messageAll,
	newClient,
	removeClient,
	beat,
	loginDuplicate,
	fileInfo,
	fileChunk,
	fileComplete,
};

class MyServer : public olc::net::server_interface<MessageType>
{
public:
	MyServer(uint16_t nPort) : olc::net::server_interface<MessageType>(nPort)
	{
		heartbeatThread = std::thread([this]()
			{
				using namespace std::chrono_literals;

				while (running)
				{
					std::this_thread::sleep_for(5s);

					olc::net::message<MessageType> msg;
					msg.header.id = MessageType::beat;
					MessageAllClients(msg);
					while (!disconnected_clients.empty()) {
						olc::net::message<MessageType>remove;
						remove.header.id = MessageType::removeClient;
						std::string username = disconnected_clients.front();
						disconnected_clients.pop_front();
						remove << username;
						MessageAllClients(remove);
					}
				}
			});

	
	}

protected:
	// Called when a client attempts to connect
	virtual bool OnClientConnect(std::shared_ptr<olc::net::connection<MessageType>> client)
	{
		std::cout << "new raw connection: #" << client->GetID() << '\n';

		return true;
	}

	// Called when a client disconnects
	virtual void OnClientDisconnect(std::shared_ptr<olc::net::connection<MessageType>> client)
	{
		if (!duplicate) {

			if (id_to_username.count(client->GetID())) {
				std::string username = id_to_username[client->GetID()];
				id_to_username.erase(client->GetID());
				if (username_to_id.count(username)) {
					username_to_id.erase(username);
				}
				disconnected_clients.push_back(username);
			}
			duplicate = false;


		}
		else {
			duplicate = false;
		}


	}

	// Called when a message arrives
	virtual void OnMessage(std::shared_ptr<olc::net::connection<MessageType>> client, olc::net::message<MessageType>& msg)
	{
		olc::net::message<MessageType> reply;
		switch (msg.header.id) {

		case MessageType::login:
		{
			std::string username, password;
			msg >> password >> username;
			if (dummy_database.count(username)) {
				if (dummy_database[username] == password) {
					reply.header.id = MessageType::loginSuccessful;
					MessageClient(client, reply);
					if (username_to_id.count(username)) {
						reply.header.id = MessageType::loginDuplicate;
						MessageClient(client, reply);
						client->Disconnect();
						reply.header.id = MessageType::beat;
						duplicate = true;
						MessageClient(client, reply);

					}
					else {
						//new client give its msg data from database //to do
						id_to_username[client->GetID()] = username;
						username_to_id[username] = client->GetID();
						reply.header.id = MessageType::newClient;
						reply << username;
						//later add msg history adding
						MessageAllClients(reply, client);

						for (auto conn : m_deqConnections) {
							if (conn != client) {
								olc::net::message<MessageType> update;
								update.header.id = MessageType::newClient;
								update << id_to_username[conn->GetID()];
								MessageClient(client, update);
							}
						}
					}
				}
				else {
					reply.header.id = MessageType::loginUnSuccessful;
					MessageClient(client, reply);
				}
			}
			else {
				reply.header.id = MessageType::loginUnSuccessful;
				MessageClient(client, reply);
			}
		}
		break;

		case MessageType::registeration:
		{
			std::string username, password;
			msg >> password >> username;
			if (!dummy_database.count(username)) {
				dummy_database[username] = password;
				reply.header.id = MessageType::registerationSuccessful;
				MessageClient(client, reply);
			}
			else {
				reply.header.id = MessageType::usernameTaken;
				MessageClient(client, reply);
			}
		}
		break;

		case MessageType::message:
		{
			std::string username, text;
			msg >> text >> username;
			reply.header.id = MessageType::message;
			reply << id_to_username[client->GetID()] << text;
			for (auto conn : m_deqConnections) {
				if (username == id_to_username[conn->GetID()]) {
					MessageClient(conn, reply);
					break;
				}
			}
		}
		break;
		case MessageType::fileInfo:
		case MessageType::fileChunk:
		case MessageType::fileComplete:
		{
			std::string receiverUsername;
			msg >> receiverUsername;

			if (!id_to_username.count(client->GetID())) {
				break;
			}

			std::string senderUsername = id_to_username[client->GetID()];
			msg << senderUsername;

			if (username_to_id.count(receiverUsername)) {
				uint32_t receiver_id = username_to_id[receiverUsername];
				for (auto conn : m_deqConnections) {
					if (conn->GetID() == receiver_id) {
						MessageClient(conn, msg);
						break;
					}
				}
			}

		}
		break;
		case MessageType::messageAll:
		{
			std::string username, text;
			username = id_to_username[client->GetID()];
			msg >> text;
			reply.header.id = MessageType::messageAll;
			reply << username << text;
			MessageAllClients(reply, client);
		}
		break;

		default:
			break;
		}

	}

	virtual void OnClientValidation(std::shared_ptr<olc::net::connection<MessageType>>client) {
		std::cout << "[" << client->GetID() << "] verified successfully\n";
		olc::net::message<MessageType> msg;
		msg.header.id = MessageType::validationSuccessful;
		MessageClient(client, msg);

	}

public:
	~MyServer()
	{
		running = false;

		if (heartbeatThread.joinable())
			heartbeatThread.join();
	}


private:
	std::map<uint32_t, std::string> id_to_username;
	std::map<std::string, uint32_t> username_to_id;
	std::map<std::string, std::string> dummy_database;
	std::deque<std::string> disconnected_clients;
	bool duplicate = false;
	std::thread heartbeatThread;
	std::atomic_bool running = true;
};


static const char* MULTICAST_GROUP = "239.255.0.1";

void RunDiscoveryResponder(unsigned short udpPort)
{
	try {
		asio::io_context io;

		asio::ip::udp::endpoint listen_endpoint(
			asio::ip::udp::v4(), udpPort);

		asio::ip::udp::socket socket(io);

		socket.open(listen_endpoint.protocol());
		socket.set_option(asio::ip::udp::socket::reuse_address(true));
		socket.bind(listen_endpoint);

		asio::ip::address multicast_addr =
			asio::ip::make_address(MULTICAST_GROUP);

		socket.set_option(
			asio::ip::multicast::join_group(multicast_addr)
		);

		socket.set_option(asio::ip::multicast::hops(1));

		std::cout << "Discovery responder running on UDP "
			<< udpPort << "\n";

		char buffer[1024];
		asio::ip::udp::endpoint sender;

		while (true) {
			size_t len = socket.receive_from(
				asio::buffer(buffer), sender);

			std::string msg(buffer, len);

			if (msg == "DISCOVER_SERVER") {
				std::string reply = "SERVER_HERE";
				socket.send_to(
					asio::buffer(reply), sender);

				std::cout << "Replied to "
					<< sender.address().to_string()
					<< "\n";
			}
		}
	}
	catch (std::exception& e) {
		std::cerr << "Discovery error: "
			<< e.what() << "\n";
	}
}




int main()
{
	
	MyServer server(60000);
	server.Start();
	std::thread discoveryThread(RunDiscoveryResponder, 8888);
	discoveryThread.detach();
	while (1)
	{
		server.Update(-1, true);
	}

	return 0;
}



