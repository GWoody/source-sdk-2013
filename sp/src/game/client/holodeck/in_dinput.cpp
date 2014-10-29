/*
==============================================================================

	in_dinput.cpp
	Does Valves job for them. 

==============================================================================
*/

#include "cbase.h"

#define DIRECTINPUT_VERSION	0x0800
#define S_OK 0x00000000

#include "dinput_lite.h"

#include "in_dinput.h"
#include "in_buttons.h"

CDirectInput *CDirectInput::_instance;

// We declare these DirectInput variables here because we don't want to include
// <dinput.h> within "in_dinput.h".
// This would pollute the engine code with references to an API that it shouldn't
// have access to.
LPDIRECTINPUT8		gDirectInput = NULL;
LPDIRECTINPUTDEVICE8	gJoystick = NULL;

#define JOY_MAX		32767
static ConVar holo_joy_forward_sensitivity( "holo_joy_forward_sensitivity", "1.0", FCVAR_ARCHIVE );
static ConVar holo_joy_side_sensitivity( "holo_joy_side_sensitivity", "1.0", FCVAR_ARCHIVE );
static ConVar holo_joy_yaw_sensitivity( "holo_joy_yaw_sensitivity", "1.0", FCVAR_ARCHIVE );
static ConVar holo_joy_deadzone_frac( "holo_joy_deadzone_frac", "0.2", FCVAR_ARCHIVE );

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
//----------------------------------------------------------------------------
CDirectInput::CDirectInput() :
	cl_forwardspeed( "cl_forwardspeed" )
{

}

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
CDirectInput::~CDirectInput()
{
}

//----------------------------------------------------------------------------
// Init method
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
			ConColorMsg( COLOR_YELLOW, __FUNCTION__": DirectInput8Create failed (%s)\n", message );
		free_message( message );
		return false;
	}

	hr = gDirectInput->Initialize( hInstance, DIRECTINPUT_VERSION );
	if( hr != DI_OK )
	{
		const char *message;
		format_message( hr, &message );
			ConColorMsg( COLOR_YELLOW, __FUNCTION__": gDirectInput->Initialize failed (%s)\n", message );
		free_message( message );
		return false;
	}

	return true;
}


//-----------------------------------------------------------------------------
// TAKEN FROM DIRECTX SAMPLE (2010, FEB)
// Name: EnumObjectsCallback()
// Desc: Callback function for enumerating objects (axes, buttons, POVs) on a 
//       Joystick. This function enables user interface elements for objects
//       that are found to exist, and scales axes min/max values.
//-----------------------------------------------------------------------------
BOOL CALLBACK  EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
{
    static int nSliderCount = 0;  // Number of returned slider controls
    static int nPOVCount = 0;     // Number of returned POV controls

    // For axes that are returned, set the DIPROP_RANGE property for the
    // enumerated axis in order to scale min/max values.
    if( pdidoi->dwType & DIDFT_AXIS )
    {
        DIPROPRANGE diprg;
        diprg.diph.dwSize = sizeof( DIPROPRANGE );
        diprg.diph.dwHeaderSize = sizeof( DIPROPHEADER );
        diprg.diph.dwHow = DIPH_BYID;
        diprg.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
        diprg.lMin = -JOY_MAX;
        diprg.lMax = +JOY_MAX;

        // Set the range for the axis
        if( FAILED( gJoystick->SetProperty( DIPROP_RANGE, &diprg.diph)))
            return DIENUM_STOP;

	}

    return DIENUM_CONTINUE;
}

