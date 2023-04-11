#ifndef GRABTHETREASURE_H
#define GRABTHETREASURE_H

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <chrono>
#include <string>
#include <conio.h>

class Hero;
class Weapon;
class Treasure;
class Interactable;
class Map;
class UIHeroInterface;
class HeroAI;

enum Graphics {
    EMPTY = ' ',
    TREASURE = 'k',
    POTION = 'i',
    TRAP = 'c',
    WEAPON = 'a',
    HERO = 'h',
    WALL = 'x',
    EXIT = 'j',
    MONSTER = 's'
};

enum Direction {
    NODIRECTION = -1,
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3
};

const bool enableColoring = true;

enum Color {
    BLACK       = 0,
    BLUE        = 1,
    GREEN       = 2,
    TEAL        = 3,
    RED         = 4,
    PURPLE      = 5,
    YELLOW      = 6,
    LIGHTGREY   = 7,

    GREY        = 8,
    LIGHTBLUE   = 9,
    LIGHTGREEN  = 10,
    LIGHTTEAL   = 11,
    LIGHTRED    = 12,
    LIGHTPURPLE = 13,
    LIGHTYELLOW = 14,
    WHITE       = 15,
};


enum Settings {
    CONSOL_WIDTH = 80,
    CONSOL_HEIGHT = 25
};

void showConsoleCursor(bool showFlag);
void resizeConsolWindow(int width, int height);
void setConsolCursorTo(int line, int cloumn = 0);
void setConsolColor(Color textColor, Color backgroundColor = Color::BLACK);

std::string readFile(std::string filePath);
void waitAnyConsolKeyPress(int timeOffsetMilliSec = 0);
bool handleKeyInputs();

enum GameState {
    GAMESTART,
    GAMEPLAYING,
    GAMEFORFEIT,
    GAMEVICTORY,
    GAMEOVER
};

class Game {
    static GameState state;
    static Map map;
    static UIHeroInterface ui;
    static HeroAI ai;
public:
    static void setState(GameState newState);
    static bool handleKeyInputs();
    static void checkGameEnd();
    static GameState getState();
    static Hero* getHero();
    static Map* getMap();
    static void render();
};

class Drawable {
public:
    virtual void draw(std::ostream& out) = 0;
};

class UIHeroInterface : public Drawable {
    int state = 0;
    Hero* hero = NULL;
public:
    UIHeroInterface(Hero& hero);
    void draw(std::ostream& out = std::cout);
};

class Tile : public Drawable{
protected:
    int x;
    int y;
    Tile* neighbours[4] = { NULL, NULL, NULL, NULL };
    Hero* presentHero = NULL;
    Interactable* presentInteractable = NULL;

public:
    Tile(int x, int y);
    int getX();
    int getY();
	virtual bool acceptHero(Hero& hero);
    virtual Tile* AIProbeStep(bool interacting, Hero& hero, Tile* from);
    void removeHero();
	Tile* getNeighbourIn(Direction dir);
    Tile** getNeighbours();
    void setNeighbourIn(Direction dir, Tile& neighbour);
    Direction getDirection(Tile* to);
    void setInteractable(Interactable& interactable);
    void removeInteractable();
    virtual void draw(std::ostream& out);
};

class Wall : public Tile {
public:
    Wall(int x, int y);
    virtual bool acceptHero(Hero& hero);
    virtual void draw(std::ostream& out);
    virtual Tile* AIProbeStep(bool interacting, Hero& hero, Tile* from);
};

class Trap : public Tile {
    bool isArmed = true;
public:
    Trap(int x, int y);
    bool acceptHero(Hero& hero);
    virtual void draw(std::ostream& out);
    virtual Tile* AIProbeStep(bool interacting, Hero& hero, Tile* from);
};

class Exit : public Tile {
public:
    Exit(int x, int y);
    bool acceptHero(Hero& hero);
    virtual void draw(std::ostream& out);
};

class Hero : public Drawable {
    int health = 2;
    Tile* position = NULL;
    Weapon* weapon = NULL;
    Treasure* treasure = NULL;
    bool escaped = false;

    friend class UIHeroInterface;

public:
    void move(Direction dir);
    void setPosition(Tile& position);
    Tile* getPosition();
    void heal();
    void damage();
    int getHealth();
    void setHealth(int amount);
    bool hasTreasure();
    bool hasWeapon();
    bool isEscaped();
    void wield(Weapon& weapon);
    void stash(Treasure& treasure);
    void escape();
    virtual void draw(std::ostream& out);

};

class Interactable : public Drawable {
public:
    virtual void interact(Hero& hero) = 0;
    virtual Hero AIProbeInteract(const Hero& hero) = 0;
};

class Monster : public Interactable {
public:
    void interact(Hero& hero);
    Hero AIProbeInteract(const Hero& hero);
    virtual void draw(std::ostream& out);
};

class Weapon : public Interactable {
public:
    void interact(Hero& hero);
    Hero AIProbeInteract(const Hero& hero);
    virtual void draw(std::ostream& out);
};

class Potion : public Interactable {
    void interact(Hero& hero);
    Hero AIProbeInteract(const Hero& hero);
    virtual void draw(std::ostream& out);
};

class Treasure : public Interactable {
public:
    void interact(Hero& hero);
    Hero AIProbeInteract(const Hero& hero);
    virtual void draw(std::ostream& out);
};

class Map {
    std::vector<Tile*> tiles;
    std::vector<Interactable*> interactables;
    Hero* hero = NULL;
    int width;
    int height;

    void createMapFrom(std::string fileString);
    void copyHeroState(Hero* otherHero);
    void destroyMap();
    void createField(char c, std::pair<int, int> coordinate);
    Tile* createTile(char c, std::pair<int, int> coordinate);
    Interactable* createInteractable(char c);
    void setNeighbourConnections();
    std::string toString() const;
    Tile* findFirst(char toFind);
    std::vector<Tile*> findAll(char toFind);

    friend class HeroAI;

public:
    Map(std::string fileString);
    Map(const Map& other);
    Map& operator=(const Map& other);
    ~Map();

    void drawAll(std::ostream& out = std::cout);
    Tile* getTile(int index);
    Tile* getTile(int x, int y);
    Hero* getHero() const;
    Map clone();
};

class HeroAI {
    Map& playMap;
    bool executing = false;
    std::vector<Tile*> path;
    std::vector<Tile*> aStarSearchPath(Map& currentMap, bool interacting, Tile* start, Tile* goal);
    int heuristic(Tile* from, Tile* to);
    
    Tile* chooseTargetWithTraps(Map& map);
    Tile* chooseTarget(Map& map, std::string& errorText);
    Tile* isOnPath(std::vector<Tile*> path, char toFind);
    std::vector<Tile*> moveHeroToTarget(Map& map, Tile* target);
    std::vector<Tile*> findShortestPathToAny(char goalTile, Map& map, bool interacting, Tile* start);

public:
    HeroAI(Map& map);
    std::vector<std::pair<int, int>> planWithTraps();
    std::vector<std::pair<int, int>> plan();
    void executePlan(std::vector<std::pair<int, int>> coordinates );
    void toggleAI();
};

#endif /* GRABTHETREASURE_H */