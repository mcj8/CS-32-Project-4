#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

using namespace std;


//ACTOR IMPLEMENTATION//
Actor::Actor(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world)
: GraphObject(imageID, startX, startY, dir, size, depth) {
    m_alive = true;   //Make alive
    m_world = world;  //initialize world
}

//Accessors
bool Actor::isAlive() const{ return m_alive; }

StudentWorld* Actor::world() const { return m_world; }

//Mutators
void Actor::setDead () { m_alive = false; }



//EARTH IMPLEMENTATION//
Earth::Earth(int startX, int startY, StudentWorld* world)
: Actor(TID_EARTH, startX, startY, right, 0.25, 3, world) {
    setVisible(true); //Set visible
}

void Earth::doSomething() {} //Does nothing



//HUMAN IMPLEMENTATION//
Human::Human(int imageID, int startX, int startY, Direction dir, unsigned int depth, StudentWorld* world)
: Actor(imageID, startX, startY, dir, 1.0, depth, world) { //all humans are size 1
    setVisible(true); //Set visible
}

//Accessors
int Human::getHp() { return m_hp; }

//Mutators
void Human::setHp(int hp) { m_hp = hp; }

void Human::loseHp(int lostHealth) { m_hp -= lostHealth; }

void Human::setAnnoyanceLevel(int annoyanceLevel) { m_annoyanceLevel = annoyanceLevel; }

void Human::increaseAnnoyanceLevel(int num) { m_annoyanceLevel += num; }




//TUNNELMAN IMPLEMENTATION//
TunnelMan::TunnelMan(StudentWorld* world)
: Human(TID_PLAYER, 30, 60, right, 0, world) {
    setHp(10);
    m_squirts = 5;
    m_sonarCharges = 1;
    m_goldNuggets = 0;
}

//Accessors
int TunnelMan::getSquirts() { return m_squirts; }

int TunnelMan::getGold() { return m_goldNuggets; }

int TunnelMan::getSonar() { return m_sonarCharges; }




//Mutators

void TunnelMan::addGold() { m_goldNuggets++; }

void TunnelMan::addSonar() { m_sonarCharges++; }

void TunnelMan::addSquirts() { m_squirts += 5; }

void TunnelMan::getAnnoyed(int hpLost) {
    loseHp(hpLost);
    if (getHp() <= 0) {
        setDead();
        world()->playSound(SOUND_PLAYER_GIVE_UP);
    }
}

void TunnelMan::shootWater() {
    Direction dir = world()->getTunnelMan()->getDirection();
    int x = getX();
    int y = getY();
    
    switch (dir) {
        case left:
            world()->addActor(new Squirt(x - 4, y, dir, world()));
            break;
        case right:
            world()->addActor(new Squirt(x + 4, y, dir, world()));
            break;
        case up:
            world()->addActor(new Squirt(x, y + 4, dir, world()));
            break;
        case down:
            world()->addActor(new Squirt(x, y - 4, dir, world()));
            break;
        default:
            break;
    }
}


