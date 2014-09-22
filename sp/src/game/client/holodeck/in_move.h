/*
==============================================================================

	in_move.h
	Defines the interface to the PS Move headtracking system. 

==============================================================================
*/

#ifndef __IN_MOVE_H__
#define __IN_MOVE_H__

typedef struct _PSMove PSMove;
typedef struct _PSMoveTracker PSMoveTracker;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CPlaystationMove
{
public:
	// Singleton methods.
	static CPlaystationMove &	Get()		{ return *_instance; }
	static void		Create()			{ _instance = new CPlaystationMove; }
	static void		Destroy()			{ delete _instance; }

	void			CreateMove( CUserCmd *cmd );

	void			StartTracker();
	
private:
	CPlaystationMove();
	~CPlaystationMove();
	static CPlaystationMove *	_instance;

	PSMove *		_move;
	PSMoveTracker *	_tracker;
	bool			_calibrated;
};

#endif // __IN_MOVE_H__