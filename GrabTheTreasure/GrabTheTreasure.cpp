#include "GrabTheTreasure.h"


bool Tile::acceptHero(Hero& hero) {
    if (presentHero == &hero)
        return true;

    if (presentInteractable != NULL) {
        presentInteractable->interact(hero);
        presentInteractable = NULL;
    }

    if (presentHero == NULL) {
        presentHero = &hero;
        if( hero.getPosition() != NULL)
            hero.getPosition()->removeHero();
        hero.setPosition(*this);
        return true;
    }
    else
        return false;
}

void Tile::removeHero()
{
    presentHero = NULL;
}

Tile* Tile::getNeighbourIn(Direction dir) {
    return neighbours[dir];
};

void Tile::setNeighbourIn(Direction dir, Tile& neighbour) {
    neighbours[dir] = &neighbour;
}
void Tile::setInteractable(Interactable& interactable)
{
    presentInteractable = &interactable;
}

void Tile::removeInteractable()
{
    presentInteractable = NULL;
}

void Tile::draw()
{
    if (presentHero != NULL)
        presentHero->draw();
    else if (presentInteractable != NULL)
        presentInteractable->draw();
    else
        std::cout << (char)EMPTY;
}

Tile::Tile(int x, int y) : x(x), y(y){ }

int Tile::getX()
{
    return x;
}

int Tile::getY()
{
    return y;
}

Tile** Tile::getNeighbours()
{
    return neighbours;
}

Tile* Tile::AIProbeStep(Hero& hero, Tile* from)
{
    if (presentInteractable != NULL) {
        Hero probeHero = presentInteractable->AIProbeInteract(hero);

        if (probeHero.getHealth() == 0) {
            return NULL;
        }
    }

    return this;
}

Wall::Wall(int x, int y) : Tile(x,y) {}

bool Wall::acceptHero(Hero& hero) {
    return false;
}

void Wall::draw()
{
    setConsolColor(BLACK, GREY);
    std::cout << (char)WALL;
    setConsolColor(WHITE);
}

Tile* Wall::AIProbeStep(Hero& hero, Tile* from) {
    return NULL;
}

Trap::Trap(int x, int y) : Tile(x,y) {}

bool Trap::acceptHero(Hero& hero){
    if (isArmed) {
        Tile* originalPos = hero.getPosition();
        Direction moveDir;

        if (neighbours[UP] == originalPos)
            moveDir = DOWN;
        if (neighbours[RIGHT] == originalPos)
            moveDir = LEFT;
        if (neighbours[DOWN] == originalPos)
            moveDir = UP;
        if (neighbours[LEFT] == originalPos)
            moveDir = RIGHT;

        originalPos->removeHero();
        hero.setPosition(*this);

        if (!neighbours[moveDir]->acceptHero(hero)) {
            hero.damage();
        }

        isArmed = false;

        return true;
    }
    else
        return false;
}

void Trap::draw()
{
    if (isArmed) {
        setConsolColor(GREY);
        std::cout << (char)TRAP;
        setConsolColor(WHITE);
    }
    else { 
        setConsolColor(BLACK, GREY);
        std::cout << (char)WALL;
        setConsolColor(WHITE);
    }
}

Tile* Trap::AIProbeStep(Hero& hero, Tile* from) {
    Tile* targetTile = NULL;
    Direction targetDirection;

    if (neighbours[UP] == from)
        targetTile = neighbours[DOWN];
    else if (neighbours[RIGHT] == from)
        targetTile = neighbours[LEFT];
    else if (neighbours[DOWN] == from)
        targetTile = neighbours[UP];
    else if (neighbours[LEFT] == from)
        targetTile = neighbours[RIGHT];

    return targetTile;
}


bool Exit::acceptHero(Hero& hero){
    hero.escape();

    hero.getPosition()->removeHero();
    hero.setPosition( *this );
    this->presentHero = &hero;

    return true;
}

void Exit::draw()
{
    setConsolColor(WHITE, GREY);
    std::cout << (char)EXIT;
    setConsolColor(WHITE);
}

Exit::Exit(int x, int y) : Tile(x,y)
{
}
    
void Hero::move(Direction dir) {
    Tile* nextPosition = position->getNeighbourIn(dir);

    if (nextPosition != NULL && health > 0) {
        nextPosition->acceptHero(*this);
    }
}