void TunnelMan::doSomething() {
    
    if (!isAlive())
        return;
    
    int input;
    if (world()->getKey(input) == true) //user hit a key this tick
    {
        switch (input)
        {
            case (KEY_PRESS_ESCAPE): //kill character and exit game
                setDead();
                break;
            case (KEY_PRESS_SPACE): //... add a Squirt in front of the player...
                if (m_squirts != 0) {
                    //fire a Squirt into the oil field
                    world()->playSound(SOUND_PLAYER_SQUIRT);
                    shootWater();
                    m_squirts--;
                }
                break;
            case (KEY_PRESS_LEFT):
                //... move player to the left ...;
                if (getDirection() != left) {
                    setDirection(left);
                }
                else //is already facing that direction
                    //if (can move in that direction)
                    if (getX() >= 1 && !world()->boulderWithinRad(getX() - 1, getY(), 4)) //Check for out of bounds and boulders
                        moveTo(getX() - 1, getY());
                break;
            case (KEY_PRESS_RIGHT):
                //... move player to the right ...;
                if (getDirection() != right) {
                    setDirection(right);
                }
                else //is already facing that direction
                    //if (can move in that direction)
                    if (getX() <= 59 && !world()->boulderWithinRad(getX() + 1, getY(), 4))
                    moveTo(getX() + 1, getY());
                break;
            case (KEY_PRESS_UP):
                //... move player up ...;
                if (getDirection() != up) {
                    setDirection(up);
                }
                else //is already facing that direction
                    //if (can move in that direction)
                    if (getY() <= 59 && !world()->boulderWithinRad(getX(), getY() + 1, 4))
                    moveTo(getX(), getY() + 1);
                break;
            case (KEY_PRESS_DOWN):
                //... move player down ...;
                if (getDirection() != down) {
                    setDirection(down);
                }
                else //is already facing that direction
                    //if (can move in that direction)
                    if (getY() >= 1&& !world()->boulderWithinRad(getX(), getY() - 1, 4))
                    moveTo(getX(), getY() - 1);
                break;
                
            case (KEY_PRESS_TAB): //drop a goldNugget if TunnelMan has one
                if (m_goldNuggets > 0) {
                    world()->dropANugget();
                    m_goldNuggets--;
                }
                break;
            case ('Z'):
            case ('z'):
                if (m_sonarCharges > 0) {
                    //use sonar and illuminate blocks within 12 sqaure radius
                    world()->useSonarCharge();
                    m_sonarCharges--;
                }
                break;
        }
        if (world()->removeEarth(getX(), getY())) {
            world()->playSound(SOUND_DIG);
        }
    }
    return;
}




//PROTESTER IMPLEMENTATION//

Protester::Protester(int imageID, StudentWorld * world)
: Human(imageID, 60, 60, left, 0, world) {
    if (imageID == TID_HARD_CORE_PROTESTER)
        setHp(20);
    else
        setHp(5);
    m_numSquaresToMoveInCurrentDirection = (rand() % 53) + 8; //random value between 8-60
    m_state = stayInField;
    m_ticksToWaitBetweenMoves = std::max(0, 3 - ((int)(world->getLevel()) / 4));
}

void Protester::resetTickToWaitBetweenMoves() { m_ticksToWaitBetweenMoves = std::max(0, 3 - ((int)(world()->getLevel()) / 4)); }

void Protester::setState(int state) { m_state = state; }

void Protester::getStunned() {
    int N = max(50, 100 - int(world()->getLevel()*10));
    m_ticksToWaitBetweenMoves = N;
}

void Protester::getAnnoyed(int hpLost) {
    loseHp(hpLost);
    
    if (m_state == leaveOilField)
        return;
    
    if (getHp() > 0) { //If still alive and not leaving field
        world()->playSound(SOUND_PROTESTER_ANNOYED);
        getStunned();
    }
    
    else  { //m_hp < 0, should leave the field
        setState(leaveOilField);
        world()->playSound(SOUND_PROTESTER_GIVE_UP);
        m_ticksToWaitBetweenMoves = 0;
        
        if (hpLost == 10) //If boulder annoyed protester
            world()->increaseScore(500);
        else
            world()->increaseScore(100);
    }
}




bool Protester::isFacingTunnelMan(){
    //Check direction of protester and see if the protester is further in the opposite direction
    switch (getDirection()) {
        case GraphObject::left:
            return world()->getTunnelMan()->getX() <= getX();
            break;
        case GraphObject::right:
            return world()->getTunnelMan()->getX() >= getX();
            break;
        case GraphObject::up:
            return world()->getTunnelMan()->getY() >= getY();
            break;
        case GraphObject::down:
            return world()->getTunnelMan()->getY() <= getY();
            break;
        default:
            break;
    }
    return false;
}


bool Protester::isWithinLine(Direction dir) { //return true if nothing is blocking
    int tX = world()->getTunnelMan()->getX();
    int tY = world()->getTunnelMan()->getY();
    int distance = world()->distanceBetween(getX(), getY(), tX, tY);
    
    if (tX != getX() && tY != getY())
        return false;
    
    switch (dir) {
        case GraphObject::left:
            if (getX() > tX && getY() == tY) {
                for (int i = 0; i < distance - 5; i++) { //Check for earth/boulders to the left of protester
                    if (world()->overlapsEarth(getX() - i, getY()) || world()->overlapsBoulder(getX() - i, getY()))
                        return false; //if something is blocking
                }
            }
            break;
            
        case GraphObject::right:
            if (getX() < tX && getY() == tY) {
                for (int i = 0; i < distance - 5; i++) { //Check for earth/boulders to the left of protester
                    if (world()->overlapsEarth(getX() + i, getY()) || world()->overlapsBoulder(getX() + i, getY()))
                        return false;
                }
            }
            break;
            
        case GraphObject::up:
            if (getX() == tX && getY() < tY) {
                for (int i = 0; i < distance - 5; i++) { //Check for earth/boulders to the left of protester
                    if (world()->overlapsEarth(getX(), getY() + i) || world()->overlapsBoulder(getX(), getY() + i))
                        return false;
                }
            }
            break;
            
        case GraphObject::down:
            if (getX() == tX && getY() > tY) {
                for (int i = 0; i < distance - 5; i++) { //Check for earth/boulders to the left of protester
                    if (world()->overlapsEarth(getX(), getY() - i) || world()->overlapsBoulder(getX(), getY() - i))
                        return false;
                }
            }
            break;
        default:
            break;
    }
    
    return true;
}

Actor::Direction Protester::facePlayerDirection() {
    int tX = world()->getTunnelMan()->getX();
    int tY = world()->getTunnelMan()->getY();
    
    if (getX() == tX && getY() < tY)
        return up;
    if (getX() == tX && getY() > tY)
        return down;
    if (getX() < tX && getY() == tY)
        return right;
    if (getX() > tX && getY() == tY)
        return left;
    if (getX() == tX && getY() == tY)
        return getDirection();
    
    return none;
}

Actor::Direction Protester::randDirection() {
    
    int val = rand() % 4;
    switch (val) {
        case 0:
            return left;
            break;
        case 1:
            return right;
            break;
        case 2:
            return up;
            break;
        case 3:
            return down;
            break;
        default:
            return none;
            break;
    }
}


bool Protester::isDirBlocked() {
    Direction dir = getDirection();
    
    switch (dir) {
        case GraphObject::left:
            if (!world()->validXY(getX() - 1, getY()))
                return true;
            break;
        case GraphObject::right:
            if (!world()->validXY(getX() + 1, getY()))
                return true;
            break;
        case GraphObject::up:
            if (!world()->validXY(getX(), getY() + 1))
                return true;
            break;
        case GraphObject::down:
            if (!world()->validXY(getX(), getY() - 1))
                return true;
            break;
        default:
            break;
    }
    return false;
}

Actor::Direction Protester::changeDirForTurn() {
    
    int val = rand() % 2; //Rand variable in the case that both turns are viable
    Direction dir = getDirection();
    bool valid1 = false;
    bool valid2 = false;
    
    if (dir == right || dir == left) {
        if (world()->validXY(getX(), getY() + 1))
            valid1 = true; //Can turn up
        if (world()->validXY(getX(), getY() - 1))
            valid2 = true; //Can turn down
        if (valid1 && !valid2)
            return up;
        if (!valid1 && valid2)
            return down;
        if (valid1 && valid2) { //If both are valid
            if (val == 0)
                return up;
            else
                return down;
        }
    }
    if (dir == up || dir == down) {
        if (world()->validXY(getX() - 1, getY()))
            valid1 = true; //Can turn left
        if (world()->validXY(getX() + 1, getY()))
            valid2 = true; //Can turn right
        if (valid1 && !valid2)
            return left;
        if (!valid1 && valid2)
            return right;
        if (valid1 && valid2) { //If both are valid
            if (val == 0)
                return left;
            else
                return right;
        }
    }
    return none;
}

void Protester::moveForwardOne(GraphObject::Direction dir) {

    switch (dir) {
        case GraphObject::left:
            if (world()->validXY(getX()-1, getY()))
                moveTo(getX()-1, getY());
            break;
        case GraphObject::right:
            if (world()->validXY(getX()+1, getY()))
                moveTo(getX()+1, getY());
            break;
        case GraphObject::up:
            if (world()->validXY(getX(), getY()+1))
                moveTo(getX(), getY()+1);
            break;
        case GraphObject::down:
            if (world()->validXY(getX(), getY()-1))
                moveTo(getX(), getY()-1);
            break;
        default:
            break;
    }
}

