/*
File:   win32_keymap.cpp
Author: Taylor Robbins
Date:   06\09\2017
Description: 
	** Handles mapping windows key codes to our own enumeration.
	** Also contains functions for handling keyboard input in widows

#included from win32_main.cpp 
*/

Buttons_t AppButtonForKey(i32 glfwKeyCode)
{
	switch (glfwKeyCode)
	{
		case GLFW_KEY_A:             return Button_A;
		case GLFW_KEY_B:             return Button_B;
		case GLFW_KEY_C:             return Button_C;
		case GLFW_KEY_D:             return Button_D;
		case GLFW_KEY_E:             return Button_E;
		case GLFW_KEY_F:             return Button_F;
		case GLFW_KEY_G:             return Button_G;
		case GLFW_KEY_H:             return Button_H;
		case GLFW_KEY_I:             return Button_I;
		case GLFW_KEY_J:             return Button_J;
		case GLFW_KEY_K:             return Button_K;
		case GLFW_KEY_L:             return Button_L;
		case GLFW_KEY_M:             return Button_M;
		case GLFW_KEY_N:             return Button_N;
		case GLFW_KEY_O:             return Button_O;
		case GLFW_KEY_P:             return Button_P;
		case GLFW_KEY_Q:             return Button_Q;
		case GLFW_KEY_R:             return Button_R;
		case GLFW_KEY_S:             return Button_S;
		case GLFW_KEY_T:             return Button_T;
		case GLFW_KEY_U:             return Button_U;
		case GLFW_KEY_V:             return Button_V;
		case GLFW_KEY_W:             return Button_W;
		case GLFW_KEY_X:             return Button_X;
		case GLFW_KEY_Y:             return Button_Y;
		case GLFW_KEY_Z:             return Button_Z;
		
		case GLFW_KEY_0:             return Button_0;
		case GLFW_KEY_1:             return Button_1;
		case GLFW_KEY_2:             return Button_2;
		case GLFW_KEY_3:             return Button_3;
		case GLFW_KEY_4:             return Button_4;
		case GLFW_KEY_5:             return Button_5;
		case GLFW_KEY_6:             return Button_6;
		case GLFW_KEY_7:             return Button_7;
		case GLFW_KEY_8:             return Button_8;
		case GLFW_KEY_9:             return Button_9;
		
		case GLFW_KEY_F1:            return Button_F1;
		case GLFW_KEY_F2:            return Button_F2;
		case GLFW_KEY_F3:            return Button_F3;
		case GLFW_KEY_F4:            return Button_F4;
		case GLFW_KEY_F5:            return Button_F5;
		case GLFW_KEY_F6:            return Button_F6;
		case GLFW_KEY_F7:            return Button_F7;
		case GLFW_KEY_F8:            return Button_F8;
		case GLFW_KEY_F9:            return Button_F9;
		case GLFW_KEY_F10:           return Button_F10;
		case GLFW_KEY_F11:           return Button_F11;
		case GLFW_KEY_F12:           return Button_F12;
		
		case GLFW_KEY_ENTER:         return Button_Enter;
		case GLFW_KEY_BACKSPACE:     return Button_Backspace;
		case GLFW_KEY_ESCAPE:        return Button_Escape;
		case GLFW_KEY_INSERT:        return Button_Insert;
		case GLFW_KEY_DELETE:        return Button_Delete;
		case GLFW_KEY_HOME:          return Button_Home;
		case GLFW_KEY_END:           return Button_End;
		case GLFW_KEY_PAGE_UP:       return Button_PageUp;
		case GLFW_KEY_PAGE_DOWN:     return Button_PageDown;
		case GLFW_KEY_TAB:           return Button_Tab;
		case GLFW_KEY_CAPS_LOCK:     return Button_CapsLock;
		
		case GLFW_KEY_LEFT_CONTROL:
		case GLFW_KEY_RIGHT_CONTROL: return Button_Control;
		case GLFW_KEY_LEFT_ALT:
		case GLFW_KEY_RIGHT_ALT:     return Button_Alt;
		case GLFW_KEY_LEFT_SHIFT:
		case GLFW_KEY_RIGHT_SHIFT:   return Button_Shift;
		
		case GLFW_KEY_RIGHT:         return Button_Right;
		case GLFW_KEY_LEFT:          return Button_Left;
		case GLFW_KEY_UP:            return Button_Up;
		case GLFW_KEY_DOWN:          return Button_Down;
		
		case GLFW_KEY_EQUAL:         return Button_Plus;
		case GLFW_KEY_MINUS:         return Button_Minus;
		case GLFW_KEY_BACKSLASH:     return Button_Pipe;
		case GLFW_KEY_LEFT_BRACKET:  return Button_OpenBracket;
		case GLFW_KEY_RIGHT_BRACKET: return Button_CloseBracket;
		case GLFW_KEY_SEMICOLON:     return Button_Colon;
		case GLFW_KEY_APOSTROPHE:    return Button_Quote;
		case GLFW_KEY_COMMA:         return Button_Comma;
		case GLFW_KEY_PERIOD:        return Button_Period;
		case GLFW_KEY_SLASH:         return Button_QuestionMark;
		case GLFW_KEY_GRAVE_ACCENT:  return Button_Tilde;
		case GLFW_KEY_SPACE:         return Button_Space;
		
		
		default:                     return Buttons_NumButtons;
	};
}

