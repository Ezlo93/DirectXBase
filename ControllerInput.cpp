#include "ControllerInput.h"

ControllerInput::ControllerInput() {

    RtlSecureZeroMemory(controllers, sizeof(ControllerState) * MAX_CONTROLLERS);
    cTimer = 0.f;
    isInit = false;
    currentUpdate = 0;
}

ControllerInput::~ControllerInput() {

    RtlSecureZeroMemory(controllers, sizeof(ControllerState) * MAX_CONTROLLERS);
    Disable();

}

/*erase controller data for controller index or all controllers
  if index is not between 0 and 3*/
void ControllerInput::resetControllerData(int index) {

    if (index >= 0 && index < 4) {
        RtlSecureZeroMemory(&controllers[index], sizeof(ControllerState));
    }
    else {
        RtlSecureZeroMemory(controllers, sizeof(ControllerState) * MAX_CONTROLLERS);
    }
}

/*poll all controller input*/
void ControllerInput::Update(float deltaTime) {

    Update_Internal(deltaTime, isInit);
    isInit = true;
}

void ControllerInput::Update_Internal(float deltaTime, bool init) {

    /*cycle to next controller if needed*/
    cTimer += deltaTime;
    currentUpdate = (currentUpdate + 1) % 4;
    cTimer -= CAP_BATTERY_POLL_TIME;


    DWORD res;
    for (DWORD i = 0; i < MAX_CONTROLLERS; i++) {

        /*query all controllers*/
        res = XInputGetState(i, &controllers[i].state);

        /*is connected?*/
        if (res == ERROR_SUCCESS) {
            /*mark as new connection in order to query type/battery*/
            int isNewConnection = !controllers[i].isConnected;
            controllers[i].isConnected = true;

            /*Dead Zone*/
            if ((controllers[i].state.Gamepad.sThumbLX < INPUT_DEADZONE &&
                controllers[i].state.Gamepad.sThumbLX > -INPUT_DEADZONE) &&
                (controllers[i].state.Gamepad.sThumbLY < INPUT_DEADZONE &&
                    controllers[i].state.Gamepad.sThumbLY > -INPUT_DEADZONE))
            {
                controllers[i].state.Gamepad.sThumbLX = 0;
                controllers[i].state.Gamepad.sThumbLY = 0;
            }

            if ((controllers[i].state.Gamepad.sThumbRX < INPUT_DEADZONE &&
                controllers[i].state.Gamepad.sThumbRX > -INPUT_DEADZONE) &&
                (controllers[i].state.Gamepad.sThumbRY < INPUT_DEADZONE &&
                    controllers[i].state.Gamepad.sThumbRY > -INPUT_DEADZONE))
            {
                controllers[i].state.Gamepad.sThumbRX = 0;
                controllers[i].state.Gamepad.sThumbRY = 0;
            }

            /*capabilites and battery information*/

            if (init == false || isNewConnection || (i == currentUpdate && cTimer >= CAP_BATTERY_POLL_TIME)) {


                res = XInputGetCapabilities(i, XINPUT_FLAG_GAMEPAD, &controllers[i].capabilities);

                if (res == ERROR_SUCCESS && controllers[i].capabilities.Flags == XINPUT_CAPS_WIRELESS) {
                    XInputGetBatteryInformation(i, BATTERY_DEVTYPE_GAMEPAD, &controllers[i].batteryInfo);
                }
            }

        }else {
            /*reset data structure*/
            controllers[i].isConnected = false;
            resetControllerData(i);
        }
        
    }


}

/*access information for controller index*/
ControllerState* ControllerInput::getState(int index) {

    return &controllers[index];

}


/*enable xinput*/
void ControllerInput::Enable() {

    XInputEnable(true);

}

/*disable xinput*/
void ControllerInput::Disable() {
    
    XInputEnable(false);

}

bool ControllerInput::isConnected(int index)
{
    return controllers[index].isConnected;
}

/*normalize 16bit signed to -1 - +1*/
float ControllerInput::normalizeThumbs(int in)
{
    return (in / 32768.f);
}

float ControllerInput::normalizeTriggers(int in)
{
    return (in / 255.f);
}
