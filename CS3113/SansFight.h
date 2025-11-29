#ifndef SANSFIGHT_H
#define SANSFIGHT_H

#include "Scene.h"
#include "Entity.h"
#include "ShaderProgram.h"
#include "Effects.h"

// Attack entity structures
struct Bone {
    Vector2 position;
    Vector2 size;
    Vector2 velocity;
    bool horizontal;
    bool active;
};

struct GasterBlaster {
    Vector2 position;
    float angle;
    float chargeTime;
    float fireTime;
    float animationTime;
    int currentFrame;
    bool firing;
    bool active;
};

struct Platform {
    Vector2 position;
    Vector2 size;
    bool active;
};

class SansFight : public Scene {
private:
    // Game state
    static constexpr int MAX_PLAYER_HP = 5;
    static constexpr float FIGHT_DURATION = 120.0f;
    static constexpr int MAX_BONES = 50;
    static constexpr int MAX_BLASTERS = 10;
    static constexpr int MAX_PLATFORMS = 5;
    
    float mAttackTimer;
    float mDamageTimer;
    bool mInvincible;
    float mInvincibilityTimer;

    int mPlayerHP = 92;
    float mElapsedTime = 0.0f;
    bool mBlueSoulMode = false;
    bool mGameOver = false;
    bool mPlayerWon = false;

    
    // Sans animation
    float mSansAnimationTimer;
    int mSansAnimationFrame;
    int mSansAnimationDirection;
    
    // Entities
    Entity* mSoul;
    
    // Attack arrays
    Bone mBones[MAX_BONES];
    GasterBlaster mBlasters[MAX_BLASTERS];
    Platform mPlatforms[MAX_PLATFORMS];

     // Shader system
    ShaderProgram mDamageShader;
    bool mShaderLoaded;
    float mDamageShaderIntensity;  
    float mDamageFlashTimer;        

    // Visual effects system
    Effects* mScreenEffects;

    
    // Textures
    struct UITextures {
        // Battle elements
        Texture2D combatZoneBorder;
        Texture2D platform1;
        Texture2D platform2;
        
        // Projectiles
        Texture2D boneH;
        Texture2D boneV;
        Texture2D blasterSheet;
        Texture2D blasterBeam;
        
        // Sans sprites
        Texture2D sansFrame1;
        Texture2D sansFrame2;
        Texture2D sansFrame3;
        Texture2D sansBody;
        
        // UI
        Texture2D uiFight;
        Texture2D uiAct;
        Texture2D uiItem;
        Texture2D uiMercy;
        Texture2D hpBar;
        Texture2D hpBackground;
        
        // Soul
        Texture2D soulRed;
        Texture2D soulBlue;
    } mUITextures;
    
    // Audio
    Music mMegalovania;
    Sound mBlasterSound;
    Sound mDamageSound;
    
    // Attack management
    void updateAttacks(float deltaTime);
    void spawnBoneWave(float startY, int count, float speed, bool fromLeft);
    void spawnBonePlatforms(int platformCount);
    void spawnGasterBlaster(Vector2 pos, float angle, float chargeTime);
    void updateBones(float deltaTime);
    void updateBlasters(float deltaTime);
    void updatePlatforms(float deltaTime);
    void updateSoulMovement(float deltaTime);
    void checkCollisions();
    void takeDamage(int amount);
    
    // Attack patterns 
    void executeAttackPattern(float time);
    
    // Helper methods
    void renderSans();
    void renderHealthBar();
    void renderButtons();
    void renderAttacks();
    void keepSoulInBattleBox();

    // Shaders & Effects
    void updateShaderState(float deltaTime);
    void applyDamageShaderToSoul();
    void initializeShaderSystem();
    void shutdownShaderSystem();
    
public:
    SansFight(Vector2 origin);
    ~SansFight();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // SANSFIGHT_H