u8 GetKeysForButton(Buttons_t button, i32* keyBuffer)
{
	switch (button)
	{
		case Button_A:            keyBuffer[0] = GLFW_KEY_A; return 1;
		case Button_B:            keyBuffer[0] = GLFW_KEY_B; return 1;
		case Button_C:            keyBuffer[0] = GLFW_KEY_C; return 1;
		case Button_D:            keyBuffer[0] = GLFW_KEY_D; return 1;
		case Button_E:            keyBuffer[0] = GLFW_KEY_E; return 1;
		case Button_F:            keyBuffer[0] = GLFW_KEY_F; return 1;
		case Button_G:            keyBuffer[0] = GLFW_KEY_G; return 1;
		case Button_H:            keyBuffer[0] = GLFW_KEY_H; return 1;
		case Button_I:            keyBuffer[0] = GLFW_KEY_I; return 1;
		case Button_J:            keyBuffer[0] = GLFW_KEY_J; return 1;
		case Button_K:            keyBuffer[0] = GLFW_KEY_K; return 1;
		case Button_L:            keyBuffer[0] = GLFW_KEY_L; return 1;
		case Button_M:            keyBuffer[0] = GLFW_KEY_M; return 1;
		case Button_N:            keyBuffer[0] = GLFW_KEY_N; return 1;
		case Button_O:            keyBuffer[0] = GLFW_KEY_O; return 1;
		case Button_P:            keyBuffer[0] = GLFW_KEY_P; return 1;
		case Button_Q:            keyBuffer[0] = GLFW_KEY_Q; return 1;
		case Button_R:            keyBuffer[0] = GLFW_KEY_R; return 1;
		case Button_S:            keyBuffer[0] = GLFW_KEY_S; return 1;
		case Button_T:            keyBuffer[0] = GLFW_KEY_T; return 1;
		case Button_U:            keyBuffer[0] = GLFW_KEY_U; return 1;
		case Button_V:            keyBuffer[0] = GLFW_KEY_V; return 1;
		case Button_W:            keyBuffer[0] = GLFW_KEY_W; return 1;
		case Button_X:            keyBuffer[0] = GLFW_KEY_X; return 1;
		case Button_Y:            keyBuffer[0] = GLFW_KEY_Y; return 1;
		case Button_Z:            keyBuffer[0] = GLFW_KEY_Z; return 1;
		
		case Button_0:            keyBuffer[0] = GLFW_KEY_0; return 1;
		case Button_1:            keyBuffer[0] = GLFW_KEY_1; return 1;
		case Button_2:            keyBuffer[0] = GLFW_KEY_2; return 1;
		case Button_3:            keyBuffer[0] = GLFW_KEY_3; return 1;
		case Button_4:            keyBuffer[0] = GLFW_KEY_4; return 1;
		case Button_5:            keyBuffer[0] = GLFW_KEY_5; return 1;
		case Button_6:            keyBuffer[0] = GLFW_KEY_6; return 1;
		case Button_7:            keyBuffer[0] = GLFW_KEY_7; return 1;
		case Button_8:            keyBuffer[0] = GLFW_KEY_8; return 1;
		case Button_9:            keyBuffer[0] = GLFW_KEY_9; return 1;
		
		case Button_F1:           keyBuffer[0] = GLFW_KEY_F1; return 1;
		case Button_F2:           keyBuffer[0] = GLFW_KEY_F2; return 1;
		case Button_F3:           keyBuffer[0] = GLFW_KEY_F3; return 1;
		case Button_F4:           keyBuffer[0] = GLFW_KEY_F4; return 1;
		case Button_F5:           keyBuffer[0] = GLFW_KEY_F5; return 1;
		case Button_F6:           keyBuffer[0] = GLFW_KEY_F6; return 1;
		case Button_F7:           keyBuffer[0] = GLFW_KEY_F7; return 1;
		case Button_F8:           keyBuffer[0] = GLFW_KEY_F8; return 1;
		case Button_F9:           keyBuffer[0] = GLFW_KEY_F9; return 1;
		case Button_F10:          keyBuffer[0] = GLFW_KEY_F10; return 1;
		case Button_F11:          keyBuffer[0] = GLFW_KEY_F11; return 1;
		case Button_F12:          keyBuffer[0] = GLFW_KEY_F12; return 1;
		
		case Button_Enter:        keyBuffer[0] = GLFW_KEY_ENTER; return 1;
		case Button_Backspace:    keyBuffer[0] = GLFW_KEY_BACKSPACE; return 1;
		case Button_Escape:       keyBuffer[0] = GLFW_KEY_ESCAPE; return 1;
		case Button_Insert:       keyBuffer[0] = GLFW_KEY_INSERT; return 1;
		case Button_Delete:       keyBuffer[0] = GLFW_KEY_DELETE; return 1;
		case Button_Home:         keyBuffer[0] = GLFW_KEY_HOME; return 1;
		case Button_End:          keyBuffer[0] = GLFW_KEY_END; return 1;
		case Button_PageUp:       keyBuffer[0] = GLFW_KEY_PAGE_UP; return 1;
		case Button_PageDown:     keyBuffer[0] = GLFW_KEY_PAGE_DOWN; return 1;
		case Button_Tab:          keyBuffer[0] = GLFW_KEY_TAB; return 1;
		case Button_CapsLock:     keyBuffer[0] = GLFW_KEY_CAPS_LOCK; return 1;
		
		case Button_Control:      keyBuffer[0] = GLFW_KEY_LEFT_CONTROL; keyBuffer[1] = GLFW_KEY_RIGHT_CONTROL; return 2;
		case Button_Alt:          keyBuffer[0] = GLFW_KEY_LEFT_ALT;     keyBuffer[1] = GLFW_KEY_RIGHT_ALT; return 2;  
		case Button_Shift:        keyBuffer[0] = GLFW_KEY_LEFT_SHIFT;   keyBuffer[1] = GLFW_KEY_RIGHT_SHIFT; return 2;
		
		case Button_Right:        keyBuffer[0] = GLFW_KEY_RIGHT; return 1;
		case Button_Left:         keyBuffer[0] = GLFW_KEY_LEFT; return 1;
		case Button_Up:           keyBuffer[0] = GLFW_KEY_UP; return 1;
		case Button_Down:         keyBuffer[0] = GLFW_KEY_DOWN; return 1;
		
		case Button_Plus:         keyBuffer[0] = GLFW_KEY_EQUAL; return 1;
		case Button_Minus:        keyBuffer[0] = GLFW_KEY_MINUS; return 1;
		case Button_Pipe:         keyBuffer[0] = GLFW_KEY_BACKSLASH; return 1;
		case Button_OpenBracket:  keyBuffer[0] = GLFW_KEY_LEFT_BRACKET; return 1;
		case Button_CloseBracket: keyBuffer[0] = GLFW_KEY_RIGHT_BRACKET; return 1;
		case Button_Colon:        keyBuffer[0] = GLFW_KEY_SEMICOLON; return 1;
		case Button_Quote:        keyBuffer[0] = GLFW_KEY_APOSTROPHE; return 1;
		case Button_Comma:        keyBuffer[0] = GLFW_KEY_COMMA; return 1;
		case Button_Period:       keyBuffer[0] = GLFW_KEY_PERIOD; return 1;
		case Button_QuestionMark: keyBuffer[0] = GLFW_KEY_SLASH; return 1;
		case Button_Tilde:        keyBuffer[0] = GLFW_KEY_GRAVE_ACCENT; return 1;
		case Button_Space:        keyBuffer[0] = GLFW_KEY_SPACE; return 1;
		
		
		default: return 0;
	};
}

Buttons_t AppButtonForMouse(i32 glfwButtonCode)
{
	switch (glfwButtonCode)
	{
		case GLFW_MOUSE_BUTTON_LEFT:   return MouseButton_Left;
		case GLFW_MOUSE_BUTTON_RIGHT:  return MouseButton_Right;
		case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButton_Middle;
		
		
		default:                       return Buttons_NumButtons;
	};
}

u8 GetMouseBtnsForButton(Buttons_t button, i32* keyBuffer)
{
	switch (button)
	{
		case MouseButton_Left:   keyBuffer[0] = GLFW_MOUSE_BUTTON_LEFT; return 1;
		case MouseButton_Right:  keyBuffer[0] = GLFW_MOUSE_BUTTON_RIGHT; return 1;
		case MouseButton_Middle: keyBuffer[0] = GLFW_MOUSE_BUTTON_MIDDLE; return 1;
		
		
		default: return 0;
	};
}

void HandleMouseEvent(GLFWwindow* window, AppInput_t* currentInput, i32 glfwButtonCode, bool pressed)
{
	Buttons_t button = AppButtonForMouse(glfwButtonCode);
	
	if (button == Buttons_NumButtons) return; //No keymap
	
	ButtonState_t* buttonState = &currentInput->buttons[button];
	
	if (buttonState->isDown != pressed)
	{
		if (pressed)
		{
			//NOTE: If the key is being pressed then all we care about is the 
			//		first key pressed when isDown is false
			buttonState->transCount++;
			buttonState->isDown = true;
			
			if (button == MouseButton_Left)
			{
				currentInput->mouseStartPos[MouseButton_Left] = currentInput->mousePos;
				currentInput->mouseMaxDist[MouseButton_Left] = 0;
			}
			else if (button == MouseButton_Right)
			{
				currentInput->mouseStartPos[MouseButton_Right] = currentInput->mousePos;
				currentInput->mouseMaxDist[MouseButton_Right] = 0;
			}
			else if (button == MouseButton_Middle)
			{
				currentInput->mouseStartPos[MouseButton_Middle] = currentInput->mousePos;
				currentInput->mouseMaxDist[MouseButton_Middle] = 0;
			}
			
		}
		else
		{
			//NOTE: If the key is being released we need to check for multiple key mappings
			//		and only trigger when the last one is released
			bool foundPressedKey = false;
			i32 keys[4] = {};
			
			u8 numKeys = GetMouseBtnsForButton(button, keys);
			for (u8 keyIndex = 0; keyIndex < numKeys; keyIndex++)
			{
				if (keys[keyIndex] != glfwButtonCode && glfwGetMouseButton(window, keys[keyIndex]) == GLFW_PRESS)
				{
					Win32_WriteLine("Other mouse button held, ignoring button release");
					
					foundPressedKey = true;
					break;
				}
			}
			if (!foundPressedKey)
			{
				ClearArray(keys);
				numKeys = GetKeysForButton(button, keys);
				for (u8 keyIndex = 0; keyIndex < numKeys; keyIndex++)
				{
					if (glfwGetKey(window, keys[keyIndex]) == GLFW_PRESS)
					{
						Win32_WriteLine("Other key held, ignoring button release");
						
						foundPressedKey = true;
						break;
					}
				}
			}
			
			if (!foundPressedKey)
			{
				buttonState->transCount++;
				buttonState->isDown = false;
			}
		}
	}
}

