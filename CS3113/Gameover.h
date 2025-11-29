#pragma once
#include "Scene.h"

class Gameover : public Scene {
private:
    // Assets
    Texture2D gameoverTexture;     
    Music gameoverMusic;          
    
    // Animation state
    enum GameoverState {
        HEART_BREAKING,   
        FADING_OUT,     
        SHOWING_TEXT,  
        MENU_ACTIVE    
    };
    
    GameoverState currentState;
    float stateTimer;
    
    // Menu selection
    int selectedOption; 
    float fadeAlpha;   
    
    // Text positions
    Vector2 gameoverTextPos;
    Vector2 hopeTextPos;
    Vector2 continueTextPos;
    Vector2 giveupTextPos;
    
    void updateHeartBreaking(float deltaTime);
    void updateFading(float deltaTime);
    void updateMenu(float deltaTime);
    void renderMenu();
    
public:
    Gameover(Vector2 origin = {0.0f, 0.0f}) : Scene(origin) {}
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};
