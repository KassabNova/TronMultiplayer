#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <time.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <thread>

void runUdpServer();
using namespace sf;

#include <iostream>

int main()
{
    std::cout << "Hello World!\n";
	runUdpServer();
}



void runUdpServer()
{
	unsigned short port = 50001;
	// Create a socket to receive a message from anyone
	sf::UdpSocket socket;

	// Listen to messages on the specified port
	if (socket.bind(port) != sf::Socket::Done)
		return;
	std::cout << "Server is listening to port " << port << ", waiting for a message... " << std::endl;

	// Wait for a message
	char in[128];
	std::size_t received;
	sf::IpAddress sender;
	sf::IpAddress player1 = sf::IpAddress::None;
	sf::IpAddress player2 = sf::IpAddress::None;
	sf::Packet packet;
	sf::Uint16 dirP1 = -1;
	sf::Uint16 dirP2;
	unsigned short senderPort;
	while (true)
	{
		if (socket.receive(packet, sender, senderPort) != sf::Socket::Done)
			return;
		if (player1 != sender  && player2 == sf::IpAddress::None)
		{
			player2 = sender;
		}
		if (player1 == sf::IpAddress::None)
		{
			player1 = sender;
		}
		

		if (player1 != IpAddress::None && player2 != IpAddress::None)
		{
			if (sender == player1)
			{
				if (packet >> dirP1)
				{
					std::cout << "PLAYER UNO: " << sender << ": \"" << dirP1 << "\"" << std::endl;
				}
				else
				{
					/*const char out[] = "Hi, I'm the server";
					if (socket.send(out, sizeof(out), player2, senderPort) != sf::Socket::Done)
						return;*/
				}

			}
			if (sender == player2)
			{
				if (packet >> dirP2)
				{
				//	std::cout << "Message received from client " << sender << ": \"" << dirP2 << "\"" << std::endl;
				}
				else
				{
					/*const char out[] = "Hi, I'm the server";
					if (socket.send(out, sizeof(out), player2, senderPort) != sf::Socket::Done)
						return;*/
				}

			}
		}
		packet.clear();
		
	}



	// Send an answer to the client
	/*const char out[] = "Hi, I'm the server";
	if (socket.send(out, sizeof(out), sender, senderPort) != sf::Socket::Done)
		return;
	std::cout << "Message sent to the client: \"" << out << "\"" << std::endl;*/
}
