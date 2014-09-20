/*
==============================================================================

	in_move.h
	Defines the interface to the PS Move headtracking system. 

==============================================================================
*/

#ifndef __IN_MOVE_H__
#define __IN_MOVE_H__

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CPlaystationMove
{
public:
	// Singleton methods.
	static CPlaystationMove &	Get()		{ return *_instance; }
	static void		Create()			{ _instance = new CPlaystationMove; }
	static void		Destroy()			{ delete _instance; }
	
private:
	CPlaystationMove();
	~CPlaystationMove();
	static CPlaystationMove *	_instance;
};

#endif // __IN_MOVE_H__