#include "SansFight.h"

SansFight::SansFight(Vector2 origin)
    : Scene(origin),
      mAttackTimer(0.0f),
      mDamageTimer(0.0f),
      mInvincible(false),
      mInvincibilityTimer(0.0f),
      mSansAnimationTimer(0.0f),
      mSansAnimationFrame(0),
      mSansAnimationDirection(1) {}

SansFight::~SansFight() {
    shutdown();
}

void SansFight::initialise() {
    mGameState.nextSceneID = -1;
    mPlayerHP = MAX_PLAYER_HP;
    mElapsedTime = 0.0f;
    mBlueSoulMode = false;
    mGameOver = false;
    mPlayerWon = false;
    
    mAttackTimer = 0.0f;
    mDamageTimer = 0.0f;
    mInvincible = false;
    mInvincibilityTimer = 0.0f;
        
    // Load texture
    mUITextures.boneH = LoadTexture("assets/projectiles/boneh.png");
    mUITextures.boneV = LoadTexture("assets/projectiles/bonev.png");
    mUITextures.blasterSheet = LoadTexture("assets/projectiles/gasterblaster-sheet0.png");
    mUITextures.blasterBeam = LoadTexture("assets/effects/gasterblasthit.png");
    mUITextures.platform1 = LoadTexture("assets/battlebox/platform1.png");
    mUITextures.platform2 = LoadTexture("assets/battlebox/platform2.png");
    mUITextures.combatZoneBorder = LoadTexture("assets/battlebox/combatzoneborder.png");
    mUITextures.sansFrame1 = LoadTexture("assets/sans/Sans-Frame1.png");
    mUITextures.sansFrame2 = LoadTexture("assets/sans/Sans-Frame2.png");
    mUITextures.sansFrame3 = LoadTexture("assets/sans/Sans-Frame3.png");
    mUITextures.sansBody = LoadTexture("assets/sans/sansbody-sheet0.png");
    mUITextures.uiFight = LoadTexture("assets/ui/uifight-sheet0.png");
    mUITextures.uiAct = LoadTexture("assets/ui/uiact-sheet0.png");
    mUITextures.uiItem = LoadTexture("assets/ui/uiitem-sheet0.png");
    mUITextures.uiMercy = LoadTexture("assets/ui/uimercy-sheet0.png");
    mUITextures.hpBar = LoadTexture("assets/ui/hpbar.png");
    mUITextures.hpBackground = LoadTexture("assets/ui/hpbackground.png");
    mUITextures.soulRed = LoadTexture("assets/soul/red.png");
    mUITextures.soulBlue = LoadTexture("assets/soul/blue.png");
    
    // Load audio
    mMegalovania = LoadMusicStream("assets/music/Megalovania.mp3");
    mBlasterSound = LoadSound("assets/music/Blaster.mp3");
    mDamageSound = LoadSound("assets/music/Damaged.mp3");
    
    SetMusicVolume(mMegalovania, 0.6f);
    PlayMusicStream(mMegalovania);
    
    // Initialize soul
    mSoul = new Entity();
    mSoul->setPosition({320.0f, 300.0f});
    mSoul->setSize({10.0f, 10.0f});
    mSoul->setSpeed(150.0f);
    mSoul->setColliderDimensions({10.0f, 10.0f});
    mSoul->activate();
    
    // Initialize attack arrays
    for (int i = 0; i < MAX_BONES; i++) {
        mBones[i].active = false;
    }
    for (int i = 0; i < MAX_BLASTERS; i++) {
        mBlasters[i].active = false;
    }
    for (int i = 0; i < MAX_PLATFORMS; i++) {
        mPlatforms[i].active = false;
    }

    initializeShaderSystem();
    }

void SansFight::initializeShaderSystem() {
    mShaderLoaded = false;
    mDamageShaderIntensity = 0.0f;
    mDamageFlashTimer = 0.0f;
    
    // Load the damage shader
    mShaderLoaded = mDamageShader.load("shaders/vertex.glsl", "shaders/fragment.glsl");
    
    // Initialize effects system for screen transitions
    mScreenEffects = new Effects({320.0f, 240.0f}, 640.0f, 480.0f);
}

