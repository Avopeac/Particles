#pragma once

namespace Win32 
{
	#define WIN32_VK_0 0x30
	#define WIN32_VK_1 0x31
	#define WIN32_VK_2 0x32
	#define WIN32_VK_3 0x33
	#define WIN32_VK_4 0x34
	#define WIN32_VK_5 0x35
	#define WIN32_VK_6 0x36
	#define WIN32_VK_7 0x37
	#define WIN32_VK_8 0x38
	#define WIN32_VK_9 0x39

	#define WIN32_VK_A 0x41
	#define WIN32_VK_B 0x42
	#define WIN32_VK_C 0x43
	#define WIN32_VK_D 0x44
	#define WIN32_VK_E 0x45
	#define WIN32_VK_F 0x46
	#define WIN32_VK_G 0x47
	#define WIN32_VK_H 0x48
	#define WIN32_VK_I 0x49
	#define WIN32_VK_J 0x4A
	#define WIN32_VK_K 0x4B
	#define WIN32_VK_L 0x4C
	#define WIN32_VK_M 0x4D
	#define WIN32_VK_N 0x4E
	#define WIN32_VK_O 0x4F
	#define WIN32_VK_P 0x50
	#define WIN32_VK_Q 0x51
	#define WIN32_VK_R 0x52
	#define WIN32_VK_S 0x53
	#define WIN32_VK_T 0x54
	#define WIN32_VK_U 0x55
	#define WIN32_VK_V 0x56
	#define WIN32_VK_W 0x57
	#define WIN32_VK_X 0x58
	#define WIN32_VK_Y 0x59
	#define WIN32_VK_Z 0x5A

	enum MouseButton
	{
		LEFT_BUTTON,
		RIGHT_BUTTON,
		MIDDLE_BUTTON,
	};

	enum ButtonState
	{
		BUTTON_UP,
		BUTTON_DOWN,
	};

	enum KeyState 
	{
		KEY_UP,
		KEY_DOWN,
	};
	
	struct Win32MouseState 
	{
		float cursorDeltaX;
		float cursorDeltaY;
		union 
		{
			ButtonState virtualMouseKey[3];
			struct 
			{
				ButtonState left;
				ButtonState right;
				ButtonState middle;
			};
		};
	};

	// NOTE: All VKs can fit in two bytes (0xFF)
	struct Win32KeyState 
	{
		KeyState virtualKey[255];
	};

	// Basic input functions for Win32 that gets both mouse and keyboard. Contains virtual key codes for Win32 since queries for certain keys will be made.
	// Author: Andreas Larsson
	// Date: 2016-12-12 (YYYY-MM-DD)
	class Win32Input 
	{
		Win32KeyState m_oldKeyState;
		Win32KeyState m_currentKeyState;
		Win32MouseState m_oldMouseState;
		Win32MouseState m_currentMouseState;

	public:
		Win32Input();
		~Win32Input();
		Win32Input(const Win32Input &) = delete;
		Win32Input(Win32Input &&) = delete;
		Win32Input &operator=(const Win32Input &) = delete;
		Win32Input &operator=(Win32Input &&) = delete;
		
		// NOTE: Copies current key state to previous key state
		void carry_current_keystate();

		// NOTE: Copies current mouse state to previous mouse state
		void carry_current_mouse_state();

		// NOTE: Get the keyboard state for a given virtual key, either present or past
		KeyState get_keyboard_key_state(BOOL current, BYTE virtualKey);

		// NOTE: Get the mouse state for a given virtual key, either present or past
		ButtonState get_mouse_button_state(BOOL current, MouseButton mouseButton);

		// NOTE: Set the keyboard state for a given virtual key
		void set_keyboard_key_state(BYTE virtualKey, KeyState keyState);

		// NOTE: Get the mouse state for a given virtual key
		void set_mouse_button_state(MouseButton mouseButton, ButtonState mouseButtonState);

		// NOTE: Set the current mouse delta movement
		void set_mouse_delta(float x, float y);

		// NOTE: Get the mouse delta horizontally
		float get_mouse_delta_x() const;

		// NOTE: Get the mouse delta vertically
		float get_mouse_delta_y() const;

	private:
		
	};
}