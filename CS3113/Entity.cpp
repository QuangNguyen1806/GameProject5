#include "Entity.h"

Entity::Entity() 
    : mPosition{0.0f, 0.0f}, 
      mVelocity{0.0f, 0.0f}, 
      mAcceleration{0.0f, 0.0f},
      mMovement{0.0f, 0.0f},
      mScale{1.0f, 1.0f},
      mSize{0.0f, 0.0f},
      mSpeed(0.0f),
      mAngle(0.0f),
      mCurrentDirection(DOWN),
      mCurrentFrame(0),
      mAnimationTime(0.0f),
      mAnimationCols(1),
      mAnimationRows(1),
      mAnimationFrames(1),
      mAnimationSpeed(1.0f),
      mCollider{0.0f, 0.0f, 0.0f, 0.0f},
      mColliderDimensions{0.0f, 0.0f},
      mCollidedTop(false),
      mCollidedBottom(false),
      mCollidedLeft(false),
      mCollidedRight(false),
      mJumpingPower(0.0f),
      mIsActive(true),
      mEntityType(PLATFORM),
      mAIType(NONE),
      mAIState(IDLE)
{
}

Entity::Entity(Vector2 position, Vector2 size, const std::string& texture_path, EntityType entityType)
    : mPosition(position),
      mVelocity{0.0f, 0.0f},
      mAcceleration{0.0f, 0.0f},
      mMovement{0.0f, 0.0f},
      mScale{1.0f, 1.0f},
      mSize(size),
      mSpeed(0.0f),
      mAngle(0.0f),
      mCurrentDirection(DOWN),
      mCurrentFrame(0),
      mAnimationTime(0.0f),
      mAnimationCols(1),
      mAnimationRows(1),
      mAnimationFrames(1),
      mAnimationSpeed(1.0f),
      mCollider{position.x, position.y, size.x, size.y},
      mColliderDimensions(size),
      mCollidedTop(false),
      mCollidedBottom(false),
      mCollidedLeft(false),
      mCollidedRight(false),
      mJumpingPower(0.0f),
      mIsActive(true),
      mEntityType(entityType),
      mAIType(NONE),
      mAIState(IDLE)
{
    if (!texture_path.empty()) {
        mTexture = LoadTexture(texture_path.c_str());
    }
}

Entity::~Entity() {
    if (mTexture.id != 0) {
        UnloadTexture(mTexture);
    }
}

void Entity::drawSprite() {
    if (mTexture.id == 0) return;
    
    Rectangle sourceRec = {
        (float)(mCurrentFrame % mAnimationCols) * (mTexture.width / (float)mAnimationCols),
        (float)(mCurrentFrame / mAnimationCols) * (mTexture.height / (float)mAnimationRows),
        (float)mTexture.width / mAnimationCols,
        (float)mTexture.height / mAnimationRows
    };
    
    Rectangle destRec = {
        mPosition.x,
        mPosition.y,
        mSize.x * mScale.x,
        mSize.y * mScale.y
    };
    
    Vector2 origin = {mSize.x * mScale.x / 2.0f, mSize.y * mScale.y / 2.0f};
    
    DrawTexturePro(mTexture, sourceRec, destRec, origin, mAngle, WHITE);
}

void Entity::update(float deltaTime, Entity* player, Entity* collidableEntities, int collidableEntityCount, Map* map) {
    if (!mIsActive) return;
    
    // Update animation
    mAnimationTime += deltaTime;
    if (mAnimationTime >= mAnimationSpeed) {
        mCurrentFrame++;
        if (mCurrentFrame >= mAnimationFrames) {
            mCurrentFrame = 0;
        }
        mAnimationTime = 0.0f;
    }
    
    // Apply velocity
    mPosition.x += mVelocity.x * deltaTime;
    mPosition.y += mVelocity.y * deltaTime;
    
    // Update collider
    mCollider.x = mPosition.x - mColliderDimensions.x / 2.0f;
    mCollider.y = mPosition.y - mColliderDimensions.y / 2.0f;
    mCollider.width = mColliderDimensions.x;
    mCollider.height = mColliderDimensions.y;
    
    // AI behavior
    if (mEntityType == NPC) {
        if (mAIType == WALKER) AIWalker();
        if (mAIType == GUARD) AIGuard();
        if (mAIType == FLYER && player != nullptr) AIFly(player);
    }
    
    // Check collisions
    if (collidableEntities != nullptr && collidableEntityCount > 0) {
        checkCollisionY(collidableEntities, collidableEntityCount);
        checkCollisionX(collidableEntities, collidableEntityCount);
    }
    
    if (map != nullptr) {
        checkCollisionY(map);
        checkCollisionX(map);
    }
}

