#include "Scene.h"

#ifndef Menu_H
#define Menu_H

class Menu : public Scene {
public:
    static constexpr float TILE_DIMENSION = 75.0f,
                           ACCELERATION_OF_GRAVITY = 981.0f,
                           END_GAME_THRESHOLD = 800.0f;
    
    static constexpr int OFFSET = 300.0f;
    
    const char * const GAME_TITLE = "BAD TIME SIMULATOR";
    const char * const START_TEXT = "Press ENTER to start";
    const char * const INSTRUCTIONS = "ARROW KEYS - Move your SOUL\nDODGE - Bones and lasers\nSURVIVE - Don't let HP reach 0";
    const char * const SANS_QUOTE = "you're gonna have a bad time";
    
    Menu();
    Menu(Vector2 origin);
    ~Menu();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;

private:
    float mTextFlashTimer;  
    bool mTextVisible;
};

#endif
