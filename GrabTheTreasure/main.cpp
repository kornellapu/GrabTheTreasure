#include "GrabTheTreasure.h"

int main()
{
    const UINT originalCodePage = GetConsoleCP();;

    resizeConsolWindow(CONSOL_WIDTH, CONSOL_HEIGHT);
    showConsoleCursor(false);

    Game::setState(GAMESTART);
    Game::render();

    Game::waitOnStartScreen();

    Game::setState(GAMEPLAYING);
    Game::render();

    while (Game::getState() == GAMEPLAYING) {

        if (Game::handleKeyInputs()) {
            Game::render();
        }

        Game::checkGameEnd();
    }

    showText(TEXT_POS_X, TEXT_POS_Y, pressKeyExitText);

    waitAnyConsolKeyPress(100);
    showConsoleCursor(true);

    SetConsoleOutputCP(originalCodePage);
    system("CLS");
};