void SansFight::update(float deltaTime) {
    UpdateMusicStream(mMegalovania);
    
    mElapsedTime += deltaTime;
    
    // Check win condition (survived 120 seconds)
    if (mElapsedTime >= FIGHT_DURATION) {
        mPlayerWon = true;
        mGameState.nextSceneID = 2;
        return;
    }
    
    // Check lose condition
    if (mPlayerHP <= 0) {
        mGameOver = true;
        mGameState.nextSceneID = 3;
        return;
    }
    
    // Update invincibility
    if (mInvincible) {
        mInvincibilityTimer += deltaTime;
        if (mInvincibilityTimer >= 0.5f) {
            mInvincible = false;
            mInvincibilityTimer = 0.0f;
        }
    }

    // Update shader state
    updateShaderState(deltaTime);

    // Update effects (for fade transitions)
    mScreenEffects->update(deltaTime, nullptr);
    
    updateSoulMovement(deltaTime);
    
    // Update soul physics
    if (mSoul && mSoul->isActive()) {
        mSoul->update(deltaTime, nullptr, nullptr, 0, nullptr);
        keepSoulInBattleBox();
    }
    
    // Sans animation
    mSansAnimationTimer += deltaTime;
    if (mSansAnimationTimer >= 0.4f) {
        mSansAnimationTimer = 0.0f;
        if (mSansAnimationDirection == 1) {
            mSansAnimationFrame++;
            if (mSansAnimationFrame >= 2) {
                mSansAnimationFrame = 2;
                mSansAnimationDirection = -1;
            }
        } else {
            mSansAnimationFrame--;
            if (mSansAnimationFrame <= 0) {
                mSansAnimationFrame = 0;
                mSansAnimationDirection = 1;
            }
        }
    }
    
    executeAttackPattern(mElapsedTime);
    updateAttacks(deltaTime);
    checkCollisions();
}

void SansFight::updateSoulMovement(float deltaTime) {
    if (!mSoul) return;
    
    Vector2 velocity = {0.0f, 0.0f};
    float speed = mSoul->getSpeed();
    
    if (mBlueSoulMode) {
        Vector2 currentVel = mSoul->getVelocity();
        
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            currentVel.x = -speed;
        } else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            currentVel.x = speed;
        } else {
            currentVel.x = 0.0f;
        }
        
        currentVel.y += 500.0f * deltaTime;
        
        int boxSize = 140;
        int boxX = (640 - boxSize) / 2;
        int boxY = 230;
        float groundY = boxY + boxSize - 8.0f; 
        
        bool onPlatform = false;
        Vector2 soulPos = mSoul->getPosition();
        
        for (int i = 0; i < MAX_PLATFORMS; i++) {
            if (mPlatforms[i].active) {
                Rectangle platRect = {
                    mPlatforms[i].position.x,
                    mPlatforms[i].position.y,
                    mPlatforms[i].size.x,
                    mPlatforms[i].size.y
                };
                
                Rectangle soulRect = {
                    soulPos.x - 5.0f,
                    soulPos.y + 3.0f, 
                    10.0f,
                    5.0f
                };
                
                if (CheckCollisionRecs(soulRect, platRect) && currentVel.y >= 0) {
                    onPlatform = true;
                    currentVel.y = 0.0f;
                    
                    soulPos.y = mPlatforms[i].position.y - 5.0f;
                    mSoul->setPosition(soulPos);
                    break;
                }
            }
        }
        
        if (!onPlatform && soulPos.y >= groundY && currentVel.y >= 0) {
            onPlatform = true;
            currentVel.y = 0.0f;
            soulPos.y = groundY;
            mSoul->setPosition(soulPos);
        }
        
        // Jumping
        if (onPlatform && IsKeyPressed(KEY_SPACE)) {
            currentVel.y = -250.0f;
        }
        
        // Set velocity for Entity physics
        mSoul->setVelocity(currentVel);
        
    } else {
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            velocity.x = -speed;
        }
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
            velocity.x = speed;
        }
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
            velocity.y = -speed;
        }
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
            velocity.y = speed;
        }
        
        mSoul->setVelocity(velocity);
    }
}



