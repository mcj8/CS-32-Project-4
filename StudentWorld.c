#include "StudentWorld.h"
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <time.h>
#include <queue>

using namespace std;

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

GameWorld* createStudentWorld(string assetDir) { return new StudentWorld(assetDir); }

StudentWorld::StudentWorld(std::string assetDir)
    : GameWorld(assetDir)
{}

StudentWorld::~StudentWorld() {
}

//Accessor
TunnelMan* StudentWorld::getTunnelMan() { return m_TunnelMan; }



double StudentWorld::distanceBetween(double x1, double y1, double x2, double y2) {
    return sqrt( ((x2 - x1) * (x2- x1)) + ((y2 - y1) * (y2 - y1)) * 1.0 );
}

bool StudentWorld::earthFound(int x, int y) { //checks if any earth objects are at (x, y)
    if (earthPointer[x][y] != nullptr && earthPointer[x][y]->isAlive())
        return true;
    return false;
}

bool StudentWorld::overlapsEarth(int x, int y) { //If a 4x4 char has any pixels overlaping earth
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (earthPointer[x+i][y+j] != nullptr && earthPointer[x+i][y+j]->isAlive())
                return true;
        }
    }
        return false;
}

bool StudentWorld::overlapsBoulder(int x, int y) { //Checks if (x,y) contains a boulder pixel
    vector<Actor*>::iterator it;
        it = actorPointer.begin();
    while (it != actorPointer.end()) {
        if ((*it)->getID() == TID_BOULDER) {
            int X = (*it)->getX();
            int Y = (*it)->getY();
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (X+i == x && Y+j == j)
                        return true;
                }
            }
            
        }
        it++;
    }
    return false;
}

bool StudentWorld::boulderWithinRad(int x, int y, int rad) {
    vector<Actor*>::iterator it;
    it = actorPointer.begin();
    while (it != actorPointer.end()) {
        if ((*it)->getID() == TID_BOULDER) {
            if(distanceBetween(x, y, (*it)->getX(), (*it)->getY()) < rad)
                return true;
         }
         it++;
     }
    return false;
}


//Tick functions
int StudentWorld::getTick() { return m_tick; }

void StudentWorld::addTick() { m_tick++; }

void StudentWorld::resetTick() { m_tick = 0; }



bool StudentWorld::validXY(int x, int y) {
    if (x < 0 || x > 60 || y < 0 || y > 60) //Check bounds
        return false;
    if (overlapsEarth(x,y)) //Check for earth
        return false;
    if (overlapsBoulder(x, y)) //Check for boulders
        return false;
    return true;
}

bool StudentWorld::squirtProtesters(int x, int y) {
    bool val = false;
    vector<Actor*>::iterator it;
    it = actorPointer.begin();
    while (it != actorPointer.end()) {
        if((*it)->getID() == TID_PROTESTER || (*it)->getID() == TID_HARD_CORE_PROTESTER) {
            Protester* p = dynamic_cast<Protester*>(*it);
            if(distanceBetween(x, y, (*it)->getX(), (*it)->getY()) < 3.00) {
                p->getAnnoyed(2);
                val = true;
            }
        }
        it++;
    }
    return val;
}

void StudentWorld::addSonarandWater() {
    int currLevel = getLevel();
    int G = (currLevel * 25) + 300;
    
    int x = 0;
    int y = 0;
    
    if (rand() % G == 0) { //Should add sonar or water
        if (rand() % 5 == 0) { //1/5 chance of Sonar
            addActor(new SonarKit(this));
        }
        else {
            while (!validXY(x, y)) {
                x = rand() % 60 + 1;
                y = rand() % 60 + 1;
            }
            //Passing this loop means no earth was found
            addActor(new Water(x, y, this));
        }
    }
}

void StudentWorld::addProtesters() {
    int T = max(25, 200 - (int)getLevel()); //Time before adding new protester
    int P = min(15, (int)(2 + getLevel() * 1.5)); //max number of protesters per level
    int probabilityOfHardcore = min(90, (int)(getLevel() * 10 + 30));
    
    m_addProtesterTick++;

    if (m_protestersInField >= P)
        return;
    
    if (m_protestersInField == 0 || m_addProtesterTick > T) {
        if ( (rand() % 100) < probabilityOfHardcore)
            addActor(new HardcoreProtester(this));
        else
            addActor(new RegularProtester(this));
        m_addProtesterTick = 0;
        m_protestersInField++;
    }
}


void StudentWorld::dropANugget() {
    int x = getTunnelMan()->getX();
    int y = getTunnelMan()->getY();
    
    GoldNugget* p = new GoldNugget(x, y, this, true);
    p->setPickupTM(false);
    p->setPickupProtesters(true);
    addActor(p);
}

void StudentWorld::useSonarCharge() {
    int x = getTunnelMan()->getX();
    int y = getTunnelMan()->getY();
    
    vector<Actor*>::iterator it = actorPointer.begin();
    while (it != actorPointer.end()) {
        //Radius = 12
        if (distanceBetween(x, y, (*it)->getX(), (*it)->getY()) <= 12.0) {
            if (!(*it)->isVisible() && (*it)->isAlive())
                (*it)->setVisible(true);
        }
        it++;
    }
}


