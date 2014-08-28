/*
==============================================================================

	in_dinput.h
	Does Valves job for them. 

==============================================================================
*/

#ifndef __IN_DINPUT_H__
#define __IN_DINPUT_H__

/*
struct DI_ENUM_CONTEXT
{
    DIJOYCONFIG* pPreferredJoyCfg;
    bool bPreferredJoyCfgValid;
};
DIJOYCONFIG PreferredJoyCfg = {0};*/

// Pre definition.
class CUserCmd;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class CDirectInput
{
public:
	// Singleton methods.
	static CDirectInput &	Get()		{ return *_instance; }
	static void		Create()			{ _instance = new CDirectInput; }
	static void		Destroy()			{ delete _instance; }

	bool			Init();
	void			Shutdown();
	void			CreateMove( CUserCmd *cmd );

	static bool		Enabled();

private:
	// Singleton methods.
					CDirectInput();
					~CDirectInput();

	// Initialisation.
	bool			CreateDirectInput();
	void			FindJoysticks();

	// CreateMove methods.
	void			FillBitFields( CUserCmd *cmd );

	// Singleton.
	static CDirectInput *	_instance;

	// Server side convars.
	const ConVarRef	cl_forwardspeed;
};

#endif // __IN_DINPUT_H__