void SansFight::executeAttackPattern(float time) {
    
    if (time >= 1.0f && time < 1.2f) {
        spawnBoneWave(250.0f, 6, 80.0f, true);
    }
    if (time >= 4.0f && time < 4.2f) {
        spawnBoneWave(270.0f, 6, 80.0f, false);
    }
    if (time >= 7.0f && time < 7.2f) {
        spawnBoneWave(240.0f, 8, 90.0f, true);
    }
    if (time >= 10.0f && time < 10.2f) {
        spawnBoneWave(300.0f, 8, 90.0f, false);
    }
    
    if (time >= 12.5f && time < 12.7f) {
        spawnGasterBlaster({250.0f, 240.0f}, 0.0f, 1.5f);   
    }
    if (time >= 15.0f && time < 15.2f) {
        spawnGasterBlaster({390.0f, 240.0f}, 180.0f, 1.5f); 
    }
    if (time >= 17.5f && time < 17.7f) {
        spawnGasterBlaster({320.0f, 200.0f}, 90.0f, 1.5f);   
    }
    
    if (time >= 20.0f && time < 20.2f) {
        spawnBoneWave(220.0f, 10, 110.0f, true);
    }
    if (time >= 22.5f && time < 22.7f) {
        spawnBoneWave(300.0f, 10, 110.0f, false);
    }
    
    if (time >= 25.0f && time < 25.2f) {
        spawnGasterBlaster({250.0f, 220.0f}, 30.0f, 1.2f);
        spawnGasterBlaster({390.0f, 220.0f}, 150.0f, 1.2f);
    }
    
    if (time >= 27.5f && time < 27.7f) {
        spawnBoneWave(260.0f, 12, 120.0f, true);
    }
    
    
    if (time >= 30.0f && time < 30.2f) {
        mBlueSoulMode = true;
        spawnBonePlatforms(3);  
    }
    
    if (time >= 32.0f && time < 32.2f) {
        spawnBoneWave(350.0f, 5, 85.0f, true);
    }
    
    if (time >= 35.0f && time < 35.2f) {
        spawnGasterBlaster({320.0f, 180.0f}, 90.0f, 1.0f);
    }
    
    if (time >= 37.5f && time < 37.7f) {
        spawnBoneWave(280.0f, 7, 100.0f, false);
    }
    
    if (time >= 40.0f && time < 40.1f) {
        mBlueSoulMode = false;
        for (int i = 0; i < MAX_PLATFORMS; i++) {
            mPlatforms[i].active = false;
        }
    }
    
    if (time >= 41.0f && time < 41.2f) {
        spawnGasterBlaster({240.0f, 210.0f}, 0.0f, 0.9f);
        spawnGasterBlaster({400.0f, 210.0f}, 180.0f, 0.9f);
    }
    
    if (time >= 43.5f && time < 43.7f) {
        spawnBoneWave(250.0f, 14, 130.0f, true);
    }
    
    if (time >= 46.0f && time < 46.2f) {
        spawnGasterBlaster({270.0f, 200.0f}, 45.0f, 0.8f);   
        spawnGasterBlaster({370.0f, 200.0f}, 135.0f, 0.8f);  
        spawnGasterBlaster({320.0f, 250.0f}, 270.0f, 0.8f);  
    }
    
    if (time >= 48.5f && time < 48.7f) {
        spawnBoneWave(290.0f, 12, 125.0f, false);
    }
    
    
    if (time >= 60.0f && time < 60.2f) {
        mBlueSoulMode = true;
        spawnBonePlatforms(4);  
    }
    
    if (time >= 62.5f && time < 62.7f) {
        spawnBoneWave(330.0f, 8, 105.0f, true);
    }
    
    if (time >= 65.0f && time < 65.2f) {
        spawnGasterBlaster({250.0f, 210.0f}, 0.0f, 0.7f);
    }
    
    if (time >= 67.5f && time < 67.7f) {
        spawnBoneWave(260.0f, 10, 115.0f, false);
    }
    
    if (time >= 70.0f && time < 70.1f) {
        mBlueSoulMode = false;
        for (int i = 0; i < MAX_PLATFORMS; i++) {
            mPlatforms[i].active = false;
        }
    }
    
    if (time >= 71.0f && time < 71.2f) {
        spawnBoneWave(240.0f, 15, 140.0f, true);
    }
    if (time >= 72.5f && time < 72.7f) {
        spawnBoneWave(300.0f, 15, 140.0f, false);
    }
    
    if (time >= 74.0f && time < 74.2f) {
        spawnGasterBlaster({280.0f, 200.0f}, 20.0f, 0.6f);
        spawnGasterBlaster({360.0f, 200.0f}, 160.0f, 0.6f);
    }
    
    if (time >= 76.0f && time < 76.2f) {
        spawnBoneWave(270.0f, 18, 150.0f, true);
    }
    
    if (time >= 78.5f && time < 78.7f) {
        spawnGasterBlaster({250.0f, 195.0f}, 10.0f, 0.5f);  
        spawnGasterBlaster({390.0f, 195.0f}, 170.0f, 0.5f); 
        spawnGasterBlaster({320.0f, 270.0f}, 270.0f, 0.5f); 
        spawnGasterBlaster({320.0f, 180.0f}, 90.0f, 0.5f);   
    }
    
    if (time >= 81.0f && time < 81.2f) {
        spawnBoneWave(280.0f, 16, 145.0f, false);
    }
    
    if (time >= 83.0f && time < 83.2f) {
        mBlueSoulMode = true;
        spawnBonePlatforms(3);
    }
    
    if (time >= 85.0f && time < 85.2f) {
        spawnBoneWave(320.0f, 12, 120.0f, true);
    }
    
    if (time >= 87.0f && time < 87.2f) {
        spawnGasterBlaster({320.0f, 190.0f}, 90.0f, 0.6f);
    }

    
    if (time >= 90.0f && time < 90.1f) {
        mBlueSoulMode = false;
        for (int i = 0; i < MAX_PLATFORMS; i++) {
            mPlatforms[i].active = false;
        }
    }
    
    if (time >= 91.0f && time < 91.2f) {
        spawnBoneWave(240.0f, 16, 150.0f, true);
    }
    if (time >= 92.5f && time < 92.7f) {
        spawnBoneWave(300.0f, 16, 150.0f, false);
    }
    if (time >= 94.0f && time < 94.2f) {
        spawnBoneWave(260.0f, 18, 155.0f, true);
    }
    
    // Blaster assault
    if (time >= 95.5f && time < 95.7f) {
        spawnGasterBlaster({250.0f, 200.0f}, 0.0f, 0.4f);
        spawnGasterBlaster({390.0f, 200.0f}, 180.0f, 0.4f);
    }
    
    if (time >= 97.0f && time < 97.2f) {
        spawnBoneWave(290.0f, 20, 160.0f, false);
    }
    
    // Final blaster combination
    if (time >= 98.5f && time < 98.7f) {
        spawnGasterBlaster({270.0f, 195.0f}, 30.0f, 0.4f);
        spawnGasterBlaster({370.0f, 195.0f}, 150.0f, 0.4f);
        spawnGasterBlaster({320.0f, 250.0f}, 270.0f, 0.4f);
    }
    
    if (time >= 100.0f && time < 100.2f) {
        spawnBoneWave(250.0f, 22, 165.0f, true);
    }
    
    // Wave spam continues
    if (time >= 101.5f && time < 101.7f) {
        spawnBoneWave(300.0f, 20, 160.0f, false);
    }
    
    if (time >= 103.0f && time < 103.2f) {
        spawnBoneWave(270.0f, 18, 155.0f, true);
    }
    
    // Extreme blaster pattern
    if (time >= 104.5f && time < 104.7f) {
        spawnGasterBlaster({240.0f, 190.0f}, 0.0f, 0.3f);
        spawnGasterBlaster({320.0f, 180.0f}, 90.0f, 0.3f);
        spawnGasterBlaster({400.0f, 190.0f}, 180.0f, 0.3f);
        spawnGasterBlaster({320.0f, 260.0f}, 270.0f, 0.3f);
    }
    
    if (time >= 106.0f && time < 106.2f) {
        spawnBoneWave(280.0f, 24, 170.0f, false);
    }
    
    // Final wave push
    if (time >= 107.5f && time < 107.7f) {
        spawnBoneWave(250.0f, 20, 165.0f, true);
    }
    
    if (time >= 109.0f && time < 109.2f) {
        spawnBoneWave(300.0f, 22, 170.0f, false);
    }
    
    // Last blaster barrage before time runs out
    if (time >= 110.5f && time < 110.7f) {
        spawnGasterBlaster({280.0f, 195.0f}, 20.0f, 0.3f);
        spawnGasterBlaster({360.0f, 195.0f}, 160.0f, 0.3f);
    }
    
    if (time >= 112.0f && time < 112.2f) {
        spawnBoneWave(270.0f, 26, 175.0f, true);
    }
    
    if (time >= 113.5f && time < 113.7f) {
        spawnBoneWave(290.0f, 24, 170.0f, false);
    }
    
    // Final assault - everything at once
    if (time >= 115.0f && time < 115.2f) {
        spawnGasterBlaster({250.0f, 185.0f}, 0.0f, 0.2f);
        spawnGasterBlaster({390.0f, 185.0f}, 180.0f, 0.2f);
        spawnBoneWave(260.0f, 28, 180.0f, true);
    }
    
    if (time >= 117.0f && time < 117.2f) {
        spawnBoneWave(300.0f, 26, 175.0f, false);
    }
    
    // Last second blaster rush
    if (time >= 118.5f && time < 118.7f) {
        spawnGasterBlaster({320.0f, 180.0f}, 90.0f, 0.2f);
        spawnGasterBlaster({320.0f, 260.0f}, 270.0f, 0.2f);
    }
}


