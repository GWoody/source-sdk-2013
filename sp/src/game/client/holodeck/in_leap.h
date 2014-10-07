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
	void			Push( const CFrame &frame );
	CFrame	Pop();
	CFrame 	Peek();
	bool			IsEmpty();

private:
	std::queue<CFrame>	_frameQueue;
	CThreadMutex	_mutex;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class CLeapMotionListener : public Leap::Listener
{
public:
					CLeapMotionListener();

	virtual void	onConnect( const Leap::Controller &controller );
	virtual void	onFrame( const Leap::Controller &controller );
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
	CFrame	BuildFinalFrame();

					CLeapMotion();
					~CLeapMotion();

	Leap::Controller	_controller;
	CLeapMotionListener _listener;
	SFrameQueue *	_queue;

	static CLeapMotion *	_instance;
};

#endif // __IN_LEAP_H__