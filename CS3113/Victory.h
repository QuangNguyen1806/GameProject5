#ifndef VICTORY_H
#define VICTORY_H

#include "Scene.h"

class Victory : public Scene {
public:
    Victory(Vector2 origin = {0.0f, 0.0f});
    ~Victory();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif
