#include <Precompiled.h>

#include "Win32Input.h"

using namespace Win32;

Win32::Win32Input::Win32Input()
{
	ZeroMemory(&m_currentKeyState, sizeof(m_currentKeyState));
	ZeroMemory(&m_currentMouseState, sizeof(m_currentMouseState));
	ZeroMemory(&m_oldKeyState, sizeof(m_oldKeyState));
	ZeroMemory(&m_oldMouseState, sizeof(m_oldMouseState));
}

Win32::Win32Input::~Win32Input()
{

}

void Win32::Win32Input::carry_current_keystate()
{
	m_oldKeyState = m_currentKeyState;
}

void Win32::Win32Input::carry_current_mouse_state()
{
	m_oldMouseState = m_currentMouseState;
}

KeyState Win32::Win32Input::get_keyboard_key_state(BOOL current, BYTE virtualKey)
{
	if (current > 0)
	{
		return m_currentKeyState.virtualKey[virtualKey];
	}
	else
	{
		return m_oldKeyState.virtualKey[virtualKey];
	}
}

void Win32::Win32Input::set_keyboard_key_state(BYTE virtualKey, KeyState keyState)
{
	m_currentKeyState.virtualKey[virtualKey] = keyState;
}

ButtonState Win32::Win32Input::get_mouse_button_state(BOOL current, MouseButton mouseButton)
{
	if (current > 0)
	{
		return m_currentMouseState.virtualMouseKey[mouseButton];
	}
	else
	{
		return m_oldMouseState.virtualMouseKey[mouseButton];
	}
}

void Win32::Win32Input::set_mouse_button_state(MouseButton mouseButton, ButtonState mouseButtonState)
{
	m_currentMouseState.virtualMouseKey[mouseButton] = mouseButtonState;
}

void Win32::Win32Input::set_mouse_delta(float x, float y)
{
	m_currentMouseState.cursorDeltaX = x;
	m_currentMouseState.cursorDeltaY = y;
}

float Win32::Win32Input::get_mouse_delta_x() const
{
	return m_currentMouseState.cursorDeltaX;;
}

float Win32::Win32Input::get_mouse_delta_y() const
{
	return m_currentMouseState.cursorDeltaY;
}
