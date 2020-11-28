
#include "robot.h"
#include "src/input/controller.h"
#include "src/input/inputfocus.h"
#include "src/collision/collisionscene.h"
#include "src/graphics/renderscene.h"
#include "geo/model.h"
#include "src/audio/playersounds.h"
#include "src/audio/audio.h"
#include "src/puzzle/entranceexit.h"
#include "src/cadet/cadet.h"

#include "src/collision/geo/robot_collision.inc.c"

struct DropShadowParams gRobotShadowParams = {
    1.0f,
    1.5f,
    168.0f,
    94.0f,
    CollisionLayersGeometry,
};

struct Robot gRobot;
struct Vector2 _gMaxRobotRotate;

struct Vector3 gAttackCenter = {0.0f, 0.0f, ROBOT_ATTACK_DISTANCE};

void robotRespawn(struct Robot* robot);

void robotCalcBB(struct Robot* robot, struct Box* box) {
    box->min.x = robot->transform.position.x - ROBOT_BB_RADIUS;
    box->min.y = robot->transform.position.y;
    box->min.z = robot->transform.position.z - ROBOT_BB_RADIUS;

    box->max.x = robot->transform.position.x + ROBOT_BB_RADIUS;
    box->max.y = robot->transform.position.y + 2.0f;
    box->max.z = robot->transform.position.z + ROBOT_BB_RADIUS;
}

void robotRender(struct DynamicActor* data, struct GraphicsState* state) {
    struct Robot* robot = (struct Robot*)data->data;

    if (robot->actor.stateFlags & ROBOT_IS_INVISIBLE) {
        return;
    }

    Mtx* nextTransfrom = graphicsStateNextMtx(state);
    Mtx* reflectMatrix = graphicsStateNextMtx(state);

    LookAt* nextLookat = graphicsStateNextLookat(state);

    guLookAtReflect(reflectMatrix, nextLookat, 
        state->cameraPos->x,
        state->cameraPos->y,
        state->cameraPos->z,
        robot->transform.position.x,
        robot->transform.position.y,
        robot->transform.position.z,
        0,
        1,
        0
    );

    if (robot->teleport.flags & TELEPORT_FLAG_ACTIVE) {
        teleportEffectCreateTransform(&robot->teleport, &robot->transform, nextTransfrom);
    } else {
        transformToMatrixL(data->transform, 1.0f / 256.0f, nextTransfrom);
    }

    gSPLookAt(state->dl++, K0_TO_PHYS(nextLookat));

    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(nextTransfrom), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(state->dl++, _robot_mesh);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
}

void robotMove(struct Robot* robot) {
    struct Vector2 rotatedInput;

    if (gInputMask & InputMaskPlayer && robot->controllerIndex != -1) {
        struct Vector2 input2d = getJoystick(robot->controllerIndex);
        cameraGetMoveDir(&gScene.camera[robot->controllerIndex], &input2d, &rotatedInput);
    } else {
        rotatedInput.x = 0.0f;
        rotatedInput.y = 0.0f;
    }


    robot->actor.velocity.y += GLOBAL_GRAVITY * gTimeDelta;
    struct Vector3 targetVelocity;

    targetVelocity.x = ROBOT_SPEED * rotatedInput.x;
    targetVelocity.y = robot->actor.velocity.y;
    targetVelocity.z = ROBOT_SPEED * rotatedInput.y;

    vector3MoveTowards(
        &robot->actor.velocity, 
        &targetVelocity, 
        ROBOT_ACCEL * gTimeDelta,
        &robot->actor.velocity
    );

    if (rotatedInput.x != 0.0f || rotatedInput.y != 0.0f) {
        float timeLeft = gTimeDelta - MIN_DELTA_TIME;
        struct Vector2 maxRotate = _gMaxRobotRotate;
        struct Vector2 dir;
        vector2Normalize(&rotatedInput, &dir);

        while (timeLeft >= MIN_DELTA_TIME) {
            vector2ComplexMul(&maxRotate, &_gMaxRobotRotate, &maxRotate);
            timeLeft -= MIN_DELTA_TIME;
        }

        vector2RotateTowards(&robot->rotation, &dir, &maxRotate, &robot->rotation);

        dir.x = robot->rotation.y;
        dir.y = robot->rotation.x;

        quatAxisComplex(&gUp, &dir, &robot->transform.rotation);
    }

    if (robot->actor.anchor) {
        transformPoint(robot->actor.anchor, &robot->actor.relativeToAnchor, &robot->transform.position);
    }

    vector3AddScaled(&robot->transform.position, &robot->actor.velocity, gTimeDelta, &robot->transform.position);

    enum SphereActorCollideResult colliderResult = sphereActorCollideScene(&robot->actor, &robot->transform.position);

    if (colliderResult == SphereActorCollideResultKill) {
        audioPlaySound(
            gPlayerSoundIds[PlayerRobotFall],
            0.5f,
            1.0f,
            0.0f,
            10
        );

        teleportEffectStart(&robot->teleport, TELEPORT_FLAG_QUICK);
        robot->state = robotRespawn;
        robot->actor.velocity = gZeroVec;
    }
}

void robotWalk(struct Robot* robot) {
    robotMove(robot);
}