void Protester::doSomething() {
    
    //Check 1: if currently alive
    if (!isAlive())
        return;
    //Check 2: if the protester is waiting until next move
    if (m_ticksToWaitBetweenMoves > 0) {
        m_ticksToWaitBetweenMoves--;
        return;
    }
    else {
        if (m_state != leaveOilField) {
            m_ticksToWaitBetweenMoves = max(0, 3 - int(world()->getLevel() / 4));
            m_turnTicks++;
            m_shoutTicks++;
        }
    }
    //Check 3: if the Regular Protester is in a leave-the-oil-field state
    if (m_state == leaveOilField) { //This state occurs when hp = 0
        if (getX() == 60 && getY() == 60) { //Reached exit point
            setDead();
            world()->protesterLeft();
            return;
        }
        else {
            //Set proper direction and move one step towards exit
            setDirection(world()->dirToLeave(getX(), getY()));
            moveForwardOne(world()->dirToLeave(getX(), getY()));
            return;
        }
    }
    //Check 4: if the protester should shout at the player
    if (world()->distanceBetween(getX(), getY(), world()->getTunnelMan()->getX(), world()->getTunnelMan()->getY()) <= 4 && isFacingTunnelMan()) {
        if (m_shoutTicks > 15) {
            world()->playSound(SOUND_PROTESTER_YELL);
            world()->getTunnelMan()->getAnnoyed(2);
            m_shoutTicks = 0;
            return;
        }
    }
    
    //if close enough to player, follow a path to the player (Hardcore Protesters only)
//    if (getID() == TID_HARD_CORE_PROTESTER) {
//        int M = 16 + (world()->getLevel() * 2);
//
//        if (world()->distanceBetween(getX(), getY(), world()->getTunnelMan()->getX(), world()->getTunnelMan()->getY()) <= M) {
//            setDirection(world()->dirToTM(getX(), getY()));
//            moveForwardOne(getDirection());
//            return;
//        }
//    }
    
    //Check 5: if the tunnelman is directly vertical/horizontal from the protester and not blocked by obstacles
    if (isWithinLine(facePlayerDirection()) && world()->distanceBetween(getX(), getY(), world()->getTunnelMan()->getX(), world()->getTunnelMan()->getY()) > 4) {
        setDirection(facePlayerDirection());
        moveForwardOne(getDirection());
        m_numSquaresToMoveInCurrentDirection = 0;
        return;
    }
    //Check 6: can't directly see Tunnelman
    m_numSquaresToMoveInCurrentDirection--;
    if (m_numSquaresToMoveInCurrentDirection <= 0) {
        setDirection(randDirection());
        //Change directions until not blocked
        while (isDirBlocked()) {
            setDirection(randDirection());
        }
        m_numSquaresToMoveInCurrentDirection = rand() % 53 + 8;
    }
    //Check 7: if at an intersection and can turn
    else if (changeDirForTurn() != none && m_turnTicks > 200) {
        setDirection(changeDirForTurn());
        m_numSquaresToMoveInCurrentDirection = rand() % 53 + 8;
        m_turnTicks = 0;
    }
    //Check 8: move forward in current direction
    moveForwardOne(getDirection());

    //Check 9: In case the direction is still blocked, reset numSquaresToMoveInCurrentDirection
    if (isDirBlocked())
        m_numSquaresToMoveInCurrentDirection = 0;
}


//REGULAR PROTESTER IMPLEMENTATION//
RegularProtester::RegularProtester(StudentWorld* world)
: Protester(TID_PROTESTER, world) { }

void RegularProtester::getBribed() {
    world()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    world()->increaseScore(25);
    setState(leaveOilField);
}

//HARDCORE PROTESTER IMPLEMENTATION//
HardcoreProtester::HardcoreProtester(StudentWorld* world)
: Protester(TID_HARD_CORE_PROTESTER, world) { }

void HardcoreProtester::getBribed() {
    world()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    world()->increaseScore(50);
    getStunned();
}





//BOULDER IMPLEMENTATION//
Boulder::Boulder(int startX, int startY, StudentWorld* world)
: Actor(TID_BOULDER, startX, startY, down, 1.0, 1, world) {
    setVisible(true);
    m_state = stable; //stable state
    m_tick = 0;
}

//Accessors
int Boulder::getState() { return m_state; }

int Boulder::getTick() { return m_tick; }

