/*
==============================================================================

	in_leap.h
	Defines the Frame Queue and Leap Interface. 

==============================================================================
*/

#ifndef __IN_LEAP_H__
#define __IN_LEAP_H__

#include <tier0/threadtools.h>

#include <Leap.h> 
#include <queue>
#include <string>
#include <sstream>

#include "holodeck/holo_shared.h"

// Pre definition.
class CLeapMotionListener;

//----------------------------------------------------------------------------
// This is thread safe.
//----------------------------------------------------------------------------
struct SFrameQueue
{
public:
	void			Push( const holo::CFrame &frame );
	holo::CFrame	Pop();
	holo::CFrame 	Peek();
	bool			IsEmpty();

private:
	std::queue<holo::CFrame>	_frameQueue;
	CThreadMutex	_mutex;
};


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class CLeapMotion
{
public:
	// Singleton methods.
	static CLeapMotion &	Get()		{ return *_instance; }
	static void		Create()			{ _instance = new CLeapMotion; }
	static void		Destroy()			{ delete _instance; }

	// Accessors.
	SFrameQueue &	GetQueue()			{ return *_queue; }

	void			CreateMove( CUserCmd *cmd );

private:
	// Frame processing.
	holo::CFrame	BuildFinalFrame();
	void			HandleWeapons( CUserCmd *cmd );

					CLeapMotion();
					~CLeapMotion();

	Leap::Controller	_controller;
	CLeapMotionListener *	_pListener;
	SFrameQueue *	_queue;

	static CLeapMotion *	_instance;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class CLeapMotionListener : public Leap::Listener
{
public:
					CLeapMotionListener( CLeapMotion *pLeap );

	virtual void	onConnect( const Leap::Controller &controller );
	virtual void	onFrame( const Leap::Controller &controller );

private:
	CLeapMotion *	_pLeap;
};


#endif // __IN_LEAP_H__