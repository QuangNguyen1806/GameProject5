#include "Menu.h"

Menu::Menu() : Scene{{0.0f, 0.0f}}, mTextFlashTimer(0.0f), mTextVisible(true) {}

Menu::Menu(Vector2 origin) : Scene{origin}, mTextFlashTimer(0.0f), mTextVisible(true) {}

Menu::~Menu() { 
    shutdown(); 
}

void Menu::initialise() {
    // Load Menu music
    mGameState.bgm = LoadMusicStream("assets/music/Menu.mp3"); 
    mGameState.nextSceneID = -1;  
    SetMusicVolume(mGameState.bgm, 0.5f);
    PlayMusicStream(mGameState.bgm);
    
    mTextFlashTimer = 0.0f;
    mTextVisible = true;
}

void Menu::update(float deltaTime) {
    UpdateMusicStream(mGameState.bgm);
    
    mTextFlashTimer += deltaTime;
    if (mTextFlashTimer >= 0.5f) {
        mTextFlashTimer = 0.0f;
        mTextVisible = !mTextVisible;
    }
    
    if (IsKeyPressed(KEY_ENTER)) {
        mGameState.nextSceneID = 1; 
    }
}

void Menu::render() {
    ClearBackground(BLACK);
    DrawText(GAME_TITLE, 40, 50, 50, WHITE);
    DrawText("HOW TO PLAY:", 200, 160, 30, WHITE);
    DrawText(INSTRUCTIONS, 100, 210, 20, WHITE);
    if (mTextVisible) {
        DrawText(START_TEXT, 180, 350, 30, WHITE);
    }
    DrawText(SANS_QUOTE, 140, 430, 20, GRAY);
}

void Menu::shutdown() {
    UnloadMusicStream(mGameState.bgm);
}