void Entity::checkCollisionY(Entity* collidableEntities, int collidableEntityCount) {
    for (int i = 0; i < collidableEntityCount; i++) {
        Entity* collidableEntity = &collidableEntities[i];
        
        if (checkCollision(collidableEntity)) {
            float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            float yOverlap = fabs(yDistance - (mSize.y / 2.0f) - (collidableEntity->mSize.y / 2.0f));
            
            if (mVelocity.y > 0) {
                mPosition.y -= yOverlap;
                mVelocity.y = 0;
                mCollidedBottom = true;
                
                if (collidableEntity->mEntityType == BLOCK) {
                    collidableEntity->mIsActive = false;
                }
            } else if (mVelocity.y < 0) {
                mPosition.y += yOverlap;
                mVelocity.y = 0;
                mCollidedTop = true;
            }
        }
    }
}

void Entity::checkCollisionX(Entity* collidableEntities, int collidableEntityCount) {
    for (int i = 0; i < collidableEntityCount; i++) {
        Entity* collidableEntity = &collidableEntities[i];
        
        if (checkCollision(collidableEntity)) {
            float xDistance = fabs(mPosition.x - collidableEntity->mPosition.x);
            float xOverlap = fabs(xDistance - (mSize.x / 2.0f) - (collidableEntity->mSize.x / 2.0f));
            
            if (mVelocity.x > 0) {
                mPosition.x -= xOverlap;
                mVelocity.x = 0;
                mCollidedRight = true;
            } else if (mVelocity.x < 0) {
                mPosition.x += xOverlap;
                mVelocity.x = 0;
                mCollidedLeft = true;
            }
        }
    }
}

void Entity::checkCollisionY(Map* map) {}

void Entity::checkCollisionX(Map* map) {}

bool Entity::checkCollision(Entity* other) const {
    return CheckCollisionRecs(mCollider, other->mCollider);
}

void Entity::AIWalker() {
    // Simple back and forth AI
    if (mVelocity.x == 0) {
        mVelocity.x = mSpeed;
    }
}

void Entity::AIGuard() {
    // Stationary guard AI
    mVelocity.x = 0;
    mVelocity.y = 0;
}

void Entity::AIFly(Entity* target) {
    if (target == nullptr) return;
    
    Vector2 targetPos = target->getPosition();
    Vector2 direction = {
        targetPos.x - mPosition.x,
        targetPos.y - mPosition.y
    };
    
    // Normalize direction
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        direction.x /= length;
        direction.y /= length;
    }
    
    // Set velocity toward target
    mVelocity.x = direction.x * mSpeed;
    mVelocity.y = direction.y * mSpeed;
}

void Entity::AIActivate(Entity* player) {}

EntityType Entity::getEntityType() const { return mEntityType; }
AIType Entity::getAIType() const { return mAIType; }
AIState Entity::getAIState() const { return mAIState; }
Vector2 Entity::getPosition() const { return mPosition; }
Vector2 Entity::getVelocity() const { return mVelocity; }
Vector2 Entity::getAcceleration() const { return mAcceleration; }
Vector2 Entity::getMovement() const { return mMovement; }
Vector2 Entity::getScale() const { return mScale; }
Vector2 Entity::getSize() const { return mSize; }
float Entity::getSpeed() const { return mSpeed; }
float Entity::getAngle() const { return mAngle; }
int Entity::getAnimationCols() const { return mAnimationCols; }
int Entity::getAnimationRows() const { return mAnimationRows; }
int Entity::getAnimationFrames() const { return mAnimationFrames; }
Direction Entity::getCurrentDirection() const { return mCurrentDirection; }
int Entity::getCurrentFrame() const { return mCurrentFrame; }
Rectangle Entity::getCollider() const { return mCollider; }
bool Entity::isActive() const { return mIsActive; }

void Entity::setEntityType(EntityType new_entity_type) { mEntityType = new_entity_type; }
void Entity::setAIType(AIType new_ai_type) { mAIType = new_ai_type; }
void Entity::setAIState(AIState new_state) { mAIState = new_state; }
void Entity::setPosition(Vector2 new_position) { mPosition = new_position; }
void Entity::setVelocity(Vector2 new_velocity) { mVelocity = new_velocity; }
void Entity::setAcceleration(Vector2 new_acceleration) { mAcceleration = new_acceleration; }
void Entity::setMovement(Vector2 new_movement) { mMovement = new_movement; }
void Entity::setScale(Vector2 new_scale) { mScale = new_scale; }
void Entity::setSize(Vector2 new_size) { mSize = new_size; }
void Entity::setSpeed(float new_speed) { mSpeed = new_speed; }
void Entity::setAngle(float new_angle) { mAngle = new_angle; }
void Entity::setAnimationCols(int new_cols) { mAnimationCols = new_cols; }
void Entity::setAnimationRows(int new_rows) { mAnimationRows = new_rows; }
void Entity::setAnimationFrames(int new_frames) { mAnimationFrames = new_frames; }
void Entity::setCurrentDirection(Direction new_direction) { mCurrentDirection = new_direction; }
void Entity::setCurrentFrame(int new_frame) { mCurrentFrame = new_frame; }
void Entity::setJumpingPower(float new_jumping_power) { mJumpingPower = new_jumping_power; }
void Entity::setColliderDimensions(Vector2 new_dimensions) { mColliderDimensions = new_dimensions; }
void Entity::activate() { mIsActive = true; }
void Entity::deactivate() { mIsActive = false; }