void SansFight::spawnBoneWave(float startY, int count, float speed, bool fromLeft) {
    int spawned = 0;
    
    // Spacing between bones vertically
    float verticalSpacing = 20.0f;  // Gap between each bone
    
    for (int i = 0; i < MAX_BONES && spawned < count; i++) {
        if (!mBones[i].active) {
            mBones[i].active = true;
            mBones[i].horizontal = true;
            mBones[i].size = {40.0f, 15.0f}; 
            
            // Calculate Y position with spacing
            float offsetY = (spawned - count / 2.0f) * verticalSpacing;
            mBones[i].position.y = startY + offsetY;
            
            if (fromLeft) {
                // Spawn from left side, move right
                mBones[i].position.x = -30.0f;
                mBones[i].velocity = {speed, 0.0f};
            } else {
                // Spawn from right side, move left
                mBones[i].position.x = 670.0f;
                mBones[i].velocity = {-speed, 0.0f};
            }
            
            spawned++;
        }
    }
}


void SansFight::spawnBonePlatforms(int platformCount) {
    int boxSize = 140;
    int boxX = (640 - boxSize) / 2;
    int boxY = 230;
    
    float platformSpacing = (float)boxSize / (platformCount + 1);
    
    for (int i = 0; i < platformCount && i < MAX_PLATFORMS; i++) {
        mPlatforms[i].active = true;
        mPlatforms[i].size = {50.0f, 10.0f};
        mPlatforms[i].position.x = boxX + platformSpacing * (i + 1) - 25.0f;
        mPlatforms[i].position.y = boxY + 30.0f + (i % 2) * 40.0f;
    }
}