void StudentWorld::generateValidCoords(int &x, int &y) {
    
    bool valid = false;
    
    while (valid == false) {
        x = (rand() % 59) + 1;
        y = (rand() % 55) + 1;
        valid = true;
        vector<Actor*>::iterator it = actorPointer.begin();
        while (it != actorPointer.end()) {
            //Check each actor if the distance is too close
            if (distanceBetween(x, y, (*it)->getX(), (*it)->getY()) <= 6.0) {
                valid = false;
                break;
            }
            it++;
        }
        //Check if the object would be within the tunnel
        if (((26 <= x && x <= 33) && (1 <= y && y <= 59)))
            valid = false;
    }
}

void StudentWorld::addActor(Actor* actor) {
    actorPointer.push_back(actor);
}



bool StudentWorld::removeEarth(int x, int y) { //removes earth that the TunnelMan touches
    
    bool val = false;
    //Loop through TunnelMan's image (4x4)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            //Check for valid coordinates
            if ((x + i >= 0 && x + i < 64) && (y + j >= 0 && y + j <60))
            {
                if (earthFound(x+i, y+j))
                {
                    earthPointer[x+i][y+j]->setDead();
                    earthPointer[x+i][y+j]->setVisible(false);
                    val = true;
                }
            }
        }
    }
    return val;
}



void StudentWorld::foundBarrel() { m_barrels--; }

void StudentWorld::cleanUpDeadActors() {
    
    vector<Actor*>::iterator it = actorPointer.begin();
    while (it != actorPointer.end()) {
        if(!(*it)->isAlive()) {
            delete (*it);
            it = actorPointer.erase(it);
        }
            else
                it++;
    }
}


//INIT FUNCTION//

int StudentWorld::init()
{
    srand(time(NULL)); //generate different seeds for rand()
    resetTick();

    m_addProtesterTick = 0;
    m_protestersInField = 0;
    
    //Create tunnelman
    m_TunnelMan = new TunnelMan(this);
    //Create earth array
    createEarth();
    //Create initial oil field objects
    createBoulderGoldOil();
    
    return GWSTATUS_CONTINUE_GAME;
}

//CREATE OIL FIELD FUNCTIONS
void StudentWorld::createEarth() {
    for (int x = 0; x < VIEW_WIDTH; x++) {
        for (int y = 0; y < VIEW_HEIGHT; y++) {
            if ( (x >= 30 && x <= 33 && y >= 4) || y >= 60) //middle shaft/above y limit
                earthPointer[x][y] = nullptr;
            else
               earthPointer[x][y] = new Earth(x, y, this);
        }
    }
}

void StudentWorld::createBoulderGoldOil() {
    int B = min( ((int)getLevel() / 2) + 2, 9);
    int G = max(5 - ((int)getLevel() / 2), 2);
    int L = min(2 + (int)getLevel(), 21);
    int x, y;
    //Create Boulders
    for (int i = 0; i < B; i++) {
        generateValidCoords(x, y);
        while (true) {
            if (y >= 20 && x <= 59) {
            addActor(new Boulder(x, y, this));
            bool removeEarthHere = removeEarth(x, y); //Remove earth where boulders are
            break;
            }
        else
            generateValidCoords(x, y);
        }
    }
    //Create Gold
    for (int i = 0; i < G; i++) {
        generateValidCoords(x, y);
        GoldNugget* p = new GoldNugget(x, y, this, false);
        p->setPickupTM(true);
        p->setPickupProtesters(false);
        addActor(p);
    }
    //Create Oil
    m_barrels = L;
    for (int i = 0; i < L; i++) {
        generateValidCoords(x, y);
        addActor(new OilBarrel(x, y, this));
    }
}




bool StudentWorld::isProtesterWithinRadius (int x, int y, int rad) {
    vector<Actor*>::iterator it;
    it = actorPointer.begin();
    while (it != actorPointer.end()) {
        if((*it)->getID() == TID_PROTESTER) {
            if (distanceBetween(x, y, (*it)->getX(), (*it)->getY()) <= rad) {
                return true;
            }
        }
        it++;
    }
    return false;
}



bool StudentWorld::annoyProtesters(int x, int y, int rad, int amount) {
    bool val = false;
    vector<Actor*>::iterator it;
    it = actorPointer.begin();
    while (it != actorPointer.end()) {
        if((*it)->getID() == TID_PROTESTER) {
            Protester* p = dynamic_cast<Protester *>(*it);
            if (distanceBetween(x, y, (p)->getX(), (p)->getY()) <= rad) {
                p->getAnnoyed(amount);
                val = true;
            }
        }
        it++;
    }
    return val;
}

void StudentWorld::bribeNearestProtester(int x, int y) {
    vector<Actor*>::iterator it;
    it = actorPointer.begin();
    while (it != actorPointer.end()) {
        if( ((*it)->getID() == TID_PROTESTER) || ((*it)->getID() == TID_HARD_CORE_PROTESTER) ) {
            if (distanceBetween(x, y, (*it)->getX(), (*it)->getY()) <= 3) {
                if ((*it)->getID() == TID_PROTESTER) {
                    RegularProtester* p = dynamic_cast<RegularProtester*>(*it);
                    p->getBribed();
                    return;
                }
                else {
                    HardcoreProtester* hp = dynamic_cast<HardcoreProtester*>(*it);
                    hp->getBribed();
                    return;
                }
                
            }
        }
        it++;
    }
}



