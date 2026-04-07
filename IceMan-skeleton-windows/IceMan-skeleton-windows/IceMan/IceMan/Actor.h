#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <string>

class StudentWorld;

//--------------------------------------------------------Actor-----------------------------------------------------------------------------

//Actor is an abstract base class or a pure virtual class
//We don't want to ever create an actor and we don't want it to be able to doSomething() 
//It is just used as an interface for all other game objects

class Actor : public GraphObject {
public:

	Actor(int imageID, int startX, int startY, Direction startDirection, StudentWorld* world, float size, unsigned int depth);

	//doSomething: What each actor does on each tick
	virtual void doSomething() = 0; //pure virtual fcn

	//Returns a pointer to the StudentWorld that is housing our game
	//Used to access all actors in the game and to update the game map
	StudentWorld* getWorld() const;

	//Returns true if the given actor is alive (bool status alive, not health)
	virtual bool isAlive();

	//Updates the bool alive status of the given actor
	virtual void setAliveStatus(bool newStatus);

	virtual void damageintake(int x);
	virtual std::string type();

	//Given a proximity, this returns true if the actor 'a' is within the specified proximity, prox, of the given actor
	virtual bool proximityCheck(int prox, Actor* a);

	//Only implement in Gold and Oil, sets the oil or gold to visible if within 12 units of a sonar wave
	virtual void sonarSearch() {/*Only does something for Oil and Gold*/ }

	virtual void gotGold() {/*only implement for protesters*/ }

	//Virtual Destructor
	virtual ~Actor() {};

private:
	StudentWorld* sw; //Game pointer
	bool alive; //Alive status

};

//--------------------------------------------------------Iceman-----------------------------------------------------------------------------

class Iceman : public Actor {
public:
	Iceman(StudentWorld* world) :
		Actor(IID_PLAYER, 30, 60, right, world, 1.0, 0), i_health(10), i_squirt(5), i_sonarCharge(1),
		i_gold_sack(0), i_facing(right) {}

	void doSomething();

	void damage(int damage); // updates the players health, subtracts damage parameter fromn health data member
	int getHealth() const; // returns the HP

	std::string type();

	//Getters and Setters for Iceman's Inventory
	int getNumGold() const;
	void addGold(int g);

	int getNumSquirt() const;
	void addWtr(int w);

	int getNumSonar() const;
	void addSonar(int s);

private:
	int i_health;
	int i_squirt;
	int i_sonarCharge;
	int i_gold_sack;
	Direction i_facing;
};

//--------------------------------------------------------Ice-----------------------------------------------------------------------------

class Ice : public Actor {
public:
	Ice(int x, int y, StudentWorld* world) :
		Actor(IID_ICE, x, y, right, world, .25, 3) {}
	void doSomething() { /*does nothing*/ }
	std::string type() { return "Ice"; }
};

//--------------------------------------------------------Boulder-----------------------------------------------------------------------------

class Boulder : public Actor
{
public:
	Boulder(int x, int y, StudentWorld* world) :
		Actor(IID_BOULDER, x, y, down, world, 1.0, 1), delay(30), time(0) {
		setVisible(true);
	}

	std::string type();
	void doSomething();
	void drop();


private:
	int delay;
	int time;
};


//-------------------------------------------------------Oil Barrel--------------------------------------------------------------------------


class Oil_Barrel : public Actor
{
public:
	Oil_Barrel(int x, int y, StudentWorld* world) :
		Actor(IID_BARREL, x, y, right, world, 1.0, 2) {
		setVisible(false);
	}

	std::string type();
	void doSomething();
	void sonarSearch();

private:

};



//-------------------------------------------------------Gold Nugget--------------------------------------------------------------------------

class Gold : public Actor
{
public:
	Gold(int x, int y, StudentWorld* world, int s) :
		Actor(IID_GOLD, x, y, right, world, 1.0, 2), state(s), delay(100) {
		if (state == 1)
			setVisible(false);
		else if (state == 0)
			setVisible(false);

		setAliveStatus(true);
	}

	std::string type();
	void doSomething();
	void sonarSearch();

private:
	int state; // 0 - dropped 1 - spawned
	int delay;
};


//-------------------------------------------------------Water--------------------------------------------------------------------------

class Water : public Actor
{
public:
	Water(int x, int y, StudentWorld* world);

	std::string type();
	void doSomething();

private:
	int delay;
};



//-------------------------------------------------------Squirt--------------------------------------------------------------------------
class Squirt : public Actor
{
public:
	Squirt(int x, int y, StudentWorld* world);
	void doSomething();
	int hitRegulator(int x);
	std::string type();
	void damage();

private:
	int distanceLeft;
	int hit;
};


//-------------------------------------------------------Sonar--------------------------------------------------------------------------


class Sonar : public Actor {
public:

	Sonar(int x, int y, StudentWorld* world);

	std::string type();
	void doSomething();

private:
	int delay;

};

//--------------------------------------------------------Protester-----------------------------------------------------------------------------

class Protester : public Actor
{
public:
	Protester(int imageID, int x, int y, Direction startDir, float size, unsigned int depth, StudentWorld* world, int hlth);

	std::string type();

	virtual void damage(int damage); // this damage function right now is only intended to harm the iceman. so protesters hurt iceman

	virtual void p_move(); //might use, might not, who knows
	virtual void changeDir();

	virtual void shortestPath(int x, int y); //sets Iceman direction towards the shortest path out of the ice field

	virtual void gotGold();

	virtual void damageintake(int x);

protected:
	int state; // 0 - dead , 1 - alive, 2 - waiting
	int cooldown; // this is a state variable that will regulate the delay. this will trigger the delay. 1 its ready to shout. 2 its not ready to shoot.
	int delay; // this is to make the protester wait 15 ticks so it can shout again.
	int restDelay; //delay between moves
	int p_health;
	int numStepsLeft;
	int ticks; //checks for 200 ticks pass

};


//--------------------------------------------------------Regular Protester-----------------------------------------------------------------------------


class Regular_protester : public Protester {
public:
	//Regular_protester(int x, int y, StudentWorld* world);
	Regular_protester(int x, int y, StudentWorld* world) :
		Protester(IID_PROTESTER, x, y, left, 1.0, 1, world, 5) {}


	void doSomething();

};


//--------------------------------------------------Hardcore Protester---------------------------------------

class hardcoreprotester : public Protester {
public:
	hardcoreprotester(int x, int y, StudentWorld* world) :
		Protester(IID_HARD_CORE_PROTESTER, x, y, left, 1.0, 1, world, 20) {}

	bool findIceman(int x, int y, int maxDistance); //hone in on iceman

	void gotGold();
	void damageintake(int x);
	void doSomething();

};

#endif


/*

modified date 6/4/2021 @ 2:04pm


*/