void SansFight::spawnGasterBlaster(Vector2 pos, float angle, float chargeTime) {
    for (int i = 0; i < MAX_BLASTERS; i++) {
        if (!mBlasters[i].active) {
            mBlasters[i].active = true;
            mBlasters[i].position = pos;
            mBlasters[i].angle = angle;
            mBlasters[i].chargeTime = chargeTime;
            mBlasters[i].fireTime = 0.0f;
            mBlasters[i].animationTime = 0.0f;
            mBlasters[i].currentFrame = 0;
            mBlasters[i].firing = false;
            PlaySound(mBlasterSound);
            break;
        }
    }
}

void SansFight::updateAttacks(float deltaTime) {
    updateBones(deltaTime);
    updateBlasters(deltaTime);
    updatePlatforms(deltaTime);
}

void SansFight::updateBones(float deltaTime) {
    for (int i = 0; i < MAX_BONES; i++) {
        if (mBones[i].active) {
            mBones[i].position.x += mBones[i].velocity.x * deltaTime;
            mBones[i].position.y += mBones[i].velocity.y * deltaTime;
            
            // Deactivate if off screen
            if (mBones[i].position.x < -50.0f || mBones[i].position.x > 690.0f ||
                mBones[i].position.y < -50.0f || mBones[i].position.y > 530.0f) {
                mBones[i].active = false;
            }
        }
    }
}

void SansFight::updateBlasters(float deltaTime) {
    for (int i = 0; i < MAX_BLASTERS; i++) {
        if (mBlasters[i].active) {
            mBlasters[i].animationTime += deltaTime;
            
            if (mBlasters[i].animationTime >= 0.1f) {
                mBlasters[i].currentFrame++;
                if (mBlasters[i].currentFrame >= 10) {
                    mBlasters[i].currentFrame = 0;
                }
                mBlasters[i].animationTime = 0.0f;
            }
            
            if (!mBlasters[i].firing) {
                mBlasters[i].chargeTime -= deltaTime;
                if (mBlasters[i].chargeTime <= 0.0f) {
                    mBlasters[i].firing = true;
                    mBlasters[i].fireTime = 2.0f;
                }
            } else {
                mBlasters[i].fireTime -= deltaTime;
                if (mBlasters[i].fireTime <= 0.0f) {
                    mBlasters[i].active = false;
                }
            }
        }
    }
}

void SansFight::updatePlatforms(float deltaTime) {}

void SansFight::checkCollisions() {
    if (!mSoul || mInvincible) return;
    
    Vector2 soulPos = mSoul->getPosition();
    Rectangle soulRect = {soulPos.x - 5.0f, soulPos.y - 5.0f, 10.0f, 10.0f};
    
    // Check bone collisions
    for (int i = 0; i < MAX_BONES; i++) {
        if (mBones[i].active) {
            Rectangle boneRect = {
                mBones[i].position.x - mBones[i].size.x / 2,
                mBones[i].position.y - mBones[i].size.y / 2,
                mBones[i].size.x,
                mBones[i].size.y
            };
            
            if (CheckCollisionRecs(soulRect, boneRect)) {
                takeDamage(5);
                return;
            }
        }
    }
    
    // Check blaster collisions
    for (int i = 0; i < MAX_BLASTERS; i++) {
        if (mBlasters[i].active && mBlasters[i].firing) {
            float beamLength = 200.0f;
            float beamWidth = 30.0f;
            
            float angleRad = mBlasters[i].angle * DEG2RAD;
            Vector2 beamEnd = {
                mBlasters[i].position.x + cos(angleRad) * beamLength,
                mBlasters[i].position.y + sin(angleRad) * beamLength
            };
            
            Rectangle beamRect = {
                fmin(mBlasters[i].position.x, beamEnd.x),
                fmin(mBlasters[i].position.y, beamEnd.y),
                fabs(beamEnd.x - mBlasters[i].position.x) + beamWidth,
                fabs(beamEnd.y - mBlasters[i].position.y) + beamWidth
            };
            
            if (CheckCollisionRecs(soulRect, beamRect)) {
                takeDamage(1);
                return;
            }
        }
    }
}

