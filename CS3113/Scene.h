#ifndef SCENE_H
#define SCENE_H

#include "cs3113.h"
#include "Entity.h"

struct GameState {
    Entity *soul;
    Music bgm;
    int nextSceneID;
};

class Scene {
protected:
    GameState mGameState;
    Vector2 mOrigin;

public:
    Scene(Vector2 origin) : mOrigin(origin) {}
    virtual ~Scene() {}

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;

    GameState getState() const { return mGameState; }
    void setState(GameState state) { mGameState = state; }
};

#endif