void Hero::setPosition(Tile& position) {
    this->position = &position;
}

Tile* Hero::getPosition()
{
    return position;
}

void Hero::heal() {
    if (health < 2)
        health++;
}

void Hero::damage() {
    if (weapon != NULL)
        health -= 1;
    if (weapon == NULL) {
        health = 0;
    }
}

int Hero::getHealth()
{
    return health;
}

bool Hero::hasTreasure()
{
    return treasure != NULL;
}

void Hero::wield(Weapon& weapon) {
    this->weapon = &weapon;
}

void Hero::stash(Treasure& treasure) {
    this->treasure = &treasure;
}

void Hero::escape() {
    escaped = true;
}

void Hero::draw()
{
    if (health > 0) {
        std::cout << (char)HERO;
    }
    else {
        setConsolColor(BLACK, RED);
        std::cout << (char)HERO;
        setConsolColor(WHITE);
    }
}

bool Hero::isEscaped()
{
    return escaped;
}

void Monster::interact(Hero& hero){
    hero.damage();
}

void Monster::draw()
{
    setConsolColor(LIGHTPURPLE);
    std::cout << (char)MONSTER;
    setConsolColor(WHITE);
}

Hero Monster::AIProbeInteract(const Hero& hero)
{
    Hero probeHero = Hero(hero);
    probeHero.damage();
    return probeHero;
}

void Weapon::interact(Hero& hero){
    hero.wield(*this);
}

void Weapon::draw()
{
    setConsolColor(LIGHTYELLOW);
    std::cout << (char)WEAPON;
    setConsolColor(WHITE);
}

Hero Weapon::AIProbeInteract(const Hero& hero)
{
    Hero probeHero = hero;
    probeHero.wield( *this );
    return probeHero;
}

void Potion::interact(Hero& hero)
{
    hero.heal();
}

void Potion::draw()
{
    setConsolColor(LIGHTGREEN);
    std::cout << (char)POTION;
    setConsolColor(WHITE);
}

Hero Potion::AIProbeInteract(const Hero& hero)
{
    Hero probeHero = hero;
    probeHero.heal();
    return probeHero;
}

void Treasure::interact(Hero& hero){
    hero.stash(*this);
}

void Treasure::draw()
{
    setConsolColor(YELLOW);
    std::cout << (char)TREASURE;
    setConsolColor(WHITE);
}

Hero Treasure::AIProbeInteract(const Hero& hero)
{
    Hero probeHero = hero;
    probeHero.stash( *this );
    return probeHero;
}

void Map::createField(char c, std::pair<int, int> coordinate)
{
    Tile* newTile = createTile(c, coordinate);
    if (newTile == NULL)
        return;

    if (c == 'h') {
        Hero* newHero = new Hero();
        if ( newTile->acceptHero(*newHero) ) {
            newHero->setPosition(*newTile);
        }
        this->hero = newHero;
    }
    else {
        Interactable* newInteractable = createInteractable(c);

        if (newInteractable != NULL) {
            newTile->setInteractable(*newInteractable);
            interactables.push_back(newInteractable);
        }
    }

    tiles.push_back(newTile);
}

Tile* Map::createTile(char c, std::pair<int, int> coordinate)
{
    Tile* newTile = NULL;

    switch (c)
    {
    case EMPTY:
        newTile = new Tile(coordinate.first, coordinate.second);
        break;
    case WALL:
        newTile = new Wall(coordinate.first, coordinate.second);
        break;
    case TRAP:
        newTile = new Trap(coordinate.first, coordinate.second);
        break;
    case EXIT:
        newTile = new Exit(coordinate.first, coordinate.second);
        break;
    default:
        newTile = new Tile(coordinate.first, coordinate.second);
        break;
    }

    return newTile;
}

Interactable* Map::createInteractable(char c)
{
    Interactable* newInteractable = NULL;

    switch (c)
    {
    case TREASURE:
        newInteractable = new Treasure();
        break;
    case POTION:
        newInteractable = new Potion();
        break;
    case WEAPON:
        newInteractable = new Weapon();
        break;
    case MONSTER:
        newInteractable = new Monster();
        break;
    default:
        break;
    }

    return newInteractable;
}

