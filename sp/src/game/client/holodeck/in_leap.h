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
class CLeapMotionListener : public GiantLeap::Listener
{
public:
					CLeapMotionListener();

	virtual void	onConnect( const GiantLeap::Controller &controller );
	virtual void	onFrame( const GiantLeap::Controller &controller );
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

					CLeapMotion();
					~CLeapMotion();

	GiantLeap::Init	_init;
	GiantLeap::Controller	_controller;
	CLeapMotionListener _listener;
	SFrameQueue *	_queue;

	static CLeapMotion *	_instance;
};

#endif // __IN_LEAP_H__