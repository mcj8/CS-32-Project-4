#ifndef ACTOR_H_
#define ACTOR_H_

#include <iostream>
#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp


class StudentWorld;

//Base class for all game objects
class Actor : public GraphObject {
public:
    //Constructor/Destructor
    Actor(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world);
    virtual ~Actor() {}
    
    //Accessors
    bool isAlive() const;
    StudentWorld* world() const;
    
    //Mutators
    virtual void doSomething() = 0;
    void setDead();
    
    
private:
    bool m_alive;
    StudentWorld *m_world;
};


class Earth : public Actor {
public:
    //Constructor/Destructor
    Earth(int startX, int startY, StudentWorld* world);
    virtual ~Earth() {}
    
    //Mutators
    void doSomething();

private:
};

//Base class for TunnelMan, Protestors, and Hardcore Protestors
class Human : public Actor {
public:
    //Constructor
    Human(int imageID, int startX, int startY, Direction dir, unsigned int depth, StudentWorld* world);
    virtual ~Human() {}
    
    //Accessors
    int getHp();
    
    //Mutators
    virtual void getAnnoyed(int hpLost) = 0;
    void setHp(int hp);
    void loseHp(int hp);
    void setAnnoyanceLevel(int annoyanceLevel);
    void increaseAnnoyanceLevel(int num);
    
    
private:
    int m_hp;
    int m_annoyanceLevel;
};


class TunnelMan : public Human {
public:
    //Constructor/Destructor
    TunnelMan(StudentWorld* world);
    virtual ~TunnelMan() {}
    
    //Accessors
    int getSquirts();
    int getGold();
    int getSonar();
    
    //Mutators
    void doSomething();
    void getAnnoyed(int hpLost);
    void shootWater();
    void addGold();
    void addSonar();
    void addSquirts();

private:
    int m_squirts;
    int m_sonarCharges;
    int m_goldNuggets;
};

//Protester state constants
const int leaveOilField = 201;
const int stayInField = 202;

class Protester : public Human {
public:
    //Constructor/Destructor
    Protester(int imageID, StudentWorld * world);
    virtual ~Protester() {}
    
    virtual void getBribed() = 0;
    void doSomething();
    void getAnnoyed(int hpLost);
    void getStunned();
    void setState(int state);
    void resetTickToWaitBetweenMoves();
    
    //Helpers
    bool isFacingTunnelMan();
    bool isWithinLine(Direction dir); //Tunnelman is acccessible within direct line of protester
    Direction facePlayerDirection();
    Direction randDirection();
    bool isDirBlocked(); //true if blocked, false if not
    Direction changeDirForTurn(); //Changes direction at intersection
    void moveForwardOne(Direction dir);
    
    
private:
    int m_numSquaresToMoveInCurrentDirection;
    int m_state;
    int m_ticksToWaitBetweenMoves;
    int m_shoutTicks;
    int m_turnTicks;
    bool annoyedByBoulder;
};

class RegularProtester : public Protester {
public:
    RegularProtester(StudentWorld* world);
    virtual ~RegularProtester() {}

    void getBribed();
private:
};

class HardcoreProtester : public Protester {
public:
    HardcoreProtester(StudentWorld* world);
    virtual ~HardcoreProtester() {}
    
    void getBribed();
private:
};

//Boulder state constants
const int stable = 101;
const int waiting = 102;
const int falling = 103;

class Boulder : public Actor {
public:
    //Constructor/Destructor
    Boulder(int startX, int startY, StudentWorld* world);
    virtual ~Boulder() {}
    
    //Mutators
    virtual void doSomething();
    void plusTick();
    void resetTick();
    
    //Accessors
    int getState();
    int getTick();
    
private:
    int m_state; 
    int m_tick;
};


class Squirt : public Actor {
public:
    Squirt(int startX, int startY, Direction dir, StudentWorld* world);
    virtual ~Squirt() {}
    
    void doSomething();
    void moveForward(GraphObject::Direction dir);
    
private:
    int m_distance;
};

//Goodie state constants
const int temporary = 104;
const int permanent = 105;

class Goodie : public Actor { //Abstract base class for Sonar Kits, Gold Nuggets, Water, Barrels of oil
public:
    Goodie(int imageID, int startX, int startY, Direction dir, unsigned int depth, StudentWorld* world);
    virtual ~Goodie() {}
    
    virtual void doSomething() = 0;
    
    void resetTick();
    void addTick();
    void setState(int state);
    
    int getTick();
    int getState();
    
private:
    int m_tick;
    int m_state;
};

class OilBarrel : public Goodie {
public:
    OilBarrel(int startX, int startY, StudentWorld * world);
    virtual ~OilBarrel() {}
    void doSomething();
private:
};

class SonarKit : public Goodie {
public:
    SonarKit(StudentWorld * world);
    virtual ~SonarKit() {}
    void doSomething();
private:
};

class Water : public Goodie {
public:
    Water(int startX, int startY, StudentWorld* world);
    virtual ~Water() {}
    void doSomething();
private:
};

class GoldNugget : public Goodie {
public:
    GoldNugget(int startX, int startY, StudentWorld* world, bool droppedByTunnelMan);
    virtual ~GoldNugget() {}
    void doSomething();
    
    void setPickupTM(bool val);
    void setPickupProtesters(bool val);
 
private:
    bool m_pickupTM;
    bool m_pickupProtesters;
    int m_tick;
};


#endif // ACTOR_H_
