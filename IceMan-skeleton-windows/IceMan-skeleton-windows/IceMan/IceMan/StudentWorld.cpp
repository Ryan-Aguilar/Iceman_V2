#include "StudentWorld.h"
#include <vector>
#include <sstream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <ctime>
#include <array>

using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

int StudentWorld::init()
{
	//Create the player character - Iceman 
	p1 = new Iceman(this);

	//create an ice field
	int rows = 64;
	int columns = 64;

	for (int y = 0; y < rows - 4; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			iceField[x][y] = new Ice(x, y, this);
		}
	}

	for (int y = 0; y < rows - 4; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			if (x >= 30 && x < 34 && y > 3)
				iceField[x][y]->setVisible(false);
		}
	}

	//add all other actors

	int numBoulders = min(((int)getLevel() / 2) + 2, 9);
	int numGold = max((5 - (int)getLevel()) / 2, 2);
	int numOil = min((2 + (int)getLevel()), 21);
	numOilLeft = numOil;

	for (int i = 0; i < numBoulders; i++) //add boulders
	{
		int tempX{};
		int tempY{};
		tempX = rand() % 60;
		tempY = rand() % 40 + 16;



		if (checkActor(tempX, tempY) == "No Actor" && (tempX <= 26 || tempX >= 34))
		{
			for (int x = tempX; x < tempX + 4; x++)
			{
				for (int y = tempY; y < tempY + 4; y++)
				{
					if (iceField[x][y]->isVisible())
					{
						iceField[x][y]->setVisible(false);
					}
				}
			}
			actors.push_back(new Boulder(tempX, tempY, this));
		}
		else
			i--;


	}
	for (int i = 0; i < numOil; i++) // add oil
	{
		int tempX{};
		int tempY{};
		tempX = rand() % 60;
		tempY = rand() % 56;

		if (checkActor(tempX, tempY) == "No Actor" && (tempX <= 26 || tempX >= 34))
		{
			actors.push_back(new Oil_Barrel(tempX, tempY, this));
		}
		else
			i--;

	}
	for (int i = 0; i < numGold; i++) // add gold
	{
		int tempX{};
		int tempY{};
		tempX = rand() % 60;
		tempY = rand() % 56;

		if (checkActor(tempX, tempY) == "No Actor" && (tempX <= 26 || tempX >= 34))
		{
			actors.push_back(new Gold(tempX, tempY, this, 1));
		}
		else
			i--;
	}

	actors.push_back(new Regular_protester(60, 60, this));


	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::setDisplayText()
{
	int level = getLevel() + 1; // 2 digits
	int lives = getLives(); // 1 digit
	int health = p1->getHealth(); // 3 digits : _75% or 100%
	int squirts = p1->getNumSquirt(); // 2 digits
	int gold = p1->getNumGold(); // 2 digits
	int barrelsLeft = numOilLeft; // 2 digits
	int sonar = p1->getNumSonar(); // 2 digits
	int score = getScore(); // 6 digits with leading zeros, not leading spaces

	ostringstream oss; //for formatting leading spaces

	oss << "Level:" << oss.fill(' ') << setw(2) << level;
	oss << " Lives:" << oss.fill(' ') << lives;
	oss << " Hlth:" << oss.fill(' ') << setw(3) << (health * 10) << "%";
	oss << " Wtr:" << oss.fill(' ') << setw(2) << squirts;
	oss << " Gld:" << oss.fill(' ') << setw(2) << gold;
	oss << " Oil Left:" << oss.fill(' ') << setw(2) << barrelsLeft;
	oss << " Sonar:" << oss.fill(' ') << setw(2) << sonar;
	oss << " Score:" << oss.fill('0') << setw(6) << score;

	string newDisplay = oss.str();


	setGameStatText(newDisplay);
}