void Map::setNeighbourConnections()
{
    for (int i = 0; i < width * height; i++) {
        Tile* upT    = NULL;
        Tile* rightT = NULL;
        Tile* downT  = NULL;
        Tile* leftT  = NULL;
        
        if (i >= width)
            upT = tiles[i - width];
        if (i % width != width - 1)
            rightT = tiles[i + 1];
        if (i < (height - 1) * width)
            downT = tiles[i + width];
        if (i % width != 0)
            leftT = tiles[i - 1];

        if(upT != NULL)
            tiles[i]->setNeighbourIn(UP, *upT);
        if(rightT != NULL)
            tiles[i]->setNeighbourIn(RIGHT, *rightT);
        if(downT != NULL)
            tiles[i]->setNeighbourIn(DOWN, *downT);
        if(leftT != NULL)
            tiles[i]->setNeighbourIn(LEFT, *leftT);
    }
}

Map::Map(std::string filePath)
{
    std::ifstream inFileStream(filePath);
    std::string mapLine;
    std::string mapString = "";
    int mapHeight = 0;
    size_t maxMapWidth = 0;

    try {
        while (getline(inFileStream, mapLine)) {
            std::istringstream inStringStream(mapLine);

            if (mapLine.size() > maxMapWidth)
                maxMapWidth = mapLine.size();

            mapString.append(mapLine);
            mapHeight++;
        }
    }
    catch (std::exception e) {
        std::cout << "Hiba a térkép fáj(\"" + filePath + "\") olvasásakor...\n" << e.what();
    }

    this->width = maxMapWidth;
    this->height = mapHeight;

    for (int i = 0; i < width * height; i++) {
        createField( mapString[i], std::pair<int, int>( i%width, i/width ) );
    }

    setNeighbourConnections();
}

void Map::drawAll()
{
    setConsolCursorTo(((int)CONSOL_HEIGHT - 5) / 2 - height / 2);

    for (int i = 0; i < width * height; i++) {
        if (i % width == 0) {
            std::cout << std::endl;
            
            int leftOffsetCount = ((int)CONSOL_WIDTH - width) / 2;
            std::string leftOffset = "";
            leftOffset.append(leftOffsetCount, ' ');
            std::cout << leftOffset;
        }

        tiles[i]->draw();
    }
}

Hero* Map::getHero()
{
    return hero;
}

Map::~Map()
{
    for (int i = 0; i < tiles.size(); i++) {
        delete tiles[i];
    }
    for (int i = 0; i < interactables.size(); i++) {
        delete interactables[i];
    }
    if(hero != NULL)
        delete hero;
}

Tile* Map::getTile(int index)
{
    return tiles[index];
}

Tile* Map::getTile(int x, int y)
{
    for (int i = 0; i < width * height; i++) {
        if (tiles[i]->getX() == x && tiles[i]->getY() == y)
            return tiles[i];
    }

    return NULL;
}

UIHeroInterface::UIHeroInterface(Hero& hero)
{
    this->hero = &hero;
}

void UIHeroInterface::draw()
{
    state++;
    state = state % 2;

    std::string weapon = "   ?   ";
    std::string heart1 = "--";
    std::string heart2 = "--";
    std::string treasure = "   ?   ";

    if (hero != NULL) {
        if (hero->weapon != NULL)
            weapon = "C~|===>";
        if (hero->health > 0)
            heart1 = "<3";
        if (hero->health > 1)
            heart2 = "<3";
        if (hero->treasure != NULL)
            treasure = ".oO8Oo.";
    }

    setConsolCursorTo( (int)CONSOL_HEIGHT - 3 );

    int leftOffsetCount = ((int)CONSOL_WIDTH - 51) / 2;
    std::string leftOffset = "";
    leftOffset.append(leftOffsetCount, ' ');

    setConsolColor(GREY);
    std::cout << leftOffset;
    std::cout << '\xDA'; //┌
    for (int i = 0; i < 51; i++) {
        std::cout << '\xC4'; //─
    }
    std::cout << '\xBF' << std::endl; //┐

    std::cout << leftOffset;
    std::cout << '\xBA'; //║
    std::cout << "  ~[ ";

    setConsolColor(LIGHTYELLOW);
    std::cout << weapon;

    setConsolColor(GREY);
    std::cout << " ]~  ~[ ";

    state % 2 == 0 ? setConsolColor(RED) : setConsolColor(LIGHTRED);
    std::cout << heart1;

    setConsolColor(GREY);
    std::cout << " ]~&~[ ";

    state % 2 == 0 ? setConsolColor(LIGHTRED) : setConsolColor(RED);
    std::cout << heart2;

    setConsolColor(GREY);
    std::cout << " ]~  ~[ ";

    setConsolColor(YELLOW);
    std::cout << treasure;

    setConsolColor(GREY);
    std::cout << " ]~  ";
    std::cout << '\xB3' << std::endl; //│

    std::cout << leftOffset;
    std::cout << '\xC8'; //╚
    for (int i = 0; i < 51; i++) {
        std::cout << '\xCD'; //═
    }
    std::cout << '\xBC' << std::endl; //╝
    setConsolColor(WHITE);

}

void showConsoleCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO     cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}

void resizeConsolWindow(int width, int height)
{
    HANDLE hOut;
    SMALL_RECT DisplayArea = { 0, 0, 0, 0 };

    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DisplayArea.Right = width;
    DisplayArea.Bottom = height;

    SetConsoleWindowInfo(hOut, TRUE, &DisplayArea);

    CONSOLE_SCREEN_BUFFER_INFO scrBufferInfo;
    GetConsoleScreenBufferInfo(hOut, &scrBufferInfo);

    short winWidth = scrBufferInfo.srWindow.Right - scrBufferInfo.srWindow.Left+1;
    short winHeight = scrBufferInfo.srWindow.Bottom - scrBufferInfo.srWindow.Top+1;

    short scrBufferWidth = scrBufferInfo.dwSize.X;
    short scrBufferHeight = scrBufferInfo.dwSize.Y;

    COORD newSize;
    newSize.X = winWidth;
    newSize.Y = winHeight;

    SetConsoleScreenBufferSize(hOut, newSize);
}

void setConsolCursorTo(int line, int column)
{
    COORD coord;
    coord.X = column;
    coord.Y = line;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleCursorPosition(hConsole, coord);
}

void setConsolColor(Color textColor, Color backgroundColor)
{
    if (enableColoring) {
        int finalColor = backgroundColor << 4 | textColor;
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, finalColor);
    }
}

std::string readFile(std::string filePath)
{
    std::ifstream inFileStream(filePath);
    std::string line;
    std::string finalFile = "";

    try {
        while (getline(inFileStream, line)) {
            std::istringstream inStringStream(line);
            finalFile.append(line);
            finalFile.push_back('\n');
        }
    }
    catch (std::exception e) {
        std::cout << "Hiba a \"" << filePath << "\" fájl olvasásakor...\n" << e.what();
    }

    return finalFile;
}

void waitAnyConsolKeyPress(int timeOffsetMilliSec) {
    using namespace std::chrono;

    milliseconds offset = milliseconds(timeOffsetMilliSec);
    milliseconds startTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

    bool waiting = true;
    while (waiting) {
        _getch();
        if (startTime + offset < duration_cast<milliseconds>(system_clock::now().time_since_epoch())) {
            waiting = false;
        }
    }
}

bool Game::handleKeyInputs()
{
    bool inputArrived = false;
    Direction moveDirection;

    if (GetAsyncKeyState(VK_LEFT) & 0x01) {
        inputArrived = true;
        moveDirection = LEFT;
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x01) {
        inputArrived = true;
        moveDirection = RIGHT;
    }
    if (GetAsyncKeyState(VK_UP) & 0x01) {
        inputArrived = true;
        moveDirection = UP;
    }
    if (GetAsyncKeyState(VK_DOWN) & 0x01) {
        inputArrived = true;
        moveDirection = DOWN;
    }
    if (GetAsyncKeyState(VK_ESCAPE) & 0x01) {
        setState(GAMEOVER);
        render();
    }

    if (inputArrived && getHero() != NULL) {
        getHero()->move(moveDirection);
    }

    return inputArrived;
}

GameState Game::state = GAMESTART;
Map Game::map = Map("map.txt");
UIHeroInterface Game::ui = UIHeroInterface(*map.getHero());

void Game::setState(GameState newState)
{
    if (state == GAMESTART && newState == GAMESTART) {
        if (IsValidCodePage(65001))
            SetConsoleOutputCP(65001);
    }
    if (state == GAMESTART && newState == GAMEPLAYING) {

        if (IsValidCodePage(852))
            SetConsoleOutputCP(852);

        state = newState;
    }
    else if (state == GAMEPLAYING) {
        if (newState == GAMEFORFEIT || newState == GAMEVICTORY || newState == GAMEOVER) {

            if (IsValidCodePage(65001))
                SetConsoleOutputCP(65001);

            state = newState;
        }
    }
}

