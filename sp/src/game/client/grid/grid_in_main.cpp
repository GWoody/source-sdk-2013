/*
===============================================================================

	grid_in_main.cpp
	Grid specific input handling.

===============================================================================
*/

#include "cbase.h"
#include "kbutton.h"
#include "input.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridInput : public CInput
{
public:
};

static CGridInput g_Input;

// Expose this interface
IInput *input = &g_Input;
