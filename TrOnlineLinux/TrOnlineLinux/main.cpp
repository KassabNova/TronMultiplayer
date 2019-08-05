#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <time.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <thread>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
void runUdpClient();
int mainGameLoop();
int connectionReady();
void kill_child(int);
using namespace sf;

const int W = 1100;
const int H = 600;
const unsigned short port = 50001;

int speed = 3;
int dirServer = 0;
int play = 1;
pid_t pid = -1;
pid_t killpid;
pid_t parent;
bool field[W][H] = { 0 };
sf::Uint16 ready = 0;
struct player
{
	int x, y, dir;
	Color color;
	player(Color c)
	{
		x = rand() % W;
		y = rand() % H;
		color = c;
		dir = 2;
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

    signal(SIGUSR1, kill_child);
    signal(SIGKILL, kill_child);

    int status;
    while(play)
    {
        while(!ready)
        {

            std::cout << "Waiting for player two \""  << std::endl;
            ready = connectionReady();
        }
        pid= fork();
        killpid = pid;
        if (pid == 0)
        {

            killpid = getpid();
            parent = getppid();
            //kill(pid,SIGKILL);

            std::cout << "PID: \"" <<  getpid() << "\"" << std::endl;

            std::cout << "PID: \"" <<  killpid << "\"" << std::endl;
            std::cout << "PID: \"" <<  getppid() << "\"" << std::endl;
            //exit(1);

            sleep(sf::seconds(15));
            runUdpClient();
        }
        else if (pid > 0)
        {
            // parent process
            mainGameLoop();
        }
        else
        {
            // fork failed
            printf("fork() failed!\n");
            return 1;
        }
        waitpid(pid,&status,0);
    }



    std::cout << "End of Program \"" << "\"" << std::endl;
	return 0;
}


int mainGameLoop() {
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
	text.setCharacterSize(40);
	text.setFillColor(sf::Color::Magenta);
	text.setOrigin(-250, -350);

	text2.setFont(font);
	text2.setString(" ");
	text2.setCharacterSize(40);
	text2.setFillColor(sf::Color::Yellow);
	text2.setOrigin(-215, -150);


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


		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
            play = 0;
            return 1;
		}

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
				ready = 0;

				p1.x = 300;
				p1.y = 400;
				p1.dir = rand() % 4;

				p2.x = 300;
				p2.y = 500;
				p2.dir = rand() % 4;

				for (int i = 0; i < H; i++)
				{
					for (int j = 0; j < W; j++)
					{
						field[j][i] = 0;
					}
				}
                return 1;
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
                raise(SIGUSR1);
                //kill(killpid, SIGUSR1);

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
/// Send a message to the server, wait for the answer      /
////////////////////////////////////////////////////////////
void runUdpClient()
{
	unsigned short port = 50001;

	// server address
	sf::IpAddress server;
	sf::Uint16 dir = dirServer;
	sf::Packet packet;
	server = "192.168.15.100";
	// Create a socket for communicating with the server
	sf::UdpSocket socket;
	// Send a message to the server
	while (true)
	{
		packet.clear();
		dir = dirServer;
		packet << dir;
		std::cout << "Message sent to the server: \"" << dir << "\"" << std::endl;

		socket.send(packet, server, port);
	}


}

int connectionReady()
{
    unsigned short port = 50001;

	// server address
	sf::IpAddress server;
	sf::Uint16 readyPlayer1 = 1;
	sf::Uint16 gameStart = 0;
	sf::Packet packet;
	server = "192.168.15.100";
	// Create a socket for communicating with the server
	sf::UdpSocket socket;
	// Send a message to the server
    packet << readyPlayer1;
    std::cout << "I am ready to play!: \"" << "\"" << std::endl;

    socket.send(packet, server, port);
    packet.clear();
    socket.receive(packet, server,port);

    if(packet >> gameStart)
    {
        return gameStart;
    }

    return 0;



}

void kill_child(int sig)
{
    pid_t test = getpid();
    std::cout << "Me voy a morir" << test <<std::endl;
    test = getppid();
    std::cout << "Me voy a morir" << test <<std::endl;
    std::cout << "Me voy a morir" << killpid <<std::endl;

    //exit(1);
    kill(killpid, SIGKILL);
    //exit(0);
}