void HandleKeyEvent(GLFWwindow* window, AppInput_t* currentInput, i32 glfwKeyCode, bool pressed)
{
	Buttons_t button = AppButtonForKey(glfwKeyCode);
	
	if (button == Buttons_NumButtons) return; //No keymap
	
	ButtonState_t* buttonState = &currentInput->buttons[button];
	
	if (buttonState->isDown != pressed)
	{
		if (pressed)
		{
			//NOTE: If the key is being pressed then all we care about is the 
			//		first key pressed when isDown is false
			buttonState->transCount++;
			buttonState->isDown = true;
		}
		else
		{
			//NOTE: If the key is being released we need to check for multiple key mappings
			//		and only trigger when the last one is released
			bool foundPressedKey = false;
			i32 keys[4] = {};
			
			u8 numKeys = GetMouseBtnsForButton(button, keys);
			for (u8 keyIndex = 0; keyIndex < numKeys; keyIndex++)
			{
				if (glfwGetMouseButton(window, keys[keyIndex]) == GLFW_PRESS)
				{
					Win32_WriteLine("Other mouse button held, ignoring key release");
					
					foundPressedKey = true;
					break;
				}
			}
			if (!foundPressedKey)
			{
				ClearArray(keys);
				numKeys = GetKeysForButton(button, keys);
				for (u8 keyIndex = 0; keyIndex < numKeys; keyIndex++)
				{
					if (keys[keyIndex] != glfwKeyCode && glfwGetKey(window, keys[keyIndex]) == GLFW_PRESS)
					{
						Win32_WriteLine("Other key held, ignoring key release");
						
						foundPressedKey = true;
						break;
					}
				}
			}
			
			if (!foundPressedKey)
			{
				buttonState->transCount++;
				buttonState->isDown = false;
			}
		}
	}
}
