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
	while (true)
	{
		sleep(sf::seconds(2));
		runUdpServer();
	}
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
	std::size_t received;
	sf::IpAddress sender;
	sf::IpAddress player1 = sf::IpAddress::None;
	sf::IpAddress player2 = sf::IpAddress::None;
	sf::Packet packet;
	sf::Uint16 dirP1;
	sf::Uint16 dirP2;
	sf::Uint16 start = -1;
	sf::Uint16 finish = 0;

	bool gameover = false;
	bool playersReady = false;
	unsigned short senderPort;

	while (!playersReady)
	{
		
		socket.receive(packet, sender, senderPort);

		if (player1 != sender && player2 == sf::IpAddress::None)
		{
			player2 = sender;
		}
		if (player1 == sf::IpAddress::None)
		{
			player1 = sender;
		}


		if (player1 != IpAddress::None && player2 != IpAddress::None)
		{
			packet.clear();
			packet << start;
			socket.send(packet, player1, senderPort);
			socket.send(packet, player2, senderPort);
			playersReady = true;
		}

	}


	while (!gameover)
	{



		socket.receive(packet, sender, senderPort);
		if (packet >> finish)
		{
			std::cout << "MENSAJE: " << sender << ": \"" << finish << "\"" << std::endl;
			if (finish == -1)
			{
				gameover = true;
				break;
			}
			
		}
		if (sender == player1)
		{
			if (packet >> dirP1)
			{
				std::cout << "PLAYER UNO: " << sender << ": \"" << dirP1 << "\"" << std::endl;
			}
			else
			{
				// Send an answer to the client
				socket.send(packet, player2, senderPort);

			}

		}
		if (sender == player2)
		{
			if (packet >> dirP2)
			{
				std::cout << "PLAYER DOS " << sender << ": \"" << dirP2 << "\"" << std::endl;
			}
			else
			{
				// Send an answer to the client
				socket.send(packet, player2, senderPort);
			}

		}

		packet.clear();

	}

	

	//std::cout << "Message sent to PLAYER ONE: \"" << out << "\"" << std::endl;
}