//----------------------------------------------------------------------------
// Attempts to connect to any devices detected by Direct Input.
//----------------------------------------------------------------------------
BOOL CALLBACK enumerate_devices(const DIDEVICEINSTANCE *device, LPVOID pvRef )
{
	ConColorMsg( COLOR_GREEN, "Connecting to %s\n", device->tszProductName );

	HRESULT deviceCreated;
	deviceCreated = gDirectInput->CreateDevice( device->guidInstance, &gJoystick, NULL );
	if( FAILED (deviceCreated) ){
		ConColorMsg( COLOR_YELLOW, __FUNCTION__": Connection failed!\n");
		return DIENUM_CONTINUE;
	} else {
		return DIENUM_STOP; // stop enumming
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CDirectInput::FindJoysticks()
{
	if( gDirectInput )
	{

		/*DI_ENUM_CONTEXT enumContext;
		enumContext.pPreferredJoyCfg = &PreferredJoyCfg;
		enumContext.bPreferredJoyCfgValid = false;
		*/
		gDirectInput->EnumDevices( DI8DEVCLASS_GAMECTRL, enumerate_devices, NULL, DIEDFL_ATTACHEDONLY );

		if( gJoystick && gJoystick->Acquire() )
		{			
			DIDEVICEINSTANCE di;
			memset( &di, 0, sizeof(di) );
			di.dwSize = sizeof(di);

			HRESULT hr = gJoystick->GetDeviceInfo( &di );
			if( hr == DI_OK )
			{
				if( FAILED(gJoystick->SetDataFormat( &c_dfDIJoystick2 ) ) )
					ConColorMsg( COLOR_YELLOW, __FUNCTION__": Failed to set data format!\n");

				if( FAILED(gJoystick->SetCooperativeLevel( GetActiveWindow(), DISCL_EXCLUSIVE | DISCL_FOREGROUND ) ) )
					ConColorMsg( COLOR_YELLOW, __FUNCTION__": Failed to set cooperative level\n");

				//HWND hDlg = ( HWND )pContext;

				HRESULT h2;
				h2 = gJoystick->EnumObjects(EnumObjectsCallback, NULL, DIDFT_ALL);
			}
			else
			{
				const char *message;
				format_message( hr, &message );
					ConColorMsg( COLOR_YELLOW, __FUNCTION__": gJoystick->GetDeviceInfo failed (%s)\n", message );
				free_message( message );
			}
		}
		else
		{
			ConColorMsg( COLOR_YELLOW, __FUNCTION__": Failed to find joystick\n" );
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
	// The players move speed is stored within a ConVar.
	const float maxPlayerVelocity = cl_forwardspeed.GetFloat();

	/** The Polling System **/
	HRESULT hr;
	DIJOYSTATE2 js;

	if( gJoystick == NULL )
	{
		return;
	}

	hr = gJoystick->Poll();
	if( FAILED( hr ) )
    {
		// DInput is telling us that the input stream has been
		// interrupted. We aren't tracking any state between polls, so
		// we don't have any special reset that needs to be done. We
		// just re-acquire and try again.
		hr = gJoystick->Acquire();
		int loops = 0;
		const int MAX_LOOPS = 5;
		while( hr != DI_OK && loops < MAX_LOOPS )
		{
			hr = gJoystick->Acquire();
			loops++;
		}

		// If we encounter a fatal error, return failure.
		if ( loops == MAX_LOOPS || hr == DIERR_INVALIDPARAM || hr == DIERR_NOTINITIALIZED ) 
		{
			return;
		}

		// hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
		// may occur when the app is minimized or in the process of 
		// switching, so just try again later 
		//turn;
	}

	// Get the input's device state
    hr = gJoystick->GetDeviceState( sizeof( DIJOYSTATE2 ), &js );

	// JAMESTODO: Poll the axis values.
	long lForwardAxis = js.lY*-1;
	long lSideAxis = js.lX;
	long lYaw = js.lRz;

	for( int i = 0; i < 128; i++ )
	{
		if( js.rgbButtons[i] )
		{
			engine->ClientCmd_Unrestricted( "vr_reset_home_pos" );
			break;
		}
	}
	
	// Scale from the range [-JOY_MAX, JOY_MAX] to [-1, 1] so we can use the axis values as
	// movement speed scalars.
	float fForwardAxis = lForwardAxis / (float)JOY_MAX;
	float fSideAxis = lSideAxis / (float)JOY_MAX;
	
	if( abs( lForwardAxis ) > holo_joy_deadzone_frac.GetFloat() * JOY_MAX )
	{
		cmd->forwardmove = fForwardAxis * maxPlayerVelocity * holo_joy_forward_sensitivity.GetFloat();
	}

	if( abs( lSideAxis ) > holo_joy_deadzone_frac.GetFloat() * JOY_MAX )
	{
		cmd->sidemove = fSideAxis * maxPlayerVelocity * holo_joy_side_sensitivity.GetFloat();
	}

	if( abs( lYaw ) > holo_joy_deadzone_frac.GetFloat() * JOY_MAX )
	{
		cmd->viewangles[YAW] -= ( lYaw / (float)JOY_MAX ) * holo_joy_yaw_sensitivity.GetFloat();;
		engine->SetViewAngles( cmd->viewangles );
	}

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
//static ConVar holo_enable_dinput( "holo_enable_dinput", "0", FCVAR_ARCHIVE );

bool CDirectInput::Enabled()
{
	// Change to false to use the keyboard.
	// THIS IS A HACK!
	return true;
}