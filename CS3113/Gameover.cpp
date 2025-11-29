#include "Gameover.h"


void Gameover::initialise() {

    mGameState.nextSceneID = -1;
    // Load assets
    gameoverTexture = LoadTexture("assets/ui/gameover-sheet0.png");
    gameoverMusic = LoadMusicStream("assets/music/GameOver.mp3");
    
    // Start with heart breaking
    currentState = HEART_BREAKING;
    stateTimer = 0.0f;
    
    // Menu setup
    selectedOption = 0;
    fadeAlpha = 0.0f;
    
    gameoverTextPos = {120.0f, 80.0f};     
    hopeTextPos = {200.0f, 270.0f};       
    continueTextPos = {260.0f, 350.0f};  
    giveupTextPos = {260.0f, 390.0f};     
    
    // Start music
    PlayMusicStream(gameoverMusic);
}


void Gameover::update(float deltaTime) {
    // Update music
    UpdateMusicStream(gameoverMusic);
    
    // State machine
    switch (currentState) {
        case HEART_BREAKING:
            updateHeartBreaking(deltaTime);
            break;
            
        case FADING_OUT:
            updateFading(deltaTime);
            break;
            
        case SHOWING_TEXT:
            stateTimer += deltaTime;
            if (stateTimer >= 1.0f) {
                currentState = MENU_ACTIVE;
                stateTimer = 0.0f;
            }
            break;
            
        case MENU_ACTIVE:
            updateMenu(deltaTime);
            break;
    }
}

void Gameover::updateHeartBreaking(float deltaTime) {
    stateTimer += deltaTime;
    
    if (stateTimer < 1.0f) {
        for (int i = 0; i < 5; i++) {
            shards[i].velocity.y += 200.0f * deltaTime;
            
            shards[i].position.x += shards[i].velocity.x * deltaTime;
            shards[i].position.y += shards[i].velocity.y * deltaTime;
            
            shards[i].rotation += shards[i].rotationSpeed * deltaTime;
        }
    } else {
        currentState = FADING_OUT;
        stateTimer = 0.0f;
    }
}

void Gameover::updateFading(float deltaTime) {
    stateTimer += deltaTime;
    
    // Fade to black over 1 second
    fadeAlpha = stateTimer / 1.0f;
    
    if (fadeAlpha >= 1.0f) {
        fadeAlpha = 1.0f;
        currentState = SHOWING_TEXT;
        stateTimer = 0.0f;
    }
}

void Gameover::updateMenu(float deltaTime) {
    // Arrow key selection
    if (IsKeyPressed(KEY_DOWN)) {
        selectedOption = (selectedOption + 1) % 2;
    }
    if (IsKeyPressed(KEY_UP)) {
        selectedOption = (selectedOption - 1 + 2) % 2;
    }
    
    // Confirm selection
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_Z)) {
        if (selectedOption == 0) {
            // Continue - reload boss fight
            mGameState.nextSceneID = 1;

        } else {
            // Give Up - return to menu
            mGameState.nextSceneID = 0;

        }
    }
}

void Gameover::render() {
    ClearBackground(BLACK);
    
    switch (currentState) {
        case HEART_BREAKING:
            break;
            
        case FADING_OUT:
            // Fade overlay
            DrawRectangle(0, 0, 640, 480, ColorAlpha(BLACK, fadeAlpha));
            break;
            
        case SHOWING_TEXT:
        case MENU_ACTIVE:
            // Black background
            ClearBackground(BLACK);
            
            // Draw "GAME OVER" graphic
            DrawTexture(gameoverTexture, 
                       (int)gameoverTextPos.x, 
                       (int)gameoverTextPos.y, 
                       WHITE);
            
            // Draw "Don't lose hope!" text
            DrawText("Don't lose hope!", 
                    (int)hopeTextPos.x, 
                    (int)hopeTextPos.y, 
                    20, 
                    WHITE);
            
            // Draw menu (only if active)
            if (currentState == MENU_ACTIVE) {
                renderMenu();
            }
            break;
    }
}

void Gameover::renderMenu() {
    // Draw "Continue" option
    Color continueColor = (selectedOption == 0) ? YELLOW : WHITE;
    DrawText("Continue", 
            (int)continueTextPos.x, 
            (int)continueTextPos.y, 
            24, 
            continueColor);
    
    // Draw "Give Up" option
    Color giveupColor = (selectedOption == 1) ? YELLOW : WHITE;
    DrawText("Give Up", 
            (int)giveupTextPos.x, 
            (int)giveupTextPos.y, 
            24, 
            giveupColor);
    
    // Draw selection heart (soul)
    Vector2 soulPos;
    if (selectedOption == 0) {
        soulPos = {continueTextPos.x - 30, continueTextPos.y + 8};
    } else {
        soulPos = {giveupTextPos.x - 30, giveupTextPos.y + 8};
    }
    
    // Draw red heart
    DrawRectangle((int)soulPos.x, (int)soulPos.y, 8, 8, RED);
}

void Gameover::shutdown() {  
    UnloadTexture(gameoverTexture);
    UnloadMusicStream(gameoverMusic);
}
