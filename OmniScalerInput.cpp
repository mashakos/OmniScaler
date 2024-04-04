////////////////////////////////////////////////////////////////////////////////
// Filename: OmniScalerInput.cpp
////////////////////////////////////////////////////////////////////////////////
#include "OmniScalerInput.h"


OmniScalerInput::OmniScalerInput()
{
}


OmniScalerInput::OmniScalerInput(const OmniScalerInput& other)
{
}


OmniScalerInput::~OmniScalerInput()
{
}


void OmniScalerInput::Initialize()
{
	int i;


	// Initialize all the keys to being released and not pressed.
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}

	return;
}


void OmniScalerInput::KeyDown(unsigned int input)
{
	// If a key is pressed then save that state in the key array.
	m_keys[input] = true;
	return;
}


void OmniScalerInput::KeyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	m_keys[input] = false;
	return;
}


bool OmniScalerInput::IsKeyDown(unsigned int key)
{
	// Return what state the key is in (pressed/not pressed).
	return m_keys[key];
}