//Mutators
void Boulder::plusTick() { m_tick++; }

void Boulder::resetTick() { m_tick = 0; }

void Boulder::doSomething() {
    
    if (!isAlive())
        return;
    
    plusTick();
    
    int x = getX();
    int y = getY();
    
    //Stable state
    if (m_state == stable) {
        bool earthFound = false;
        //Check for earth beneath boulder
        for (int i = 0; i < 4; i++) {
            if (world()->earthFound(x + i, y - 1))
                earthFound = true;
        }
        //If no earth found, wait 30 ticks
        if (earthFound == false) {
            resetTick();
            m_state = waiting;
        }
    }
    //Waiting state
    else if (m_state == waiting) {
        //If 30 ticks have passed
        if (getTick() >= 30) {
            m_state = falling;
            world()->playSound(SOUND_FALLING_ROCK);
        }
    }
    //Falling state
    else if (m_state == falling) {
        moveTo(x, y - 1);
        
        //If the boulder hits earth
        for (int i = 0; i < 4; i++) {
            if (world()->earthFound(x + i, y - 1)) {
                setDead();
                return;
            }
        }
        //If the boulder hits another boulder
        for (int i = 0; i < 4; i++) {
            if (world()->overlapsBoulder(x + i, y - 1)) {
                setDead();
                return;
            }
        }
        //If the boulder hits the bottom of the oil field
        if (y == 1) {
            setDead();
            return;
        }
        //If boulder hits tunnelman
        if (world()->distanceBetween(x, y, world()->getTunnelMan()->getX(), world()->getTunnelMan()->getY()) <= 3) {
            world()->getTunnelMan()->getAnnoyed(10); //loseHp sets tunnelMan to dead
        }
        //if boulder hits protester
        if (world()->isProtesterWithinRadius(getX(), getY(), 3)) {
            world()->annoyProtesters(getX(), getY(), 3, 10);
        }
    }
}


//SQUIRT IMPLEMENTATION//
Squirt::Squirt(int startX, int startY, Direction dir, StudentWorld * world)
: Actor(TID_WATER_SPURT, startX, startY, dir, 1.0, 1, world) {
    m_distance = 4; //4 squares travel distance
    setVisible(true);
}

void Squirt::moveForward(GraphObject::Direction dir) {
    
    switch (dir) {
        case GraphObject::left:
            if (world()->validXY(getX() - 1, getY())
                /*&& world()->earthFound(getX() - 1, getY()) == false*/) {
                moveTo(getX()-1, getY());
                m_distance--;
            }
            else
                setDead();
            break;
        case GraphObject::right:
            if (world()->validXY(getX() - 1, getY())
                /*&& world()->earthFound(getX() + 1, getY()) == false*/) {
                moveTo(getX()+1, getY());
                m_distance--;
            }
            else
                setDead();
            break;
        case GraphObject::down:
            if (world()->validXY(getX() - 1, getY())
                /*&& world()->earthFound(getX(), getY() - 1) == false*/) {
                moveTo(getX(), getY()-1);
                m_distance--;
            }
            else
                setDead();
            break;
        case GraphObject::up:
            if (world()->validXY(getX() - 1, getY())
                /*&& world()->earthFound(getX(), getY() + 1) == false*/) {
                moveTo(getX(), getY()+1);
                m_distance--;
            }
            else
                setDead();
            break;
        default:
            break;
    }
}

void Squirt::doSomething() {
    //If can't move further
    if (m_distance <= 0) {
        setDead();
    }
    //Squirt all protesters in range
    if (world()->squirtProtesters(getX(), getY()))
        setDead();
    
    //Move forward one square if possible
    moveForward(getDirection());
}




//GOODIE IMPLEMENTATION//
Goodie::Goodie(int imageID, int startX, int startY, Direction dir, unsigned int depth, StudentWorld* world)
: Actor(imageID, startX, startY, dir, 1.0, depth, world) {
    resetTick();
}

void Goodie::resetTick() { m_tick = 0; }

void Goodie::setState(int state) { m_state = state; }

void Goodie::addTick() { m_tick++; }


int Goodie::getTick() { return m_tick; }

int Goodie::getState() { return m_state; }


