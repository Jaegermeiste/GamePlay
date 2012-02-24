#ifndef PHYSICSCHARACTER_H_
#define PHYSICSCHARACTER_H_

#include "Node.h"
#include "PhysicsRigidBody.h"
#include "PhysicsMotionState.h"
#include "Vector3.h"

namespace gameplay
{

/**
 * Physics controller class for a game character.
 *
 * This class can be used to control the movements and collisions of a character
 * in a game. It interacts with the Physics system to apply gravity and handle
 * collisions, however dynamics are not applied to the character directly by the
 * physics system. Instead, the character's movement is controlled directly by the
 * PhysicsCharacter class. This results in a more responsive and typical game
 * character than would be possible if trying to move a character by applying
 * physical simulation with forces.
 *
 * This class can also be used to control animations for a character. Animation
 * clips can be setup for typical character animations, such as walk, run, jump,
 * etc; and the controller will handle blending between these animations as needed.
 *
 * @todo Add support for collision listeners.
 */
class PhysicsCharacter : public Transform::Listener, public btActionInterface
{
    friend class PhysicsController;

public:

    /**
     * Flags for controlling how a character animation is played back.
     */
    enum AnimationFlags
    {
        /**
         * Plays an animation once and then stops.
         */
        ANIMATION_STOP,

        /**
         * Play an animation once and then resumes the previous playing animation.
         */
        ANIMATION_RESUME,

        /**
         * Plays an animation and repeats it indefinitely.
         */
         ANIMATION_REPEAT
    };

    /**
     * Flags controlling how a character is moved.
     */
    enum MoveFlags
    {
        /**
         * Translates the character.
         */
        MOVE_TRANSLATE = 1,

        /**
         * Rotates the character.
         */
        MOVE_ROTATE = 2
    };

    /**
     * Returns the character node for this PhysicsCharacter.
     *
     * @return The character Node.
     */
    Node* getNode() const;

    /**
     * Returns the maximum step height for the character.
     *
     * @return The maximum step height.
     */
    float getMaxStepHeight() const;

    /**
     * Sets the maximum step height for the character.
     *
     * @param height The maximum step height.
     */
    void setMaxStepHeight(float height);

    /**
     * Returns the maximum slope angle for the character.
     *
     * The maximum slope angle determines the maximum angle of terrain
     * that the character can walk on. Slopes with an angle larger
     * than the specified angle will not allow the character to move on.
     *
     * @return The maximum slope angle.
     */
    float getMaxSlopeAngle() const;

    /**
     * Sets the maximum slope angle (in degrees).
     *
     * @param angle The maximum slope angle.
     */
    void setMaxSlopeAngle(float angle);

    /**
     * Configures a new animation for this character.
     *
     * This method registers an animation for the character, with an associated movement speed.
     * The moveSpeed specifies how fast the character moves while the animation is playing.
     * The final velocity of the character is the product of the current move velocity and
     * the currently playing animation(s) moveSpeed.
     *
     * @param name Name of the animation.
     * @param animationClip Animation clip associated with the new character animation.
     * @param moveSpeed Base movement speed (meters per second) associated with the animation.
     */
    void addAnimation(const char* name, AnimationClip* animationClip, float moveSpeed);

    /**
     * Returns the animation with the specified name.
     *
     * @return The specified animation clip.
     */
    AnimationClip* getAnimation(const char* name) const;

    /**
     * Plays the specified animation.
     *
     * There are some limiations and considerations that should be ponited out when
     * playing animations:
     * <li>You should avoid playing multiple animations concurrently that have the same target.
     * For example, two animations targetting the character's joints should not be played 
     * concurrently, but it is fine to play one animation that targets the joints and another
     * that targets the character's Node.
     * <li>When playing an animation that targets the transform of the character's Node
     * (such as a motion path animation), the character's velocity vector should be set to
     * Vector3::zero() so that the PhysicsCharacter stops applying motion directly
     * and instead relies on the motion animation to control the character.
     *
     * The optional animation layer can be used to group animations on separate layers.
     * Each animation layer can have at most one active animation. Playing multiple
     * animations concurrently can be achieved by putting the different animations
     * on separate layers. For example, a motion path animation that targets the
     * character's Node can be put on one layer, while a running animation that targets
     * a character's Joints can be put on a separate layer. This allows a character's
     * movement to be animated at the same time as the run animation is playing.
     *
     * @param name Animation name, or NULL to stop all character animations on the given layer.
     * @param flags Animation flags from the AnimationFlags enumeration.
     * @param speed Optional animation speed (default is 1.0).
     * @param blendDuration Optional number of milliseconds to crossfade between the
     *      currently playing animation on the given layer and the new animation.
     * @param layer Optional animation layer.
     */
    void play(const char* name, AnimationFlags flags, float animationSpeed = 1.0f, unsigned int blendDuration = 0, unsigned int layer = 0);