void SansFight::takeDamage(int amount) {
    if (mShaderLoaded) {
        mDamageShaderIntensity = 1.0f;  
        mDamageFlashTimer = 0.0f;
    }

    if (mScreenEffects) {
        mScreenEffects->start(FADEOUT); 
        mScreenEffects->setEffectSpeed(3.0f);
    }
    mPlayerHP -= amount;
    if (mPlayerHP < 0) mPlayerHP = 0;
    mInvincible = true;
    mInvincibilityTimer = 0.0f;
    PlaySound(mDamageSound);
}

void SansFight::render() {
    ClearBackground(BLACK);
    
    int screenWidth = 640;
    int screenHeight = 480;
    
    // Timer display
    float timeRemaining = FIGHT_DURATION - mElapsedTime;
    DrawText(TextFormat("Time: %.1f / 120.0", mElapsedTime), 10, 10, 16, WHITE);
    DrawText(TextFormat("Remaining: %.1f", timeRemaining), 10, 30, 16, YELLOW);
    
    renderSans();
    
    int boxSize = 140;
    int boxX = (screenWidth - boxSize) / 2;
    int boxY = 230;
    
    // Sans fight box
    DrawRectangleLines(boxX, boxY, boxSize, boxSize, WHITE);
    
    for (int i = 0; i < MAX_PLATFORMS; i++) {
        if (mPlatforms[i].active) {
            Texture2D platTex = (i % 2 == 0) ? mUITextures.platform1 : mUITextures.platform2;
            if (platTex.id != 0) {
                DrawTexturePro(
                    platTex,
                    {0, 0, (float)platTex.width, (float)platTex.height},
                    {mPlatforms[i].position.x, mPlatforms[i].position.y, mPlatforms[i].size.x, mPlatforms[i].size.y},
                    {0, 0},
                    0.0f,
                    WHITE
                );
            } else {
                DrawRectangle(
                    (int)mPlatforms[i].position.x,
                    (int)mPlatforms[i].position.y,
                    (int)mPlatforms[i].size.x,
                    (int)mPlatforms[i].size.y,
                    ORANGE
                );
            }
        }
    }
    
    // Render bones
    for (int i = 0; i < MAX_BONES; i++) {
        if (mBones[i].active) {
            Texture2D boneTex = mBones[i].horizontal ? mUITextures.boneH : mUITextures.boneV;
            if (boneTex.id != 0) {
                DrawTexturePro(
                    boneTex,
                    {0, 0, (float)boneTex.width, (float)boneTex.height},
                    {mBones[i].position.x - mBones[i].size.x / 2, 
                     mBones[i].position.y - mBones[i].size.y / 2, 
                     mBones[i].size.x, 
                     mBones[i].size.y},
                    {0, 0},
                    0.0f,
                    WHITE
                );
            } else {
                DrawRectangle(
                    (int)(mBones[i].position.x - mBones[i].size.x / 2),
                    (int)(mBones[i].position.y - mBones[i].size.y / 2),
                    (int)mBones[i].size.x,
                    (int)mBones[i].size.y,
                    WHITE
                );
            }
        }
    }
    
    // render blaster
for (int i = 0; i < MAX_BLASTERS; i++) {
    if (mBlasters[i].active && mBlasters[i].firing) {
        float angleRad = mBlasters[i].angle * DEG2RAD;
        Vector2 beamEnd = {
            mBlasters[i].position.x + cos(angleRad) * 200.0f,
            mBlasters[i].position.y + sin(angleRad) * 200.0f
        };
        
        DrawLineEx(mBlasters[i].position, beamEnd, 30.0f, WHITE);
    }
}
for (int i = 0; i < MAX_BLASTERS; i++) {
    if (mBlasters[i].active) {
        if (mUITextures.blasterSheet.id != 0) {
            
            int halfWidth = mUITextures.blasterSheet.width / 2;
            int halfHeight = mUITextures.blasterSheet.height / 2;
            
            Rectangle sourceRect;
            
            if (mBlasters[i].firing) {
                sourceRect = {
                    (float)59.0f,
                    (float)46.0f,
                    (float)58.0f,
                    (float)44.0f
                };
            } else {
                sourceRect = {
                    0.0f,
                    2.0f,
                    (float)58.0f,
                    (float)43.0f
                };
            }
            
            Rectangle destRect = {
                mBlasters[i].position.x,
                mBlasters[i].position.y,
                50.0f,
                50.0f
            };
            
            Vector2 origin = {25.0f, 25.0f};
            
            DrawTexturePro(
                mUITextures.blasterSheet,
                sourceRect,
                destRect,
                origin,
                mBlasters[i].angle,
                WHITE
            );
        }
    }
}

if (mSoul && mSoul->isActive()) {
    Vector2 soulPos = mSoul->getPosition();
    Texture2D soulTex = mBlueSoulMode ? mUITextures.soulBlue : mUITextures.soulRed;
    
    if (mInvincible && (int)(mInvincibilityTimer * 10) % 2 == 0) {

    } else {
        if (soulTex.id != 0) {
            DrawTexturePro(
                soulTex,
                {0, 0, (float)soulTex.width, (float)soulTex.height},
                {soulPos.x - 5, soulPos.y - 5, 10, 10},
                {0, 0},
                0.0f,
                WHITE
            );
        }
    }
}

    // Apply damage shader effect to soul
if (mInvincible && (int)(mInvincibilityTimer * 10) % 2 == 0) {
    // Skip rendering during invincibility flicker
} else {
    applyDamageShaderToSoul();
}

// Render effects (fade overlays)
if (mScreenEffects) {
    mScreenEffects->render();
}

    renderHealthBar();
    renderButtons();
}

