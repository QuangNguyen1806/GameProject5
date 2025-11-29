#include "raylib.h"
#include "CS3113/Scene.h"
#include "CS3113/Menu.h"
#include "CS3113/SansFight.h"
#include "CS3113/Victory.h"
#include "CS3113/Gameover.h"


// ========== GLOBAL CONSTANTS ==========
constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;
constexpr int FPS = 60;
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;  
constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };


// ========== GLOBAL VARIABLES ==========
AppStatus gAppStatus = RUNNING;

float gPreviousTicks = 0.0f;
float gTimeAccumulator = 0.0f;

Scene *gCurrentScene = nullptr;
std::vector<Scene*> gLevels = {};

Menu *gMenu = nullptr;
SansFight *gSansFight = nullptr;
Victory *gVictory = nullptr;
Gameover *gGameover = nullptr;


// ========== FUNCTION DECLARATIONS ==========
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();


// ========== FUNCTION IMPLEMENTATIONS ==========
void switchToScene(Scene *scene) {
    if (gCurrentScene) {
        gCurrentScene->shutdown();
    }
    gCurrentScene = scene;
    if (gCurrentScene) {
        gCurrentScene->initialise();
    }
}


void initialise() {
    // Raylib setup
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sans Fight - 120s Survival");
    SetTargetFPS(FPS);
    InitAudioDevice();

    // Create scenes
    gMenu = new Menu(ORIGIN);
    gSansFight = new SansFight(ORIGIN);
    gVictory = new Victory(ORIGIN);
    gGameover = new Gameover(ORIGIN);

    // Populate levels vector (keeps indices similar to original)
    gLevels.push_back(gMenu);       // Index 0
    gLevels.push_back(gSansFight);  // Index 1
    gLevels.push_back(gVictory);    // Index 2
    gLevels.push_back(gGameover);   // Index 3

    // Start at menu (initialise it)
    switchToScene(gLevels[0]);
}


void processInput() {
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) {
        gAppStatus = TERMINATED;
    }
}


void update() {
    float ticks = GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
    
    gTimeAccumulator += deltaTime;
    
    while (gTimeAccumulator >= FIXED_TIMESTEP) {
        gCurrentScene->update(FIXED_TIMESTEP);
        
        if (gCurrentScene->getState().nextSceneID >= 0) {
            switchToScene(gLevels[gCurrentScene->getState().nextSceneID]);
        }
        
        gTimeAccumulator -= FIXED_TIMESTEP;
    }
}



void render() {
    BeginDrawing();
    if (gCurrentScene) {
        gCurrentScene->render();
    }
    EndDrawing();
}


void shutdown() {
    for (Scene *scene : gLevels) {
        delete scene;
    }
    gLevels.clear();

    CloseAudioDevice();
    CloseWindow();
}


int main() {
    initialise();

    while (gAppStatus == RUNNING) {
        processInput();
        update();
        render();
    }

    shutdown();
    return 0;
}
