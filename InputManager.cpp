#include "InputManager.h"

InputManager::InputManager()
{
    RtlSecureZeroMemory(&data[0], sizeof(InputData) * INPUT_MAX);
    RtlSecureZeroMemory(&prevData[0], sizeof(InputData) * INPUT_MAX);

    controller = new ControllerInput();
}

InputManager::~InputManager()
{
}

void InputManager::Update(float deltaTime)
{
    //read devices first

    //read keyboard mouse to index 0
    short keyboard[256];
    bool keyReturn;

    memset(keyboard, 0, sizeof(short)*256);
    
    for (int i = 0; i < 256; i++)
    {
        keyboard[i] = GetAsyncKeyState(i);
    }

    //read controllers to 1-5
    controller->Update(deltaTime);

    //set previous
    for (int i = 0; i < INPUT_MAX; i++)
    {
        prevData[i] = data[i];
    }

    //clear previous
    RtlSecureZeroMemory(&data[0], sizeof(InputData)*(MAX_CONTROLLERS+1));

    //read keyboard

    data[0].type = TYPE_KEYBOARD;
    data[0].isConnected = true;

    if (data[0].isConnected)
    {
        data[0].trigger[THUMB_LY] = keyboard[83] ? -1 : 0;
        data[0].trigger[THUMB_LY] += keyboard[87] ? 1 : 0;
        data[0].trigger[THUMB_LX] = keyboard[65] ? -1 : 0;
        data[0].trigger[THUMB_LX] += keyboard[68]  ? 1 : 0;
        //data[0].trigger[THUMB_RX] = keyboard[87] ? 1 : 0;
        //data[0].trigger[THUMB_RY] = keyboard[87] ? 1 : 0;
        //data[0].trigger[LEFT_TRIGGER] = keyboard[87] ? 1 : 0;
        //data[0].trigger[RIGHT_TRIGGER] = keyboard[87] ? 1 : 0;
    }

    //read controllers
    for (int c = 0; c < MAX_CONTROLLERS; c++)
    {
        
        //skip if not connected
        ControllerState *cs = controller->getState(c);

        if (!cs->isConnected)
        {
            continue;
        }

        data[c + 1].isConnected = true;
        data[c + 1].type = TYPE_GAMEPAD;


        /*buttons*/
        if (cs->state.Gamepad.wButtons != 0)
        {
            if (cs->state.Gamepad.wButtons & XINPUT_GAMEPAD_A)
            {
                data[c + 1].buttons[BUTTON_A] = true;
            }
            if (cs->state.Gamepad.wButtons & XINPUT_GAMEPAD_B)
            {
                data[c + 1].buttons[BUTTON_B] = true;
            }
            if (cs->state.Gamepad.wButtons & XINPUT_GAMEPAD_X)
            {
                data[c + 1].buttons[BUTTON_X] = true;
            }
            if (cs->state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
            {
                data[c + 1].buttons[BUTTON_Y] = true;
            }
            if (cs->state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
            {
                data[c + 1].buttons[DPAD_UP] = true;
            }
            if (cs->state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
            {
                data[c + 1].buttons[DPAD_DOWN] = true;
            }
            if (cs->state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
            {
                data[c + 1].buttons[DPAD_LEFT] = true;
            }
            if (cs->state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
            {
                data[c + 1].buttons[DPAD_RIGHT] = true;
            }
            if (cs->state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
            {
                data[c + 1].buttons[LEFT_SHOULDER] = true;
            }
            if (cs->state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
            {
                data[c + 1].buttons[RIGHT_SHOULDER] = true;
            }
            if (cs->state.Gamepad.wButtons & XINPUT_GAMEPAD_START)
            {
                data[c + 1].buttons[START] = true;
            }
            if (cs->state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
            {
                data[c + 1].buttons[BACK] = true;
            }
            if (cs->state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
            {
                data[c + 1].buttons[LEFT_THUMB] = true;
            }
            if (cs->state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
            {
                data[c + 1].buttons[RIGHT_THUMB] = true;
            }
        }

        /*triggers*/

        data[c + 1].trigger[LEFT_TRIGGER] = controller->normalizeTriggers(cs->state.Gamepad.bLeftTrigger);
        data[c + 1].trigger[RIGHT_TRIGGER] = controller->normalizeTriggers(cs->state.Gamepad.bRightTrigger);

        data[c + 1].trigger[THUMB_LX] = controller->normalizeThumbs(cs->state.Gamepad.sThumbLX);
        data[c + 1].trigger[THUMB_LY] = controller->normalizeThumbs(cs->state.Gamepad.sThumbLY);
        data[c + 1].trigger[THUMB_RX] = controller->normalizeThumbs(cs->state.Gamepad.sThumbRX);
        data[c + 1].trigger[THUMB_RY] = controller->normalizeThumbs(cs->state.Gamepad.sThumbRY);
    }


}

InputData* InputManager::getInput(int index)
{
    return &data[index];
}

InputData* InputManager::getPrevInput(int index)
{
    return &prevData[index];
}
