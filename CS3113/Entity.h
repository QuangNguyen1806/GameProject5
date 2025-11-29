#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"
#include "Map.h"

enum EntityType { PLAYER, PLATFORM, ENEMY, BONE, BLASTER, PROJECTILE, BLOCK, NPC }; 
enum AIType     { WALKER, GUARD, FLYER, NONE };
enum AIState    { IDLE, WALKING, ATTACKING, PATROLLING, DYING };
enum Direction  { LEFT, RIGHT, UP, DOWN };

class Entity
{
public:
    Vector2 mPosition;
    Vector2 mVelocity;
    Vector2 mAcceleration;
    Vector2 mMovement;
    Vector2 mScale;
    Vector2 mSize;
    
    float mSpeed;
    float mAngle;
    
    Texture2D mTexture;
    std::map<Direction, std::vector<int>> mAnimationIndices;
    Direction mCurrentDirection;
    int mCurrentFrame;
    float mAnimationTime;
    int mAnimationCols;
    int mAnimationRows;
    int mAnimationFrames;
    float mAnimationSpeed;
    
    Rectangle mCollider;
    Vector2 mColliderDimensions;
    bool mCollidedTop;
    bool mCollidedBottom;
    bool mCollidedLeft;
    bool mCollidedRight;
    
    float mJumpingPower;
    bool mIsActive;  
    
    EntityType mEntityType;
    AIType mAIType;
    AIState mAIState;
    
public:
    static constexpr int SECONDS_PER_FRAME = 4;
    
    Entity();
    Entity(Vector2 position, Vector2 size, const std::string& texture_path, EntityType entityType);
    ~Entity();
    
    void drawSprite();
    void update(float deltaTime, Entity* player, Entity* collidableEntities, int collidableEntityCount, Map* map);
    
    // Collision detection
    void checkCollisionY(Entity* collidableEntities, int collidableEntityCount);
    void checkCollisionX(Entity* collidableEntities, int collidableEntityCount);
    void checkCollisionY(Map* map);
    void checkCollisionX(Map* map);
    bool checkCollision(Entity* other) const;
    
    // AI
    void AIWalker();
    void AIGuard();
    void AIFly(Entity* target);  
    void AIActivate(Entity* player);
    
    // ————— GETTERS ————— //
    EntityType getEntityType() const;
    AIType getAIType() const;
    AIState getAIState() const;
    Vector2 getPosition() const;
    Vector2 getVelocity() const;
    Vector2 getAcceleration() const;
    Vector2 getMovement() const;
    Vector2 getScale() const;
    Vector2 getSize() const;
    float getSpeed() const;
    float getAngle() const;
    int getAnimationCols() const;
    int getAnimationRows() const;
    int getAnimationFrames() const;
    Direction getCurrentDirection() const;
    int getCurrentFrame() const;
    Rectangle getCollider() const;
    bool isActive() const;  
    
    // ————— SETTERS ————— //
    void setEntityType(EntityType new_entity_type);
    void setAIType(AIType new_ai_type);
    void setAIState(AIState new_state);
    void setPosition(Vector2 new_position);
    void setVelocity(Vector2 new_velocity);
    void setAcceleration(Vector2 new_acceleration);
    void setMovement(Vector2 new_movement);
    void setScale(Vector2 new_scale);
    void setSize(Vector2 new_size);
    void setSpeed(float new_speed);
    void setAngle(float new_angle);
    void setAnimationCols(int new_cols);
    void setAnimationRows(int new_rows);
    void setAnimationFrames(int new_frames);
    void setCurrentDirection(Direction new_direction);
    void setCurrentFrame(int new_frame);
    void setJumpingPower(float new_jumping_power);
    void setColliderDimensions(Vector2 new_dimensions);
    void activate();
    void deactivate();
};

#endif // ENTITY_H
