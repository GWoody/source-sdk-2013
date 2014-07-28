/*
==============================================================================

	in_dinput.cpp
	Does Valves job for them. 

==============================================================================
*/

#include "cbase.h"
#include "in_dinput.h"
#include "in_buttons.h"
#include "c_basehlplayer.h"

#define DIRECTINPUT_VERSION	0x0800
#include "dinput_lite.h"

CDirectInput *CDirectInput::_instance;

// We declare these DirectInput variables here because we don't want to include
// <dinput.h> within "in_dinput.h".
// This would pollute the engine code with references to an API that it shouldn't
// have access to.
static LPDIRECTINPUT8		gDirectInput = NULL;
static LPDIRECTINPUTDEVICE8	gJoystick = NULL;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void format_message( HRESULT hr, const char **message )
{
	FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)message, 0, NULL);
}

void free_message( const char *message )
{
	LocalFree( (HLOCAL)message );
}

//----------------------------------------------------------------------------
// Attempts to connect to any devices detected by Direct Input.
//----------------------------------------------------------------------------
BOOL FAR PASCAL enumerate_devices( LPCDIDEVICEINSTANCE device, LPVOID pvRef )
{
	Warning( "HOLODECK: Attempting connection to %s\n", device->tszProductName );
	gDirectInput->CreateDevice( device->guidInstance, &gJoystick, NULL );
	return gJoystick ? DIENUM_STOP : DIENUM_CONTINUE;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CDirectInput::CDirectInput() :
	hl2_walkspeed( "hl2_walkspeed" ),
	hl2_normspeed( "hl2_normspeed" ),
	hl2_sprintspeed( "hl2_sprintspeed" )
{

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CDirectInput::~CDirectInput()
{

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool CDirectInput::Init()
{
	if( !CreateDirectInput() )
	{
		return false;
	}

	FindJoysticks();
	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool CDirectInput::CreateDirectInput()
{
	// Get the instance of this application (Source doesn't expose this).
	HINSTANCE hInstance = GetModuleHandle( NULL );

	HRESULT hr = DirectInput8Create( hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&gDirectInput, NULL );
	if( hr != DI_OK )
	{
		const char *message;
		format_message( hr, &message );
			Warning( "HOLODECK: DirectInput8Create failed (%s)\n", message );
		free_message( message );
		return false;
	}

	hr = gDirectInput->Initialize( hInstance, DIRECTINPUT_VERSION );
	if( hr != DI_OK )
	{
		const char *message;
		format_message( hr, &message );
			Warning( "HOLODECK: gDirectInput->Initialize failed (%s)\n", message );
		free_message( message );
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CDirectInput::FindJoysticks()
{
	if( gDirectInput )
	{
		gDirectInput->EnumDevices( DI8DEVCLASS_GAMECTRL, enumerate_devices, NULL, DIEDFL_ATTACHEDONLY );

		if( gJoystick )
		{
			gJoystick->SetCooperativeLevel( GetActiveWindow(), DISCL_EXCLUSIVE | DISCL_FOREGROUND );

			DIDEVICEINSTANCE di;
			memset( &di, 0, sizeof(di) );
			di.dwSize = sizeof(di);

			HRESULT hr = gJoystick->GetDeviceInfo( &di );
			if( hr == DI_OK )
			{
				Warning( "HOLODECK: Found joystick \"%s\"\n", di.tszProductName );
			}
			else
			{
				const char *message;
				format_message( hr, &message );
					Warning( "HOLODECK: gJoystick->GetDeviceInfo failed (%s)\n", message );
				free_message( message );
			}
		}
		else
		{
			Warning( "HOLODECK: Failed to find joystick\n" );
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CDirectInput::Shutdown()
{
	if( gJoystick )
	{
		gJoystick->Release();
	}

	if( gDirectInput )
	{
		gDirectInput->Release();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CDirectInput::CreateMove( CUserCmd *cmd )
{
	// Get the players client side entity singleton.
	C_BaseHLPlayer *player = dynamic_cast<C_BaseHLPlayer *>( C_BasePlayer::GetLocalPlayer() );

	// The players move speed is stored within a ConVar.
	const ConVarRef *movespeed = player->IsSprinting() ? &hl2_sprintspeed : &hl2_normspeed;
	const float maxPlayerVelocity = movespeed->GetFloat();

	// JAMESTODO: Poll the axis values.
	const float forwardAxis = 0.0f;
	const float sideAxis = 0.0f;

	cmd->forwardmove = forwardAxis * maxPlayerVelocity;
	cmd->sidemove = sideAxis * maxPlayerVelocity;

	// Do this last.
	FillBitFields( cmd );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CDirectInput::FillBitFields( CUserCmd *cmd )
{
	if( cmd->forwardmove > 0 )
	{
		cmd->buttons |= IN_FORWARD;
	}
	else
	{
		cmd->buttons |= IN_BACK;
	}

	if( cmd->sidemove < 0 )
	{
		cmd->buttons |= IN_RIGHT;
	}
	else
	{
		cmd->buttons |= IN_LEFT;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static ConVar holo_enable_dinput( "holo_enable_dinput", "0", FCVAR_ARCHIVE );

bool CDirectInput::Enabled()
{
	return holo_enable_dinput.GetBool();
}