void SansFight::renderSans() {
    int screenWidth = 640;
    int boxY = 230;
    int gapAboveBox = 10;
    int sansFeetY = boxY - gapAboveBox;
    
    Texture2D currentFrame;
    bool hasFrame = false;
    
    if (mSansAnimationFrame == 0 && mUITextures.sansFrame1.id != 0) {
        currentFrame = mUITextures.sansFrame1;
        hasFrame = true;
    } else if (mSansAnimationFrame == 1 && mUITextures.sansFrame2.id != 0) {
        currentFrame = mUITextures.sansFrame2;
        hasFrame = true;
    } else if (mSansAnimationFrame == 2 && mUITextures.sansFrame3.id != 0) {
        currentFrame = mUITextures.sansFrame3;
        hasFrame = true;
    }
    
    if (hasFrame) {
        float scale = 0.20f;
        int scaledWidth = (int)(currentFrame.width * scale);
        int scaledHeight = (int)(currentFrame.height * scale);
        int sansY = sansFeetY - scaledHeight;
        int sansX = (screenWidth - scaledWidth) / 2;
        
        DrawTexturePro(
            currentFrame,
            {0, 0, (float)currentFrame.width, (float)currentFrame.height},
            {(float)sansX, (float)sansY, (float)scaledWidth, (float)scaledHeight},
            {0, 0},
            0.0f,
            WHITE
        );
        
        int labelX = sansX + (scaledWidth / 2) - 15;
        int labelY = sansFeetY + 3;
    }
}

void SansFight::renderHealthBar() {
    int screenWidth = 640;
    int barWidth = 170;
    int barHeight = 18;
    int hpX = (screenWidth / 2) - 35;
    int hpY = 385;
    
    DrawText("HP", hpX - 35, hpY + 1, 16, WHITE);
    DrawText("CHARA", hpX - 200, hpY + 1, 16, WHITE);
    DrawText("LV 19", hpX - 110, hpY + 1, 16, WHITE);
    
    DrawRectangle(hpX, hpY, barWidth, barHeight, (Color){40, 40, 40, 255});
    DrawRectangleLines(hpX, hpY, barWidth, barHeight, WHITE);
    
    float hpPercent = (float)mPlayerHP / (float)MAX_PLAYER_HP;
    Color barColor = YELLOW;
    if (hpPercent <= 0.2f) barColor = RED;
    else if (hpPercent <= 0.5f) barColor = ORANGE;
    
    int filledWidth = (int)((barWidth - 4) * hpPercent);
    DrawRectangle(hpX + 2, hpY + 2, filledWidth, barHeight - 4, barColor);
    
    DrawText(TextFormat("%d / %d", mPlayerHP, MAX_PLAYER_HP),
             hpX + barWidth + 8, hpY + 1, 15, WHITE);
}

void SansFight::renderButtons() {
    int screenWidth = 640;
    int buttonY = 410;
    
    const char* labels[4] = {"FIGHT", "ACT", "ITEM", "MERCY"};
    Texture2D textures[4] = {
        mUITextures.uiFight,
        mUITextures.uiAct,
        mUITextures.uiItem,
        mUITextures.uiMercy
    };
    
    int buttonStartX = 100;
    int spacing = 20;
    
    for (int i = 0; i < 4; i++) {
        int xPos = buttonStartX + i * (100 + spacing);
        
        if (textures[i].id != 0) {
            
            int textureWidth = textures[i].width;
            int textureHeight = textures[i].height;
            int frameHeight = textureHeight / 3;  
            
            Rectangle sourceRect = {
                0.0f,
                0.0f,  
                (float)textureWidth,
                (float)frameHeight
            };
            
            Rectangle destRect = {
                (float)xPos,
                (float)buttonY,
                (float)textureWidth,
                (float)frameHeight
            };
            
            DrawTexturePro(textures[i], sourceRect, destRect, {0, 0}, 0.0f, WHITE);
        } else {
            DrawText(labels[i], xPos, buttonY, 15, ORANGE);
        }
    }
}