int StudentWorld::move()
{
	// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
	// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.

	ticksPassed++;

	setDisplayText();

	if (!(p1->isAlive()))
	{
		decLives();
		playSound(SOUND_PLAYER_GIVE_UP);
		return GWSTATUS_PLAYER_DIED;
	}
	else if (getOilLeft() == 0)
	{
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}


	////check which actors are dead and remove them from the game
	actors.erase(remove_if(actors.begin(), actors.end(), [](auto a) {return !(a->isAlive()); }), actors.end());

	//Add water and sonar to the game
	int randomChance = rand() % ((25 * getLevel()) + 300); // 1 in int G = current_level_number * 25 + 300
	if (randomChance == 1)
	{
		int random = rand() % 5;
		if (random == 1)
		{
			actors.push_back(new Sonar(0, 60, this));
		}
		else
		{
			bool spawned = false;

			while (!spawned)
			{
				int tempX{};
				int tempY{};
				tempX = rand() % 60;
				tempY = rand() % 60;

				bool noIce = true;

				for (int x = tempX; x < tempX + 4; x++)
				{
					for (int y = tempY; y < tempY + 4; y++)
					{
						if (checkIce(x, y))
						{
							noIce = false;
						}
					}
				}

				if (checkActor(tempX, tempY) == "No Actor" && noIce)
				{
					actors.push_back(new Water(tempX, tempY, this));
					spawned = true;
				}

			}
		}
	}

	int spawnProtester = std::max(25, 200 - (int)getLevel());

	if (ticksPassed % spawnProtester == 0)
	{
		int numProtester = 0;
		for (auto a : actors)
		{
			if (a->type() == "Protester")
			{
				numProtester++;
			}
		}

		if (numProtester < std::min(15, 2 + (int)(getLevel() * 1.5)))
		{
			int probOfHardcore = std::min(90, (int)getLevel() * 10 + 30);
			int random = rand() % probOfHardcore;
			if (random == 1)
			{
				addActor(new hardcoreprotester(60, 60, this));
			}
			else
			{
				addActor(new Regular_protester(60, 60, this));
			}
		}
	}



	//Do Something
	p1->doSomething();

	for (auto a : actors)
	{
		a->doSomething();
	}

	return GWSTATUS_CONTINUE_GAME; //return GWSTATUS_PLAYER_DIED;
}

void StudentWorld::cleanUp()
{
	delete p1; //delete the Iceman

	for (auto a : actors)
	{
		delete a; //delete all other actors
		actors.pop_back();
	}

	for (int y = 0; y < 64; y++) //delete the ice field
	{
		for (int x = 0; x < 64; x++)
		{
			if (iceField[x][y] != nullptr)
				delete iceField[x][y];
		}
	}
}

void StudentWorld::destroyIce(int x, int y)
{
	if (iceField[x][y] != nullptr && iceField[x][y]->isVisible())
	{
		playSound(SOUND_DIG);
		iceField[x][y]->setVisible(false);
	}
}

bool StudentWorld::checkIce(int x, int y)
{
	if (iceField[x][y] != nullptr)
	{
		if (iceField[x][y]->isVisible())
			return true;
	}

	return false;
}

string StudentWorld::checkActor(int x, int y)
{
	int ax, ay;
	for (auto a : actors)
	{
		ax = a->getX();
		ay = a->getY();

		if (((ax > x - 4) && (ax < x + 4)) && ((ay > y - 4) && (ay < y + 4))) {
			return a->type();
		}

	}
	return "No Actor";
}

Iceman* StudentWorld::getPlayer()
{
	return p1;
}

void StudentWorld::addActor(Actor* a)
{
	actors.push_back(a);
}

//Sonar Functions
int StudentWorld::updateSonarLeft(int x) {
	return numSonarLeft += x;
}

void StudentWorld::call()
{
	for (auto z : actors)
	{
		z->sonarSearch();
	}

}

bool StudentWorld::protesterGold(int x, int y)
{

	for (auto a : actors)
	{
		int ax = a->getX();
		int ay = a->getY();
		if (a->type() == "Protester")
		{
			if (x <= ax + 3 && x >= ax - 3)
			{
				if (y <= ay + 3 && y >= ay - 3)
				{
					a->gotGold();
					return true;
				}
			}
		}
	}

	return false;
}

//Squirt Functions

void StudentWorld::damagecall(Actor* actor, std::string who, int x, int y)
{
	std::string hit{};
	for (auto z : actors)
	{
		std::string s = z->type();
		std::string punisher = actor->type();
		int ax = z->getX();
		int ay = z->getY();



		if (s == who && punisher == "Squirt")
		{
			if (x <= ax + 3 && x >= ax - 3)
			{
				if (y <= ay + 3 && y >= ay - 3)
				{
					hit = "hit";
					z->damageintake(2);
				}
			}
		}
		else if (s == who && punisher == "Boulder")
		{
			if (x <= ax + 3 && x >= ax - 3)
			{
				if (y == ay + 3)
				{
					hit = "hit";
					z->damageintake(100);
					increaseScore(500);
				}
			}
		}
	}
	//std::string g = "missed";
	return;
}

int* StudentWorld::checkExitPath(int x, int y)
{
	int* steps = shortPath[x][y];
	return steps;
}

int* StudentWorld::i_checkPath(int x, int y)
{
	int* steps = i_shortPath[x][y];
	return steps;
}


#include <utility>


void StudentWorld::shortestPath(int x, int y, int ax, int ay)
{

	queue<pair<int, int>> q;
	pair<int, int> point;
	point.first = x; point.second = y;

	q.push(point);
	shortPath[x][y] = new int(0);

	while (!q.empty())
	{
		int curX = q.front().first; int curY = q.front().second;


		//if we are currently looking at an invalid posiiton on the ice field, just a precaution
		if (checkIce(curX, curY) || (checkActor(curX, curY) == "Boulder"))
			continue;

		//check up down left and right, 
		if (!checkIce(curX, curY + 1) && !(checkActor(curX, curY + 1) == "Boulder") && curY < 60 && *shortPath[curX][curY + 1] >= 10000) //check up
		{
			bool valid = true;
			for (int tx = curX; tx < curX + 4; tx++)
			{
				for (int ty = curY; ty < curY + 4; ty++)
				{
					if (checkIce(tx, ty))
					{
						valid = false;
						continue;
					}
				}
			}
			if (valid)
				q.push(pair<int, int>(curX, curY + 1));
		}
		if (!checkIce(curX + 1, curY) && !(checkActor(curX + 1, curY) == "Boulder") && curX < 60 && *shortPath[curX + 1][curY] >= 1000) //check right
		{
			bool valid = true;
			for (int tx = curX; tx < curX + 4; tx++)
			{
				for (int ty = curY; ty < curY + 4; ty++)
				{
					if (checkIce(tx, ty))
					{
						valid = false;
						continue;
					}
				}
			}
			if (valid)
				q.push(pair<int, int>(curX + 1, curY));
		}
		if (curX > 0 && !checkIce(curX - 1, curY) && (checkActor(curX - 1, curY) != "Boulder") && shortPath[curX - 1][curY] != nullptr && *shortPath[curX - 1][curY] >= 10000) //check left
		{
			bool valid = true;
			for (int tx = curX; tx < curX + 4; tx++)
			{
				for (int ty = curY; ty < curY + 4; ty++)
				{
					if (checkIce(tx, ty))
					{
						valid = false;
						continue;
					}
				}
			}
			if (valid)
				q.push(pair<int, int>(curX - 1, curY));
		}
		if (curY > 0 && !checkIce(curX, curY - 1) && (checkActor(curX, curY - 1) != "Boulder") && shortPath[curX][curY - 1] != nullptr && *shortPath[curX][curY - 1] >= 10000) //check down
		{
			bool valid = true;
			for (int tx = curX; tx < curX + 4; tx++)
			{
				for (int ty = curY; ty < curY + 4; ty++)
				{
					if (checkIce(tx, ty))
					{
						valid = false;
						continue;
					}
				}
			}
			if (valid)
				q.push(pair<int, int>(curX, curY - 1));
		}

		//check if we have visited the lcoation already
		if (*shortPath[curX][curY] >= 100)
		{
			int lowest = 100;

			if (*shortPath[curX][curY + 1] < lowest)
				lowest = *shortPath[curX][curY + 1];
			if (curY > 0 && *shortPath[curX][curY - 1] < lowest)
				lowest = *shortPath[curX][curY - 1];
			if (*shortPath[curX + 1][curY] < lowest)
				lowest = *shortPath[curX + 1][curY];
			if (curX > 0 && *shortPath[curX - 1][curY] < lowest)
				lowest = *shortPath[curX - 1][curY];

			shortPath[curX][curY] = new int(lowest + 1);

		}

		//if the current (x,y) is equal to that of the desired actors (ax,ay)
		if (curX == ax && curY == ay)
			return;

		q.pop();

	}

}