void robotAttack(struct Robot* robot) {
    struct Vector3 attackPos;

    audioRestartPlaySound(
        gPlayerSoundIds[PlayerRobotAttack],
        0.5f,
        1.0f,
        0.0f,
        10
    );

    transformPoint(&robot->transform, &gAttackCenter, &attackPos);
    attackPos.y += ROBOT_ATTACK_RADIUS;
    collisionSceneCollideSphere(&attackPos, ROBOT_ATTACK_RADIUS, CollisionLayersBreakable);

    explosionInit(&robot->explosions[robot->nextExplosion], &attackPos, ExplosionTypeRobot);

    attackPos.y -= ROBOT_ATTACK_RADIUS;
    cadetExplosionAt(&gCadet, &attackPos, ROBOT_ATTACK_RADIUS * 3.0f);

    robot->nextExplosion = (robot->nextExplosion + 1) % ROBOT_MAX_EXPLOSIONS;
}

void robotIdle(struct Robot* robot) {

}

void robotTeleportIn(struct Robot* robot) {
    if (robot->actor.lastStableAnchor) {
        transformPoint(robot->actor.lastStableAnchor, &robot->actor.lastStableLocation, &robot->transform.position);
    }

    if (!teleportEffectUpdate(&gRobot.teleport)) {
        robot->state = robotWalk;
        robot->actor.stateFlags &= ~ROBOT_IS_CUTSCENE;
    }
}

void robotTeleportOut(struct Robot* robot) {
    struct Vector3 targetCenter;

    targetCenter.x = gRobotExit.transform.position.x;
    targetCenter.y = robot->transform.position.y;
    targetCenter.z = gRobotExit.transform.position.z;

    vector3MoveTowards(&robot->transform.position, &targetCenter, gTimeDelta * 0.5f, &robot->transform.position);

    if (!teleportEffectUpdate(&gRobot.teleport)) {
        robot->state = robotIdle;
        robot->actor.stateFlags &= ~ROBOT_IS_CUTSCENE;
        robot->actor.stateFlags |= ROBOT_IS_INVISIBLE;
    }
}

void robotRespawn(struct Robot* robot) {
    if (!teleportEffectUpdate(&robot->teleport)) {
        robot->state = robotTeleportIn;
        teleportEffectStart(&robot->teleport, TELEPORT_FLAG_REVERSE | TELEPORT_FLAG_QUICK);

        if (robot->actor.lastStableAnchor) {
            transformPoint(robot->actor.lastStableAnchor, &robot->actor.lastStableLocation, &robot->transform.position);
        } else {
            robot->transform.position = robot->actor.lastStableLocation;
        }
        robot->actor.velocity = gZeroVec;
        robot->actor.anchor = 0;
    }
}

void robotUpdate(void* robotPtr) {
    struct Robot* robot = (struct Robot*)robotPtr;

    struct Vector3 lastPos = robot->transform.position;

    robot->state(robot);

    if (lastPos.x != robot->transform.position.x || 
        lastPos.y != robot->transform.position.y || 
        lastPos.z != robot->transform.position.z) {
        struct Box nextBB;
        robotCalcBB(robot, &nextBB);
        sparseCollisionReindex(&gSparseCollisionGrid, &gRobot.collider, &nextBB);
    }

    dropShadowCalculate(&robot->shadow, robot->actor.stateFlags & SPHERE_ACTOR_IS_GROUNDED, &robot->transform.position);

    if (gInputMask & InputMaskPlayer) {
        if (getButtonDown(robot->controllerIndex, B_BUTTON)) {
            robotAttack(robot);
        }
    }
}

void robotReset(struct Vector3* startLocation) {
    timeAddListener(&gRobot.updateListener, robotUpdate, &gRobot, TimeUpdateGroupWorld);
    transformIdentity(&gRobot.transform);
    
    dropShadowInit(&gRobot.shadow, &gRobot.transform, &gRobotShadowParams);

    gRobot.transform.position = *startLocation;
    gRobot.state = robotTeleportIn;

    gRobot.actor.radius = ROBOT_RADIUS;
    gRobot.actor.velocity = gZeroVec;
    gRobot.actor.lastStableLocation = *startLocation;
    gRobot.actor.stateFlags = 0;
    gRobot.actor.anchor = 0;
    gRobot.actor.lastStableAnchor = 0;
    gRobot.actor.relativeToAnchor = gZeroVec;
    gRobot.rotation = gUp2;
    gRobot.attackTimer = 0.0f;

    int i;

    for (i = 0; i < ROBOT_MAX_EXPLOSIONS; ++i) {
        explosionReset(&gRobot.explosions[i]);
    }
    gRobot.nextExplosion = 0;
    gRobot.controllerIndex = -1;

    teleportEffectStart(&gRobot.teleport, TELEPORT_FLAG_REVERSE);

    dynamicActorAddToGroup(&gScene.dynamicActors, &gRobot.transform, &gRobot, robotRender, MATERIAL_INDEX_NOT_BATCHED, 2.0f);

    robotCalcBB(&gRobot, &gRobot.collider.lastBoundingBox);
    sparseCollisionAdd(&gSparseCollisionGrid, &gRobot.collider, &gRobot.collider.lastBoundingBox);
}

void robotInit() {
    _gMaxRobotRotate.x = cosf(MIN_DELTA_TIME * ROBOT_TURN_RATE);
    _gMaxRobotRotate.y = sinf(MIN_DELTA_TIME * ROBOT_TURN_RATE);

    gRobot.actor.collisionMask = 
        CollisionLayersGeometry | 
        CollisionLayersRobotSwitch |
        CollisionLayersMutualSwitch |
        CollisionLayersSwamp |
        CollisionLayersKillPlane;

    gRobot.collider.collider = &_robot_collision_collider;
    gRobot.collider.transform = &gRobot.transform;

    gRobot.shadow.params = &gRobotShadowParams;

    robotReset(&gZeroVec);
}

void robotFinishLevel(struct Robot* robot) {
    teleportEffectStart(&gRobot.teleport, 0);
    robot->state = robotTeleportOut;
}