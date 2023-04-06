#include "GrabTheTreasure.h"

int main()
{
    const UINT originalCodePage = GetConsoleCP();;

    resizeConsolWindow(CONSOL_WIDTH, CONSOL_HEIGHT);
    showConsoleCursor(false);


    Game::setState(GAMESTART);
    Game::render();

    waitAnyConsolKeyPress();

    Game::setState(GAMEPLAYING);
    Game::render();

    HeroAI ai = HeroAI( *Game::getMap() );
    ai.aStarSearchPath(Game::getMap()->getHero()->getPosition(), Game::getMap()->getTile(1, 7));

    while (Game::getState() == GAMEPLAYING) {

        if (Game::handleKeyInputs()) {
            Game::render();
        }

        Game::checkGameEnd();
    }

    waitAnyConsolKeyPress(2000);

    SetConsoleOutputCP(originalCodePage);
};