void StudentWorld::i_shortestPath(int x, int y, int ax, int ay, int maxDistance)
{

	queue<pair<int, int>> q;
	pair<int, int> point;
	point.first = x; point.second = y;

	q.push(point);
	i_shortPath[x][y] = new int(0);

	while (!q.empty())
	{
		int curX = q.front().first; int curY = q.front().second;


		//if we are currently looking at an invalid posiiton on the ice field, just a precaution
		if (checkIce(curX, curY) || (checkActor(curX, curY) == "Boulder"))
		{
			q.pop();
			continue;
		}

		//check up down left and right, 
		if (!checkIce(curX, curY + 1) && !(checkActor(curX, curY + 1) == "Boulder") && curY < 60 && *i_shortPath[curX][curY + 1] >= 10000) //check up
		{
			bool valid = true;
			for (int tx = curX; tx < curX + 4; tx++)
			{
				for (int ty = curY; ty < curY + 4; ty++)
				{
					if (checkIce(tx, ty))
					{
						valid = false;
						continue;
					}
				}
			}
			if (valid)
				q.push(pair<int, int>(curX, curY + 1));
		}
		if (!checkIce(curX + 1, curY) && !(checkActor(curX + 1, curY) == "Boulder") && curX < 60 && *i_shortPath[curX + 1][curY] >= 1000) //check right
		{
			bool valid = true;
			for (int tx = curX; tx < curX + 4; tx++)
			{
				for (int ty = curY; ty < curY + 4; ty++)
				{
					if (checkIce(tx, ty))
					{
						valid = false;
						continue;
					}
				}
			}
			if (valid)
				q.push(pair<int, int>(curX + 1, curY));
		}
		if (curX > 0 && !checkIce(curX - 1, curY) && (checkActor(curX - 1, curY) != "Boulder") && i_shortPath[curX - 1][curY] != nullptr && *i_shortPath[curX - 1][curY] >= 10000) //check left
		{
			bool valid = true;
			for (int tx = curX; tx < curX + 4; tx++)
			{
				for (int ty = curY; ty < curY + 4; ty++)
				{
					if (checkIce(tx, ty))
					{
						valid = false;
						continue;
					}
				}
			}
			if (valid)
				q.push(pair<int, int>(curX - 1, curY));
		}
		if (curY > 0 && !checkIce(curX, curY - 1) && (checkActor(curX, curY - 1) != "Boulder") && i_shortPath[curX][curY - 1] != nullptr && *i_shortPath[curX][curY - 1] >= 10000) //check down
		{
			bool valid = true;
			for (int tx = curX; tx < curX + 4; tx++)
			{
				for (int ty = curY; ty < curY + 4; ty++)
				{
					if (checkIce(tx, ty))
					{
						valid = false;
						continue;
					}
				}
			}
			if (valid)
				q.push(pair<int, int>(curX, curY - 1));
		}

		//check if we have visited the lcoation already
		if (*i_shortPath[curX][curY] >= 100)
		{
			int lowest = 100;

			if (*i_shortPath[curX][curY + 1] < lowest)
				lowest = *i_shortPath[curX][curY + 1];
			if (curY > 0 && *i_shortPath[curX][curY - 1] < lowest)
				lowest = *i_shortPath[curX][curY - 1];
			if (*i_shortPath[curX + 1][curY] < lowest)
				lowest = *i_shortPath[curX + 1][curY];
			if (curX > 0 && *i_shortPath[curX - 1][curY] < lowest)
				lowest = *i_shortPath[curX - 1][curY];

			i_shortPath[curX][curY] = new int(lowest + 1);

		}

		//if the current (x,y) is equal to that of the desired actors (ax,ay)
		if (curX == ax && curY == ay || *i_shortPath[curX][curY] >= maxDistance)
			return;

		q.pop();

	}

}

void StudentWorld::cleanUpShortPath()
{

	for (int a = 0; a < 64; a++)
	{
		for (int b = 0; b < 64; b++)
		{
			delete shortPath[a][b];
			delete i_shortPath[a][b];
		}
	}

	for (int a = 0; a < 64; a++)
	{
		for (int b = 0; b < 64; b++)
		{
			shortPath[a][b] = new int(10000);
			i_shortPath[a][b] = new int(10000);
		}
	}
}

//Destructor
StudentWorld::~StudentWorld()
{
	delete p1; //delete the Iceman

	for (auto a : actors)
	{
		delete a; //delete all other actors
	}

	for (int a = 0; a < 64; a++)
	{
		for (int b = 0; b < 64; b++)
		{
			delete shortPath[a][b];
			delete i_shortPath[a][b];
		}
	}

	for (int y = 0; y < 64; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			if (iceField[x][y] != nullptr)
				delete iceField[x][y];
		}
	}
}
// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

/*

modified date 6/4/2021 @ 2:04pm


*/