    /**
     * Sets the velocity of the character.
     *
     * Calling this function sets the velocity (speed and direction) for the character.
     * The velocity is maintained until this method is called again. The final velocity
     * of the character is determined by product of the current velocity vector(s)
     * and the current character animation's move speed. Therefore, specifying a
     * normalized (unit-length) velocity vector results in the character speed being
     * controled entirely by the current animation's velocity; whereas the speed of
     * the character can be augmented by modifying the magnitude of the velocity vector.
     *
     * Note that a zero velocity vector and/or a zero animation move speed will
     * result in no character movement (the character will be stationary). A zero
     * velocity vector should be used when playing an animation that targets the
     * character's transform directly (such as a motion path animation), since these
     * animations will overwrite any transformations on the character's node.
     *
     * @param velocity Movement velocity.
     * @param flags Optional bitwise combination of MoveFlags constants.
     */
    void setVelocity(const Vector3& velocity, unsigned int flags = MOVE_TRANSLATE | MOVE_ROTATE);

    /**
     * Moves the character forward with the given velocity vector.
     *
     * The forward velocity is defined by the character's current orientation
     * (it is the forward vector from the character's current world transform).
     *
     * The specified velocity acts as a multiplier on the currently playing animation's
     * velocity (or, if there is no animation playing, it directly impacts velocity).
     *
     * Note that a negative velocity (i.e. -1.0f) will move the character backwards.
     *
     * @param velocity Optional velocity modifier.
     */
    void setForwardVelocity(float velocity = 1.0f);

    /**
     * Moves the character right with the given velocity vector.
     *
     * The right velocity is defined by the character's current orientation
     * (it is the right vector from the character's current world transform).
     *
     * The specified velocity acts as a multiplier on the currently playing animation's
     * velocity (or, if there is no animation playing, it directly impacts velocity).
     *
     * Note that a negative velocity (i.e. -1.0f) will move the character left.
     *
     * @param velocity Optional velocity modifier.
     */
    void setRightVelocity(float velocity = 1.0f);

    /**
     * Causes the character to jump with the specified initial upwards velocity.
     *
     * @param velocity Initial jump velocity.
     */
    void jump(float height);

    /**
     * @see Transform::Listener::transformChanged
     */
    void transformChanged(Transform* transform, long cookie);

    /**
     * @see btActionInterface::updateAction
     */
    void updateAction(btCollisionWorld* collisionWorld, btScalar deltaTimeStep);

    /**
     * @see btActionInterface::debugDraw
     */
	void debugDraw(btIDebugDraw* debugDrawer);

private:

    struct CharacterAnimation
    {
        const char* name;
        AnimationClip* clip;
        float moveSpeed;
        unsigned int layer;
        bool playing;
        AnimationFlags animationFlags;
        unsigned int blendDuration;
        CharacterAnimation* prev;
    };

    /**
     * Creates a new PhysicsCharacter.
     *
     * @param node Scene node that represents the character.
     * @param radius Radius of capsule volume used for character collisions.
     * @param height Height of the capsule volume used for character collisions.
     * @param center Center point of the capsule volume for the character.
     */
    PhysicsCharacter(Node* node, float radius, float height, const Vector3 center = Vector3::zero());

    /**
     * Destructor.
     */
    virtual ~PhysicsCharacter();

    void updateCurrentVelocity();

    void play(CharacterAnimation* animation, unsigned int layer);

    void stepUp(btCollisionWorld* collisionWorld, btScalar time);

    void stepDown(btCollisionWorld* collisionWorld, btScalar time);

    void stepForwardAndStrafe(btCollisionWorld* collisionWorld, float time);

    void updateTargetPositionFromCollision(btVector3& targetPosition, const btVector3& collisionNormal);

    bool fixCollision(btCollisionWorld* world);

    Node* _node;
    PhysicsMotionState* _motionState;
    btVector3 _moveVelocity;
    float _forwardVelocity;
    float _rightVelocity;
    btVector3 _fallVelocity;
    btVector3 _currentVelocity;
    btVector3 _normalizedVelocity;
    unsigned int _moveFlags;
    bool _colliding;
    btVector3 _collisionNormal;
    btVector3 _currentPosition;
    std::map<const char*, CharacterAnimation> _animations;
    std::map<unsigned int, CharacterAnimation*> _layers;
    btPairCachingGhostObject* _ghostObject;
    btConvexShape* _collisionShape;
    btManifoldArray	_manifoldArray;
    int _ignoreTransformChanged;
    float _stepHeight;
    float _slopeAngle;
    float _cosSlopeAngle;
};

}

#endif