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

void Tile::removeHero(){
    presentHero = NULL;
}

Tile* Tile::getNeighbourIn(Direction dir) {
    return neighbours[dir];
};

void Tile::setNeighbourIn(Direction dir, Tile& neighbour) {
    neighbours[dir] = &neighbour;
}

Direction Tile::getDirection(Tile* to) {
    if (neighbours[UP] == to)
        return UP;
    else if (neighbours[RIGHT] == to)
        return RIGHT;
    else if (neighbours[DOWN] == to)
        return DOWN;
    else if (neighbours[LEFT] == to)
        return LEFT;

    return NODIRECTION;
}

void Tile::setInteractable(Interactable& interactable){
    presentInteractable = &interactable;
}

void Tile::removeInteractable(){
    presentInteractable = NULL;
}

void Tile::draw(std::ostream& out){
    if (presentHero != NULL)
        presentHero->draw(out);
    else if (presentInteractable != NULL)
        presentInteractable->draw(out);
    else
        out << (char)EMPTY;
}

Tile::Tile(int x, int y) : x(x), y(y){ }

int Tile::getX(){
    return x;
}

int Tile::getY(){
    return y;
}

Tile** Tile::getNeighbours(){
    return neighbours;
}

Tile* Tile::AIProbeStep(bool interacting, Hero& hero, Tile* from){
    if (interacting && presentInteractable != NULL) {
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

void Wall::draw(std::ostream& out){
    setConsolColor(BLACK, GREY);
    out << (char)WALL;
    setConsolColor(WHITE);
}

Tile* Wall::AIProbeStep(bool interacting, Hero& hero, Tile* from) {
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

void Trap::draw(std::ostream& out){
    if (isArmed) {
        setConsolColor(GREY);
        out << (char)TRAP;
        setConsolColor(WHITE);
    }
    else { 
        setConsolColor(BLACK, GREY);
        out << (char)WALL;
        setConsolColor(WHITE);
    }
}

Tile* Trap::AIProbeStep(bool interacting, Hero& hero, Tile* from) {
    if (!isArmed)
        return NULL;

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

void Exit::draw(std::ostream& out){
    setConsolColor(WHITE, GREY);
    out << (char)EXIT;
    setConsolColor(WHITE);
}

Exit::Exit(int x, int y) : Tile(x,y) {}
    
void Hero::move(Direction dir) {
    Tile* nextPosition = position->getNeighbourIn(dir);

    if (nextPosition != NULL && health > 0) {
        nextPosition->acceptHero(*this);
    }
}

void Hero::setPosition(Tile& position) {
    this->position = &position;
}

Tile* Hero::getPosition(){
    return position;
}

void Hero::heal() {
    if (health < 2)
        health++;
}

void Hero::damage() {
    if (weapon != NULL && health > 0)
        health -= 1;
    if (weapon == NULL) {
        health = 0;
    }
}

int Hero::getHealth(){
    return health;
}

bool Hero::hasTreasure(){
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

void Hero::draw(std::ostream& out){

    if (health > 0) {
        if(Game::isAIExecuting()){
            setConsolColor(WHITE, BLUE);
        }
        out << (char)HERO;
    }
    else {
        setConsolColor(BLACK, RED);
        out << (char)HERO;
    }
    setConsolColor(WHITE);
}

bool Hero::isEscaped(){
    return escaped;
}

void Hero::setHealth(int amount){
    health = amount;
}

bool Hero::hasWeapon(){
    return weapon != NULL;
}

void Monster::interact(Hero& hero){
    hero.damage();
}

void Monster::draw(std::ostream& out){
    setConsolColor(LIGHTPURPLE);
    out << (char)MONSTER;
    setConsolColor(WHITE);
}

Hero Monster::AIProbeInteract(const Hero& hero){
    Hero probeHero = Hero(hero);
    probeHero.damage();
    return probeHero;
}

void Weapon::interact(Hero& hero){
    hero.wield(*this);
}

void Weapon::draw(std::ostream& out){
    setConsolColor(LIGHTYELLOW);
    out << (char)WEAPON;
    setConsolColor(WHITE);
}

Hero Weapon::AIProbeInteract(const Hero& hero){
    Hero probeHero = hero;
    probeHero.wield( *this );
    return probeHero;
}

void Potion::interact(Hero& hero){
    hero.heal();
}

void Potion::draw(std::ostream& out){
    setConsolColor(LIGHTGREEN);
    out << (char)POTION;
    setConsolColor(WHITE);
}

Hero Potion::AIProbeInteract(const Hero& hero){
    Hero probeHero = hero;
    probeHero.heal();
    return probeHero;
}

void Treasure::interact(Hero& hero){
    hero.stash(*this);
}

void Treasure::draw(std::ostream& out){
    setConsolColor(YELLOW);
    out << (char)TREASURE;
    setConsolColor(WHITE);
}

Hero Treasure::AIProbeInteract(const Hero& hero){
    Hero probeHero = hero;
    probeHero.stash( *this );
    return probeHero;
}

void Map::createMapFrom(std::string fileString) {

    std::string mapString = "";
    int mapHeight = 0;
    size_t maxMapWidth = 0;

    while (!fileString.empty()) {
        std::string mapLine = fileString.substr(0, fileString.find('\n'));
        fileString.erase(0, fileString.find('\n') + 1);

        if (mapLine.size() > maxMapWidth)
            maxMapWidth = mapLine.size();

        mapString.append(mapLine);
        mapHeight++;
    }

    this->width = maxMapWidth;
    this->height = mapHeight;

    for (int i = 0; i < width * height; i++) {
        createField(mapString[i], std::pair<int, int>(i % width, i / width));
    }

    setNeighbourConnections();

}

void Map::copyHeroState(Hero* otherHero){

    if (otherHero->hasWeapon()) {
        Weapon* newWeapon = new Weapon();
        hero->wield(*newWeapon);
        this->interactables.push_back(newWeapon);
    }
    if (otherHero->hasTreasure()) {
        Treasure* newTreasure = new Treasure();
        hero->stash(*newTreasure);
        this->interactables.push_back(newTreasure);
    }
    hero->setHealth(otherHero->getHealth());
}

void Map::destroyMap(){

    for (int i = 0; i < tiles.size(); i++) {
        delete tiles[i];
    }
    for (int i = 0; i < interactables.size(); i++) {
        delete interactables[i];
    }
    if (hero != NULL)
        delete hero;
}

void Map::createField(char c, std::pair<int, int> coordinate){

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

Tile* Map::createTile(char c, std::pair<int, int> coordinate){
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

Interactable* Map::createInteractable(char c){
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

void Map::setNeighbourConnections(){

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

std::string Map::toString() const{
    std::stringstream mapStream;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = y * width + x;
            tiles[i]->draw( mapStream );
        }
        
        mapStream << std::endl;
    }

    return mapStream.str();
}

Tile* Map::findFirst(char toFind){
    std::stringstream characters;

    for (Tile* tile : tiles) {
        tile->draw(characters);
    }

    size_t position = characters.str().find(toFind);
    if (position != std::string::npos) {
        return tiles[position];
    }

    return NULL;
}

std::vector<Tile*> Map::findAll(char toFind){
    std::vector<Tile*> foundTargets;

    for(Tile* tile : tiles ){
        std::stringstream character;
        tile->draw( character );
        if( character.str()[0] == toFind )
            foundTargets.push_back( tile );
    }

    return foundTargets;
}

Map::Map(std::string fileString){
    createMapFrom(fileString);
}

Map::Map(const Map& other){
    createMapFrom( other.toString() );
    copyHeroState( other.getHero() );
}

Map& Map::operator=(const Map& other){
    if(this == &other)
        return *this;

    destroyMap();

    createMapFrom( other.toString() );
    copyHeroState( other.getHero() );
}

int Map::getHeight(){
    return height;
}

int Map::getWidth() {
    return width;
}

Map::~Map(){
    destroyMap();
}

void Map::drawAll(std::ostream& out){
    setConsolCursorTo( (CONSOL_HEIGHT - 5) / 2 - height / 2);

    for (int i = 0; i < width * height; i++) {
        if (i % width == 0) {
            std::cout << std::endl;
            
            int leftOffsetCount = (CONSOL_WIDTH - width) / 2;
            std::string leftOffset = "";
            leftOffset.append(leftOffsetCount, ' ');
            std::cout << leftOffset;
        }

        tiles[i]->draw(out);
    }
}

Hero* Map::getHero() const{
    return hero;
}

Tile* Map::getTile(int index){
    return tiles[index];
}

Tile* Map::getTile(int x, int y){

    for (int i = 0; i < width * height; i++) {
        if (tiles[i]->getX() == x && tiles[i]->getY() == y)
            return tiles[i];
    }

    return NULL;
}

UIHeroInterface::UIHeroInterface(Hero& hero){
    this->hero = &hero;
}

void UIHeroInterface::draw(std::ostream& out){

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

    setConsolCursorTo( CONSOL_HEIGHT - 3 );

    int leftOffsetCount = (CONSOL_WIDTH - 51) / 2;
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

void showText(int x, int y, std::string text)
{
    bool inputArrived = false;
    int currentCharacter = 0;
    setConsolCursorTo(y, x);

    while( !inputArrived && currentCharacter < text.size()){
        std::cout << text[currentCharacter];
        currentCharacter++;

        Sleep(100);
    }
}

void showConsoleCursor(bool showFlag){

    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO     cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}

void resizeConsolWindow(int width, int height){

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

void setConsolCursorTo(int line, int column){

    COORD coord;
    coord.X = column;
    coord.Y = line;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleCursorPosition(hConsole, coord);
}

void setConsolCursorToOnMap(int x, int y){
    int mapYStart = (CONSOL_HEIGHT - 5) / 2 - Game::getMap()->getHeight() / 2;
    int mapXStart = (CONSOL_WIDTH - Game::getMap()->getWidth()) / 2;

    setConsolCursorTo(mapYStart+1 + y, mapXStart + x);
}

void setConsolColor(Color textColor, Color backgroundColor){

    if (enableColoring) {
        int finalColor = backgroundColor << 4 | textColor;
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, finalColor);
    }
}

std::string readFile(std::string filePath){

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

bool Game::handleKeyInputs(){

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
    if (GetAsyncKeyState(0x4D /*m*/) & 0x01) {
        ai.toggleAI();
    }

    if (inputArrived && getHero() != NULL) {
        getHero()->move(moveDirection);
    }

    return inputArrived;
}

GameState Game::state = GAMESTART;
Map Game::map = Map( readFile("map.txt") );
UIHeroInterface Game::ui = UIHeroInterface(*map.getHero());
HeroAI Game::ai = HeroAI( Game::map );

void Game::setState(GameState newState){
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

GameState Game::getState(){
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

void Game::checkGameEnd(){

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

Map* Game::getMap(){
    return &map;
}

Hero* Game::getHero(){
    return map.getHero();
}

bool Game::isAIExecuting(){
    return ai.isAIExecuting();
}

void Game::animateStartScreen(int frameIndex){
    frameIndex = frameIndex % 14;

    std::string animationFile = readFile("gem_animation.txt");
    int startLine = 5;
    int startColumn = 37;

    std::string delimiter = "\n";
    size_t pos = 0;
    std::vector<std::string> lines;
    while ((pos = animationFile.find(delimiter)) != std::string::npos) {
        lines.push_back(animationFile.substr(0, pos));
        animationFile.erase(0, pos + delimiter.length());
    }

    setConsolCursorTo(startLine, startColumn);
    std::cout << lines[frameIndex];

    Sleep(100);
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

bool HeroAI::checkValidPath(Map map, std::vector<std::pair<int, int>> pathCoordinates){
    if(pathCoordinates.size() == 0)
        return true;

    Hero* hero = map.getHero();

    for(int next = 1; next < pathCoordinates.size(); next++){
        int current = next-1;
        Tile* currentTile = map.getTile( pathCoordinates[current].first, pathCoordinates[current].second );
        Tile* nextTile = map.getTile( pathCoordinates[next].first, pathCoordinates[next].second );

        if(hero->getPosition() == currentTile){
            Direction moveDirection = currentTile->getDirection(nextTile);
            map.getHero()->move(moveDirection);
        }

        if(hero->getHealth() == 0)
            return false;
    }

    Tile* lastTile = map.getTile( pathCoordinates.back().first, pathCoordinates.back().second );
    if(hero->getPosition() == lastTile){
        return true;
    }

    return false;
}

std::vector<Tile*> HeroAI::aStarSearchPath(Map currentMap, bool interacting, Tile* start, Tile* goal) {
    using namespace std;
    if (goal == NULL)
        return vector<Tile*>();

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
                
                Tile* finalTile = neighbours[i]->AIProbeStep(interacting , *currentMap.getHero(), current);
                if (finalTile != NULL) {
                    auto foundCost = getCostFrom(costsSoFar, current);

                    int currentCost = foundCost.first ? foundCost.second : currentMap.width * currentMap.height + 1;
                    int newCost = currentCost + 1;

                    auto foundNeighbourCost = getCostFrom(costsSoFar, neighbours[i]);
                    if (!foundNeighbourCost.first || newCost < foundNeighbourCost.second) {
                        costsSoFar.insert(pair<Tile*, int>(neighbours[i], newCost));
                        
                        int priority = newCost + heuristic(neighbours[i], goal);
                        if (finalTile != neighbours[i])
                            waveFront.insert(pair<Tile*, int>(finalTile, priority));
                        waveFront.insert(pair<Tile*, int>(neighbours[i], priority));

                        cameFrom.insert(pair<Tile*, Tile*>(neighbours[i], current));
                    }
                }
            }
        }
    }

    vector<Tile*> reconstructedPath = reconstructPath(cameFrom, start, goal);
    if(interacting){
        if( checkValidPath(currentMap, toCoordinates(reconstructedPath)) ){
            return reconstructedPath;
        }
        else{
            return vector<Tile*>();
        }
    }

    return reconstructedPath;
}

std::vector<std::pair<int, int>> HeroAI::toCoordinates(const std::vector<Tile*>& path){
    std::vector<std::pair<int, int>> coordinates;

    for(auto tile : path){
        coordinates.push_back( std::pair<int, int>(tile->getX(), tile->getY()) );
    }

    return coordinates;
}

std::vector<std::pair<int, int>> HeroAI::concatCoordinates(const std::vector<std::pair<int, int>>& front, const std::vector<std::pair<int, int>>& back) {
    std::vector<std::pair<int, int>> full = front;
    full.insert( full.end(), back.begin(), back.end() );
    return full;
}

int HeroAI::heuristic(Tile* from, Tile* to){
    return abs(to->getX() - from->getX()) + abs(to->getY() - from->getY());
}

std::vector<std::pair<int, int>> HeroAI::plan() {

    Tile* heroPosition = playMap.getHero()->getPosition();
    setConsolCursorToOnMap(heroPosition->getX(), heroPosition->getY());
    setConsolColor(WHITE, BLUE);
    std::cout << "?";
    setConsolColor(WHITE);

    validPaths.clear();
    searchPaths(playMap, std::vector<std::pair<int,int>>() );

    if(validPaths.size() == 0)
        return std::vector<std::pair<int, int>>();

    std::vector<std::pair<int, int>> shortestPath = validPaths[0];

    for(auto path : validPaths){
        if(path.size() < shortestPath.size())
            shortestPath = path;
    }

    return shortestPath;
}

void HeroAI::searchPaths(Map map, std::vector<std::pair<int, int>> coordinatesSoFar){

    Hero* hero = map.getHero();
    Tile* heroPosition = hero->getPosition();

    std::vector<Tile*> directPath = findShortestPathToAny((char)TREASURE, map, true, heroPosition);
    if(directPath.size() > 0){
        std::vector<std::pair<int, int>> fullPath = concatCoordinates(coordinatesSoFar, toCoordinates(directPath));
        if(validPaths.size() == 0 || validPaths.size() != 0 && fullPath.size() < validPaths[0].size() )
            validPaths.push_back( fullPath );
    }

    std::vector<Tile*> anyPath = findShortestPathToAny((char)TREASURE, map, false, heroPosition);
    if(anyPath.size() > 0 ){

        Tile* monsterTile = isOnPath(anyPath, (char)MONSTER);
        if( monsterTile != NULL ){

            if( hero->hasWeapon() ){

                if( hero->getHealth() == 2 ){
                    std::vector<Tile*> movement = moveHeroToTarget( map, monsterTile );
                    searchPaths( map, concatCoordinates(coordinatesSoFar, toCoordinates(movement)) );
                }
                else{

                    std::vector<Tile*> potionPath = findShortestPathToAny((char)POTION, map, false, heroPosition);
                    if(potionPath.size() > 0){
                        std::vector<Tile*> movement = moveHeroToTarget( map, potionPath.back() );
                        searchPaths( map, concatCoordinates(coordinatesSoFar, toCoordinates(movement)) );
                    }
                    else{
                        //Error there is no free path to any potion
                    }
                }
            }
            else{
                
                std::vector<Tile*> weaponPath = findShortestPathToAny((char)WEAPON, map, false, heroPosition);
                if(weaponPath.size() > 0){
                    std::vector<Tile*> movement = moveHeroToTarget( map, weaponPath.back() );
                    searchPaths( map, concatCoordinates(coordinatesSoFar, toCoordinates(movement)) );
                }
                else{
                    //Error there is no free path to any weapon
                }
            }
        }
        else{
            //Error there is no free path to any treasure and there is no monster on the path
        }
    }
    else{
        //Error there is no path to any treasure
    }
}

bool HeroAI::isAIExecuting(){
    return executing;
}

Tile* HeroAI::chooseTarget(Map& map, std::string& errorText) {
    Tile* heroPosition = map.getHero()->getPosition();
    
    return heroPosition;
}

std::vector<Tile*> HeroAI::findShortestPathToAny(char goalTile, Map& map, bool interacting, Tile* start){

    std::vector< std::vector<Tile*> > paths;

    std::vector<Tile*> targets = map.findAll(goalTile);

    for (Tile* target : targets) {
        std::vector<Tile*> pathToTarget = aStarSearchPath(map, interacting, start, target);
        if(pathToTarget.size() > 0)
            paths.push_back( pathToTarget );
    }

    if(paths.size() == 0)
        return std::vector<Tile*>();
    std::vector<Tile*> shortestPath = paths[0];

    for(std::vector<Tile*> path : paths){
        if( path.size() < shortestPath.size() )
            shortestPath = path;
    }

    return shortestPath;
}

Tile* HeroAI::isOnPath(std::vector<Tile*> path, char toFind){
    std::stringstream characters;
    char c;

    for (Tile* tile : path) {
        tile->draw(characters);
    }

    size_t position = characters.str().find(toFind);
    if (position != std::string::npos) {
        return path[position];
    }

    return NULL;
}

void HeroAI::executePlan(std::vector<std::pair<int, int>> coordinates){
    std::vector<Tile*> path;

    if(coordinates.size() == 0){
        Tile* heroPosition = playMap.getHero()->getPosition();
        
        for(int i = 0; i < 2; i++){
            setConsolCursorToOnMap( heroPosition->getX(), heroPosition->getY() );
            setConsolColor( WHITE, BLUE );
            std::cout << "!";
            setConsolColor(WHITE);
            Sleep(250);

            setConsolCursorToOnMap(heroPosition->getX(), heroPosition->getY());
            setConsolColor(WHITE);
            std::cout << "h";
            Sleep(250);
        }

        executing = false;
        return;
    }

    for (int i = 0; i < coordinates.size(); i++) {
        path.push_back( playMap.getTile(coordinates[i].first, coordinates[i].second) );
    }

    for (int nextIndex = 1; nextIndex < path.size(); nextIndex++) {
        if (playMap.getHero()->getPosition() == path[nextIndex - 1]) {
            bool treasureBefore = playMap.getHero()->hasTreasure();

            Direction moveDirection = path[nextIndex - 1]->getDirection(path[nextIndex]);
            playMap.getHero()->move(moveDirection);

            Game::render();
            Sleep(200);

            if (treasureBefore != playMap.getHero()->hasTreasure()) {
                executing = false;
            }
            Game::handleKeyInputs();
            if (!executing){
                Game::render();
                return;
            }
        }
    }
}

std::vector<Tile*> HeroAI::moveHeroToTarget(Map& map, Tile* target){
    std::vector<Tile*> movementHistory;

    std::vector<Tile*> path = aStarSearchPath(map, true, map.getHero()->getPosition(), target);
    for (int nextIndex = 1; nextIndex < path.size(); nextIndex++) {

        movementHistory.push_back( path[nextIndex - 1] );

        if (map.getHero()->getPosition() == path[nextIndex - 1]) {
            Direction moveDirection = path[nextIndex - 1]->getDirection(path[nextIndex]);
            map.getHero()->move(moveDirection);
        }
    }

    return movementHistory;
}

void HeroAI::toggleAI(){

    executing = !executing;

    if (executing) {
        executePlan( plan() );
    }
}
