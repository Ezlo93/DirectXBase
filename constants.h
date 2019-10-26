#pragma once


/*camera.h*/
#define CAMERA_RESTRICTION_ANGLE 10.f

/*directxbase.h*/
#define MODEL_PATH "data/models"
#define TEXTURE_PATH "data/textures"
#define SOUND_PATH "data/sound"
#define PLAYER_DISTANCE 40.f

#define INTROCAMERA_RADIUS 80.f
#define INTROCAMERA_HEIGHT 35.f
#define INTROCAMERA_SPEED 0.025f

#define ENDCAMERA_X -13
#define ENDCAMERA_Y 22
#define ENDCAMERA_Z -30

#define TRANSITION_TIME 0.9f
#define POST_PROCESS
#define END_TIME_V 9.f

/*ball.h*/
#define BALL_BORDER (PLAYER_DISTANCE + 3)
#define MAX_VELOCITY 500.f
#define START_VELOCITY 35.f
#define SPAWN_FREEZE .75f

/*level.h*/
#define LEVEL_PATH "data/levels/"

/*modelcollection.h*/
#define DEFAULT_NONE "!none!"

/*modelinstancestatic.h*/
#define DEFAULT_PLANE "defaultPlane"
#define DEFAULT_SPHERE "defaultSphere"
#define DEFAULT_CUBE "defaultCube"

/*playablechar.h*/
#define PLAYER_HEIGHT 1.01f

/*player.h*/
#ifdef _DEBUG
#define MAX_HP 1
#else
#define MAX_HP 3
#endif

/*shadowmap.h*/
#define SHADOW_HIGH 4096

/*shader.h*/
#define NUM_THREADS 256.f