GameState Game::getState()
{
    return state;
}

void Game::render() {
    if (state == GAMEPLAYING) {
        system("CLS");
        map.drawAll();
        ui.draw();


    }
    else if (state == GAMESTART || state == GAMEFORFEIT || state == GAMEOVER || state == GAMEVICTORY) {
        std::string filePath;
        int waitTimeMs = 0;

        switch (state)
        {
        case GAMESTART:
            filePath = "ui_start.txt";
            break;
        case GAMEFORFEIT:
            filePath = "ui_forfeit.txt";
            break;
        case GAMEVICTORY:
            filePath = "ui_victory.txt";
            break;
        case GAMEOVER:
            filePath = "ui_gameover.txt";
            break;
        }

        std::string scene = readFile(filePath);
        system("CLS");
        std::cout << scene;

        if (waitTimeMs > 0)
            waitAnyConsolKeyPress(waitTimeMs);
    } 
}

void Game::checkGameEnd()
{
    if (map.getHero()->getHealth() == 0) {
        render();
        Sleep(2000);

        setState(GAMEOVER);
        render();
    }
    else if (map.getHero()->isEscaped()) {
        if (map.getHero()->hasTreasure()) {
            setState(GAMEVICTORY);
            render();
        }
        else {
            setState(GAMEFORFEIT);
            render();
        }
    }
}

Map* Game::getMap()
{
    return &map;
}

Hero* Game::getHero()
{
    return map.getHero();
}

HeroAI::HeroAI(Map& map) : playMap(map){ }

std::pair<bool,int> getCostFrom(std::map<Tile*, int> collection, Tile* toFind) {
    for (auto tileNode : collection) {
        if (tileNode.first == toFind)
            return std::pair<bool, int>( true, tileNode.second );
    }
    return std::pair<bool, int>(false, 0);
}

std::vector<Tile*> reconstructPath(std::map<Tile*, Tile*>& collection, Tile* start, Tile* goal) {
    std::vector<Tile*> path;
    Tile* current = goal;

    if (collection.find(goal) == collection.end())
        return path;

    while (current != start) {
        path.push_back(current);
        current = collection[current];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}

void HeroAI::aStarSearchPath(Tile* start, Tile* goal) {
    using namespace std;
    map<Tile*, int> waveFront;
    
    map<Tile*, int> costsSoFar;
    map<Tile*, Tile*> cameFrom;

    waveFront.insert( pair<Tile*, int>(start,0) );
    costsSoFar.insert( pair<Tile*, int>(start, 0) );
    cameFrom.insert( pair<Tile*, Tile*>(start, start) );

    while ( !waveFront.empty() ) {
        Tile* current = waveFront.begin()->first;
        int currentMin = waveFront.begin()->second;
        for (auto element : waveFront) {
            if (element.second < currentMin)
                current = element.first;
        }
        waveFront.erase(current);

        if (current == goal)
            break;

        Tile** neighbours = current->getNeighbours();
        for (int i = 0; i < 4; i++) {
            if (neighbours[i] != NULL ) {
                
                Tile* neighbour = neighbours[i]->AIProbeStep(*playMap.getHero(), current);
                if (neighbour != NULL) {
                    auto foundCost = getCostFrom(costsSoFar, current);

                    int currentCost = foundCost.first ? foundCost.second : playMap.width * playMap.height + 1;
                    int newCost = currentCost + 1;

                    auto foundNeighbourCost = getCostFrom(costsSoFar, neighbour);
                    if (!foundNeighbourCost.first || newCost < foundNeighbourCost.second) {
                        costsSoFar.insert(pair<Tile*, int>(neighbour, newCost));
                        int priority = newCost + heuristic(neighbour, goal);
                        waveFront.insert(pair<Tile*, int>(neighbour, priority));
                        cameFrom.insert(pair<Tile*, Tile*>(neighbour, current));
                    }
                }
            }
        }
    }

    vector<Tile*> path = reconstructPath(cameFrom, start, goal);
}

int HeroAI::heuristic(Tile* from, Tile* to)
{
    return abs(to->getX() - from->getX()) + abs(to->getY() - from->getY());
}
