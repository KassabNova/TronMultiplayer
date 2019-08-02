#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <time.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <thread>
void runUdpServer();
void runUdpClient();
using namespace sf;

const int W = 1600;
const int H = 900;
const unsigned short port = 50001;

int speed = 3;
int dirServer = 0;
bool field[W][H] = { 0 };

struct player
{
	int x, y, dir;
	Color color;
	player(Color c)
	{
		x = rand() % W;
		y = rand() % H;
		color = c;
		dir = rand() % 4;
	}
	void tick()
	{
		if (dir == 0) y += 1;
		if (dir == 1) x -= 1;
		if (dir == 2) x += 1;
		if (dir == 3) y -= 1;

		if (x >= W) x = 0;  if (x < 0) x = W - 1;
		if (y >= H) y = 0;  if (y < 0) y = H - 1;
	}

	Vector3f getColor()
	{
		return Vector3f(color.r, color.g, color.b);
	}
};

int main()
{
	srand(time(0));

	RenderWindow window(VideoMode(W, H), "The Tron Game!");
	window.setFramerateLimit(60);
	player p1(Color::Red), p2(Color::Green);

	Texture texture;
	texture.loadFromFile("background.jpg");
	Sprite sBackground(texture);


	Sprite sprite;
	RenderTexture t;
	t.create(W, H);
	t.setSmooth(true);
	sprite.setTexture(t.getTexture());
	t.clear();  t.draw(sBackground);

	bool Game = 1;
	bool PlayAgain = 0;
	sf::Text text;
	sf::Text text2;
	sf::Font font;
	font.loadFromFile("CourierNew.ttf");
	text.setFont(font);
	text.setString("");
	text.setCharacterSize(80);
	text.setFillColor(sf::Color::Magenta);
	text.setOrigin(-250, -350);

	text2.setFont(font);
	text2.setString(" ");
	text2.setCharacterSize(80);
	text2.setFillColor(sf::Color::Yellow);
	text2.setOrigin(-215, -150);

	std::thread cliente(runUdpClient);     // spawn new thread that calls foo()
	//std::thread server(runUdpServer);     // spawn new thread that calls foo()

	while (window.isOpen())
	{
		Event e;
		while (window.pollEvent(e))
		{
			if (e.type == Event::Closed)
				window.close();
		}

		if (Keyboard::isKeyPressed(Keyboard::Left)) if (p1.dir != 2) p1.dir = 1;
		if (Keyboard::isKeyPressed(Keyboard::Right)) if (p1.dir != 1)  p1.dir = 2;
		if (Keyboard::isKeyPressed(Keyboard::Up)) if (p1.dir != 0) p1.dir = 3;
		if (Keyboard::isKeyPressed(Keyboard::Down)) if (p1.dir != 3) p1.dir = 0;

		if (Keyboard::isKeyPressed(Keyboard::A)) if (p2.dir != 2) p2.dir = 1;
		if (Keyboard::isKeyPressed(Keyboard::D)) if (p2.dir != 1)  p2.dir = 2;
		if (Keyboard::isKeyPressed(Keyboard::W)) if (p2.dir != 0) p2.dir = 3;
		if (Keyboard::isKeyPressed(Keyboard::S)) if (p2.dir != 3) p2.dir = 0;



		// synchronize threads:
		//cliente.join();
		//runUdpClient(50001);
		if (PlayAgain)
		{
			text2.setString("Press space to play again");
			window.draw(text);
			window.draw(text2);
			window.display();
			if (Keyboard::isKeyPressed(Keyboard::Space))
			{
				text2.setString(" ");
				text.setString(" ");

				window.clear();
				t.clear();

				t.draw(sBackground);
				window.draw(sprite);
				window.draw(text);
				window.draw(text2);
				window.display();

				Game = 1;
				PlayAgain = 0;
				p1.x = rand() % W;
				p1.y = rand() % H;
				p1.dir = rand() % 4;

				p2.x = rand() % W;
				p2.y = rand() % H;
				p2.dir = rand() % 4;

				for (int i = 0; i < H; i++)
				{
					for (int j = 0; j < W; j++)
					{
						field[j][i] = 0;
					}
				}

			}
		}


		if (!Game)continue;



		for (int i = 0; i < speed; i++)
		{
			dirServer = p1.dir;
			p1.tick(); p2.tick();
			if (field[p1.x][p1.y] == 1)
			{
				Game = 0;
				PlayAgain = 1;
				text.setString("Player one is a loozer!");
			}
			if (field[p2.x][p2.y] == 1)
			{
				Game = 0;
				PlayAgain = 1;
				text.setString("Player two is a loozer!");
			}
			field[p1.x][p1.y] = 1;
			field[p2.x][p2.y] = 1;

			CircleShape c(3);
			c.setPosition(p1.x, p1.y); c.setFillColor(p1.color);	t.draw(c);
			c.setPosition(p2.x, p2.y); c.setFillColor(p2.color);	t.draw(c);
			if (Game == 0)
			{
			}
			t.display();
		}

		////// draw  ///////
		window.clear();
		window.draw(sprite);

		window.draw(text);
		window.display();
	}
	return 0;
}



////////////////////////////////////////////////////////////
/// Launch a server, wait for a message, send an answer.   /
////////////////////////////////////////////////////////////
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
	unsigned short senderPort;
	while (true)
	{
		if (socket.receive(in, sizeof(in), received, sender, senderPort) != sf::Socket::Done)
			return;
		std::cout << "Message received from client " << sender << ": \"" << in << "\"" << std::endl;
	}



	// Send an answer to the client
	/*const char out[] = "Hi, I'm the server";
	if (socket.send(out, sizeof(out), sender, senderPort) != sf::Socket::Done)
		return;
	std::cout << "Message sent to the client: \"" << out << "\"" << std::endl;*/
}


////////////////////////////////////////////////////////////
/// Send a message to the server, wait for the answer      /
////////////////////////////////////////////////////////////
void runUdpClient()
{
	unsigned short port = 50001;

	// server address
	char out[] = "1";
	sf::IpAddress server;
	server = sf::IpAddress::LocalHost;
	// Create a socket for communicating with the server
	sf::UdpSocket socket;
	// Send a message to the server
	//std::string s = std::to_string(dirServer);
	while (true)
	{
		out[0] = dirServer + 48;

		if (socket.send(out, sizeof(out), server, port) != sf::Socket::Done)
			std::cout << "Socket thingy" << std::endl;
		std::cout << "Message sent to the server: \"" << out << "\"" << std::endl;
	}


}