void SansFight::keepSoulInBattleBox() {
    int boxSize = 140;
    int boxX = (640 - boxSize) / 2;
    int boxY = 230;
    
    float boxLeft = boxX + 8.0f;
    float boxRight = boxX + boxSize - 8.0f;
    float boxTop = boxY + 8.0f;
    float boxBottom = boxY + boxSize - 8.0f;
    
    Vector2 pos = mSoul->getPosition();
    
    if (pos.x < boxLeft) pos.x = boxLeft;
    if (pos.x > boxRight) pos.x = boxRight;
    if (pos.y < boxTop) pos.y = boxTop;
    if (pos.y > boxBottom) pos.y = boxBottom;
    
    mSoul->setPosition(pos);
}

void SansFight::shutdown() {
    // Save game state before unloading
    bool wasGameOver = mGameOver;
    bool wasPlayerWon = mPlayerWon;
    int finalHP = mPlayerHP;
    
    // Unload textures
    if (mUITextures.boneH.id != 0) UnloadTexture(mUITextures.boneH);
    if (mUITextures.boneV.id != 0) UnloadTexture(mUITextures.boneV);
    if (mUITextures.blasterSheet.id != 0) UnloadTexture(mUITextures.blasterSheet);
    if (mUITextures.blasterBeam.id != 0) UnloadTexture(mUITextures.blasterBeam);
    if (mUITextures.platform1.id != 0) UnloadTexture(mUITextures.platform1);
    if (mUITextures.platform2.id != 0) UnloadTexture(mUITextures.platform2);
    if (mUITextures.combatZoneBorder.id != 0) UnloadTexture(mUITextures.combatZoneBorder);
    if (mUITextures.sansFrame1.id != 0) UnloadTexture(mUITextures.sansFrame1);
    if (mUITextures.sansFrame2.id != 0) UnloadTexture(mUITextures.sansFrame2);
    if (mUITextures.sansFrame3.id != 0) UnloadTexture(mUITextures.sansFrame3);
    if (mUITextures.sansBody.id != 0) UnloadTexture(mUITextures.sansBody);
    if (mUITextures.uiFight.id != 0) UnloadTexture(mUITextures.uiFight);
    if (mUITextures.uiAct.id != 0) UnloadTexture(mUITextures.uiAct);
    if (mUITextures.uiItem.id != 0) UnloadTexture(mUITextures.uiItem);
    if (mUITextures.uiMercy.id != 0) UnloadTexture(mUITextures.uiMercy);
    if (mUITextures.hpBar.id != 0) UnloadTexture(mUITextures.hpBar);
    if (mUITextures.hpBackground.id != 0) UnloadTexture(mUITextures.hpBackground);
    if (mUITextures.soulRed.id != 0) UnloadTexture(mUITextures.soulRed);
    if (mUITextures.soulBlue.id != 0) UnloadTexture(mUITextures.soulBlue);
    
    // Delete entities
    if (mSoul) {
        delete mSoul;
        mSoul = nullptr;
    }
    shutdownShaderSystem();
             
}

void SansFight::updateShaderState(float deltaTime) {
    if (!mShaderLoaded) return;
    if (mDamageShaderIntensity > 0.0f) {
        mDamageShaderIntensity -= 5.0f * deltaTime;  
        if (mDamageShaderIntensity < 0.0f) {
            mDamageShaderIntensity = 0.0f;
        }
    }
    
    mDamageFlashTimer += deltaTime;
}

void SansFight::applyDamageShaderToSoul() {
    if (!mShaderLoaded || mDamageShaderIntensity <= 0.0f) return;
    
    mDamageShader.begin();
    
    mDamageShader.setFloat("damageIntensity", mDamageShaderIntensity);
    mDamageShader.setFloat("damageFlashTime", mDamageFlashTimer);
    mDamageShader.setVector2("lightPosition", {320.0f, 240.0f});
    
    if (mSoul && mSoul->isActive()) {
        Vector2 soulPos = mSoul->getPosition();
        Color soulColor = mBlueSoulMode ? BLUE : RED;
        DrawRectangle(
            (int)soulPos.x - 5,
            (int)soulPos.y - 5,
            10, 10,
            soulColor
        );
    }
    
    mDamageShader.end();
}

void SansFight::shutdownShaderSystem() {
    if (mScreenEffects) {
        delete mScreenEffects;
        mScreenEffects = nullptr;
    }
    
}