//BARREL OF OIL IMPLEMENTATION//
OilBarrel::OilBarrel(int startX, int startY, StudentWorld * world)
: Goodie(TID_BARREL, startX, startY, right, 2, world){
    setVisible(false);
    //setVisible(true); //CHANGE BACK LATER //////////////////////////////
}

void OilBarrel::doSomething() {
    if (!isAlive())
        return;
    
    int tmX = world()->getTunnelMan()->getX();
    int tmY = world()->getTunnelMan()->getY();
    
    if (!isVisible() && world()->distanceBetween(tmX, tmY, getX(), getY()) <= 4) {
        setVisible(true);
        return;
    }
    
    if (world()->distanceBetween(tmX, tmY, getX(), getY()) <= 3) {
        setDead();
        world()->playSound(SOUND_FOUND_OIL);
        world()->increaseScore(1000);
        world()->foundBarrel();
    }
}

//SONAR KIT IMPLEMENTATION//
SonarKit::SonarKit(StudentWorld* world)
: Goodie(TID_SONAR, 0, 60, right, 2, world) {
    setVisible(true);
    setState(temporary);
}

void SonarKit::doSomething() {
    addTick();
    
    if (!isAlive())
        return;
    
    int tmX = world()->getTunnelMan()->getX();
    int tmY = world()->getTunnelMan()->getY();
        
    if (world()->distanceBetween(tmX, tmY, getX(), getY()) <= 3) {
        setDead();
        world()->playSound(SOUND_GOT_GOODIE);
        world()->increaseScore(75);
        world()->getTunnelMan()->addSonar();
    }
    
    if (getTick() >= std::max(100, 300 - (10 * static_cast<int>(world()->getLevel())) ))
        setDead();
}

//WATER POOL IMPLEMENTATION//
Water::Water(int startX, int startY, StudentWorld* world)
: Goodie(TID_WATER_POOL, startX, startY, right, 2, world) {
    setVisible(true);
}

void Water::doSomething() {
    addTick();
    
    if (!isAlive())
        return;
    
    int tmX = world()->getTunnelMan()->getX();
    int tmY = world()->getTunnelMan()->getY();
        
    if (world()->distanceBetween(tmX, tmY, getX(), getY()) <= 3) {
        setDead();
        world()->playSound(SOUND_GOT_GOODIE);
        world()->increaseScore(100);
        world()->getTunnelMan()->addSquirts();
    }
    
    if (getTick() >= std::max(100, 300 - (10 * (int)(world()->getLevel())) ))
        setDead();
    
}

//GOLD NUGGET IMPLEMENTATION//
GoldNugget::GoldNugget(int startX, int startY, StudentWorld* world, bool droppedByTunnelMan)
: Goodie(TID_GOLD, startX, startY, right, 2, world) {
    //Buried gold starts out invisible, dropped gold starts out visible
    //Buried gold starts as permanent state (until picked up), dropped gold starts out in temp state
    setVisible(droppedByTunnelMan);
    setState(permanent - droppedByTunnelMan);
    setPickupTM(!droppedByTunnelMan);
    setPickupProtesters(droppedByTunnelMan);
}

void GoldNugget::setPickupTM(bool val) { m_pickupTM = val; }

void GoldNugget::setPickupProtesters(bool val) { m_pickupProtesters = val; }


void GoldNugget::doSomething() {
    addTick();
    
    if (!isAlive())
        return;
    
    int tmX = world()->getTunnelMan()->getX();
    int tmY = world()->getTunnelMan()->getY();
    
    if (!isVisible() && world()->distanceBetween(tmX, tmY, getX(), getY()) <= 4) {
        setVisible(true);
        return;
    }
    
    if (m_pickupTM == true && world()->distanceBetween(tmX, tmY, getX(), getY()) <= 3) {
        setDead();
        world()->playSound(SOUND_GOT_GOODIE);
        world()->increaseScore(10);
        world()->getTunnelMan()->addGold();
    }
    
    if (m_pickupProtesters == true && world()->isProtesterWithinRadius(getX(), getY(), 3)) {
        setDead();
        world()->playSound(SOUND_PROTESTER_FOUND_GOLD);
        world()->bribeNearestProtester(getX(), getY());
    }
    
    //Delete expired gold
    if (getState() == temporary && getTick() >= 100)
        setDead();
}


