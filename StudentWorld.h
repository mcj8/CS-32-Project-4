#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "GameController.h"
#include <string>
#include <vector>
#include <sstream>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    //Constructor/Destructor
    StudentWorld(std::string assetDir);
    virtual ~StudentWorld();

    //Accessors
    TunnelMan* getTunnelMan();
    
    
    //Object checker functions
    double distanceBetween(double x1, double y1, double x2, double y2);
    bool earthFound(int x, int y); //earth at (x,y)
    bool overlapsEarth(int x, int y); //checks if earth within 4x4 square
    bool overlapsBoulder(int x, int y); //checks if boulder within 4x4 square
    bool boulderWithinRad(int x, int y, int rad); //checks if boulder is within rad units
    bool validXY(int x, int y); //Checks for bounds (60, 60), earth, and boulders
    
    //Protester functions
    bool isProtesterWithinRadius(int x, int y, int rad);//returns true if any protester is within rad of (x,y)
    bool annoyProtesters(int x, int y, int rad, int amount); //annoys any protester within rad from (x,y)
    void bribeNearestProtester(int x, int y);
    void createPathToXY(GraphObject::Direction game[VIEW_HEIGHT][VIEW_WIDTH], int x, int y); //Finds a path to (x, y) for protesters
    GraphObject::Direction dirToLeave(int x, int y);
    GraphObject::Direction dirToTM(int x, int y);
    void protesterLeft();
    
    //Tick functions 
    int getTick();
    void addTick();
    void resetTick();
    
    //Oil field creation functions
    void createEarth();
    void createBoulderGoldOil();
    
    //Other actor creation
    void addSonarandWater();
    void addProtesters();
    void dropANugget();

    //Helper functions
    void useSonarCharge();
    bool squirtProtesters(int x, int y); //Squirt all protesters within range of (x,y)
    bool removeEarth(int x, int y);
    void generateValidCoords(int &x, int &y);
    void foundBarrel();
    void cleanUpDeadActors();
    void addActor(Actor* actor);
    
    //Display text funcitons
    void updateDisplayText();
    std::string formatDisplayText(int level, int lives, double health, int squirts, int gold, int barrelsLeft, int sonar, int score);
    
    //Main functions to run the game
    virtual int init();
    virtual int move();
    virtual void cleanUp();

private:
    Earth* earthPointer[VIEW_WIDTH][VIEW_HEIGHT];
    std::vector <Actor*> actorPointer; //holds everything except TunnelMan and earth
    TunnelMan* m_TunnelMan;
    int m_barrels;
    int m_tick;
    int m_addProtesterTick;
    int m_protestersInField;
    GraphObject::Direction m_exitPath[VIEW_WIDTH][VIEW_HEIGHT];
    GraphObject::Direction m_hardcoreNavigation[VIEW_WIDTH][VIEW_HEIGHT];
    
    struct coords {
        coords(int x, int y) {
            m_x = x;
            m_y = y;
        }
        int m_x;
        int m_y;
    };
};

#endif // STUDENTWORLD_H_