//MOVE FUNCTION//

int StudentWorld::move()
{
    
    m_tick++;
    
    updateDisplayText();
    
    //Call tunnelman to do something
    m_TunnelMan->doSomething();
    
    //call each actor to do something
    vector<Actor*>::iterator it = actorPointer.begin();
    while (it != actorPointer.end()) {
        if ((*it)->isAlive())
            (*it)->doSomething();
        it++;
    }
    //Add actors when needed
    addSonarandWater();
    addProtesters();
    
    //generate paths to exit and tunnelman
    createPathToXY(m_exitPath, 60, 60);
    createPathToXY(m_hardcoreNavigation, getTunnelMan()->getX(), getTunnelMan()->getY());
    
    //Clear all dead actors
    cleanUpDeadActors();
    
    //Check if TunnelMan is alive
    if (!m_TunnelMan->isAlive()) {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    //Check if player beat level
    if (m_barrels == 0) {
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    //Otherwise, continue the game
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::createPathToXY(GraphObject::Direction game[VIEW_HEIGHT][VIEW_WIDTH], int x, int y) {
    //make a 2D array representation of the game
    for (int i = 0; i < VIEW_WIDTH; i++) {
        for (int j = 0; j < VIEW_HEIGHT; j++) {
            game[i][j] = GraphObject::none;
        }
    }
    
    queue<coords> q;
    q.push(coords(x, y)); //ending (x,y)
    
    //BFS to create a path to (x,y)
    while (q.empty() == false) {
        //(X,Y) are coords of current location in the BFS
        int X = q.front().m_x;
        int Y = q.front().m_y;
        q.pop();
        
        //Left search
        if (validXY(X-1, Y) && game[X-1][Y] == GraphObject::none) {
            game[X-1][Y] = GraphObject::right;
            q.push(coords(X-1,Y));
        }
        //Right search
        if (validXY(X+1, Y) && game[X+1][Y] == GraphObject::none) {
            game[X+1][Y] = GraphObject::left;
            q.push(coords(X+1,Y));
        }
        //Down search
        if (validXY(X, Y-1) && game[X][Y-1] == GraphObject::none) {
            game[X][Y-1] = GraphObject::up;
            q.push(coords(X,Y-1));
        }
        //Up search
        if (validXY(X, Y+1) && game[X][Y+1] == GraphObject::none) {
            game[X][Y+1] = GraphObject::down;
            q.push(coords(X,Y));
        }
    }
}

GraphObject::Direction StudentWorld::dirToLeave(int x, int y) { return m_exitPath[x][y]; }

GraphObject::Direction StudentWorld::dirToTM(int x, int y) { return m_hardcoreNavigation[x][y]; }

void StudentWorld::protesterLeft() { m_protestersInField--; }

void StudentWorld::updateDisplayText() {
    int level = getLevel();
    int lives = getLives();
    int health = m_TunnelMan->getHp();
    int squirts = m_TunnelMan->getSquirts();
    int gold = m_TunnelMan->getGold();
    int barrelsLeft = m_barrels;
    int sonar = m_TunnelMan->getSonar();
    int score = getScore();
    string s = formatDisplayText(level, lives, health, squirts, gold, barrelsLeft, sonar, score);
    setGameStatText(s);
}

string StudentWorld::formatDisplayText(int level, int lives, double health, int squirts, int gold, int barrelsLeft, int sonar, int score) {
    ostringstream oss;
    
        oss.fill('0');
        oss << "Scr: ";
        oss << setw(6) << score;
        oss.fill(' ');
        oss << "  Lvl: ";
        oss << setw(2) << level;
        oss << "  Lives: ";
        oss << setw(1) << lives;
        oss << "  Hlth: ";
        oss << setw(3) << health * 10;
        oss << '%';
        oss << "  Wtr: ";
        oss << setw(2) << squirts;
        oss << "  Gld: ";
        oss << setw(2) << gold;
        oss << "  Sonar: ";
        oss << setw(2) << sonar;
        oss << "  Oil Left: ";
        oss << setw(2) << barrelsLeft;

        return oss.str();
}


void StudentWorld::cleanUp() {
    
    //Delete Earth objects
    for (int x = 0; x < VIEW_WIDTH; x++) {
        for (int y = 0; y < VIEW_HEIGHT; y++) {
            delete earthPointer[x][y];
            earthPointer[x][y] = nullptr;
        }
    }
    //Delete all actors in the actorPointer vector
    vector<Actor*>::iterator it;
        it = actorPointer.begin();
        while (it != actorPointer.end()) {
            Actor *ptr = *it;
            delete ptr;
            it = actorPointer.erase(it);
        }
    
    //Delete TunnelMan
    delete m_TunnelMan;
    m_TunnelMan = nullptr;
    
}
