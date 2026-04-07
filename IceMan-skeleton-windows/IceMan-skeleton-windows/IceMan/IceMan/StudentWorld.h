#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <queue>
#include <array>



// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir), p1(nullptr), ticksPassed(0)
	{
		for (int a = 0; a < 64; a++)
		{
			for (int b = 0; b < 64; b++)
			{
				shortPath[a][b] = new int(10000);
				i_shortPath[a][b] = new int(10000);
			}
		}
	}

	virtual int init();

	virtual int move();

	virtual void cleanUp();

	void destroyIce(int x, int y);
	//sets any ice object that comes in contact with the Iceman to not visible

	bool checkIce(int x, int y);
	//returns true if there is ice at the location parameter

	std::string checkActor(int x, int y);



	Iceman* getPlayer();
	void addActor(Actor* a);
	void setDisplayText();

	int getOilLeft() const { return numOilLeft; }
	void updateOilLeft(int o) { numOilLeft -= o; };

	int getSonarLeft() const { return numSonarLeft; }
	int updateSonarLeft(int x);
	// (+) to increment and (-) to decrement

	bool protesterGold(int x, int y);

	void call();
	void damagecall(Actor* s1, std::string who, int x, int y);

	void shortestPath(int startX, int startY, int ax, int ay); //Finds the shortest path from the coords (x,y) to the actors location (ax,ay)
	void i_shortestPath(int startX, int startY, int ax, int ay, int maxDistance);
	int* checkExitPath(int x, int y);
	int* i_checkPath(int x, int y);
	void cleanUpShortPath();

	~StudentWorld();

private:
	std::vector<Actor*> actors; // part A
	Iceman* p1;
	Ice* iceField[64][64]{ nullptr };
	int* shortPath[64][64]; //shorest path to exit field
	int* i_shortPath[64][64]; //shortest path to Iceman 
	int numOilLeft;
	int numSonarLeft;
	int ticksPassed;
};

#endif // STUDENTWORLD_H_

/*

modified date 6/4/2021 @ 2:04pm


*/
