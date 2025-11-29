#include "Victory.h"

Victory::Victory(Vector2 origin) : Scene(origin) {}

Victory::~Victory() {}

void Victory::initialise() {
    mGameState.nextSceneID = -1; 
}

void Victory::update(float deltaTime) {
    if (IsKeyPressed(KEY_ENTER)) {
        mGameState.nextSceneID = 0; 
    }
}

void Victory::render() {
    ClearBackground(BLACK);
    
    const char* victoryText = "YOU WON!";
    int victoryWidth = MeasureText(victoryText, 80);
    DrawText(victoryText, 320 - victoryWidth/2, 120, 80, GREEN); 
    
    const char* flavorText = "wow. you actually dodged all that?";
    int flavorWidth = MeasureText(flavorText, 25);
    DrawText(flavorText, 320 - flavorWidth/2, 220, 25, WHITE);
    
    const char* flavorText2 = "heh. guess you're pretty determined.";
    int flavorWidth2 = MeasureText(flavorText2, 25);
    DrawText(flavorText2, 320 - flavorWidth2/2, 260, 25, WHITE);
    
    const char* instruction = "Press ENTER to return to menu";
    int instructionWidth = MeasureText(instruction, 20);
    DrawText(instruction, 320 - instructionWidth/2, 400, 20, LIGHTGRAY);
}

void Victory::shutdown() {}
