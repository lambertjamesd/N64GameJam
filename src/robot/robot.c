
#include "robot.h"
#include "src/input/controller.h"
#include "src/input/inputfocus.h"
#include "src/collision/collisionscene.h"
#include "src/graphics/renderscene.h"
#include "geo/model.h"

struct Robot gRobot;
struct Vector2 _gMaxRobotRotate;

void robotRender(struct DynamicActor* data, struct GraphicsState* state) {
    Mtx* nextTransfrom = graphicsStateNextMtx(state);

    transformToMatrixL(data->transform, 1.0f / 256.0f, nextTransfrom);
    gSPMatrix(state->dl++, OS_K0_TO_PHYSICAL(nextTransfrom), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(state->dl++, _robot_mesh);
    gSPPopMatrix(state->dl++, G_MTX_MODELVIEW);
}

void robotMove(struct Robot* robot) {
    struct Vector2 input = {0.0f, 0.0f};

    if (gInputMask & InputMaskRobot) {
        input.x = gControllerState[0].stick_x / 80.0f;
        input.y = -gControllerState[0].stick_y / 80.0f;
    }

    robot->actor.velocity.y += GLOBAL_GRAVITY * gTimeDelta;
    struct Vector3 targetVelocity;

    targetVelocity.x = ROBOT_SPEED * input.x;
    targetVelocity.y = robot->actor.velocity.y;
    targetVelocity.z = ROBOT_SPEED * input.y;

    vector3MoveTowards(
        &robot->actor.velocity, 
        &targetVelocity, 
        ROBOT_ACCEL * gTimeDelta,
        &robot->actor.velocity
    );

    if (input.x != 0.0f || input.y != 0.0f) {
        float timeLeft = gTimeDelta - MIN_DELTA_TIME;
        struct Vector2 maxRotate = _gMaxRobotRotate;
        struct Vector2 dir;
        vector2Normalize(&input, &dir);

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

    sphereActorCollideScene(&robot->actor, &robot->transform.position);
}

void robotWalk(struct Robot* robot) {
    robotMove(robot);
}

void robotUpdate(void* robotPtr) {
    struct Robot* robot = (struct Robot*)robotPtr;
    robot->state(robot);

    if (gInputMask & InputMaskRobot) {
        if (getButtonDown(0, L_TRIG | Z_TRIG)) {
            gInputMask = INPUT_MASK_CADET;
        }

        gScene.camera.targetPosition = robot->transform.position;
    }
}

void robotReset(struct Vector3* startLocation) {
    transformIdentity(&gRobot.transform);
    
    gRobot.transform.position = *startLocation;
    gRobot.state = robotWalk;

    gRobot.actor.radius = ROBOT_RADIUS;
    gRobot.actor.velocity = gZeroVec;
    gRobot.actor.lastStableLocation = *startLocation;
    gRobot.actor.stateFlags = 0;
    gRobot.actor.anchor = 0;
    gRobot.actor.relativeToAnchor = gZeroVec;
    gRobot.rotation = gUp2;

    dynamicActorAddToGroup(&gScene.dynamicActors, &gRobot.transform, &gRobot, robotRender, MATERIAL_INDEX_NOT_BATCHED);
}

void robotInit() {
    timeAddListener(&gRobot.updateListener, robotUpdate, &gRobot);
    robotReset(&gZeroVec);

    _gMaxRobotRotate.x = cosf(MIN_DELTA_TIME * ROBOT_TURN_RATE);
    _gMaxRobotRotate.y = sinf(MIN_DELTA_TIME * ROBOT_TURN_RATE);
}