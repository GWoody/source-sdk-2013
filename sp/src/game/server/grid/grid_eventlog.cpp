/*
===============================================================================

	grid_eventlog.cpp
	Instanciates the event log object.

===============================================================================
*/

#include "cbase.h"
#include "../EventLog.h"
#include "KeyValues.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridEventLog : public CEventLog
{
private:
	typedef CEventLog BaseClass;

public:
	virtual char const *Name() { return "CGridEventLog"; }

	virtual ~CGridEventLog() {};
};

static CGridEventLog s_EventLog;

//-----------------------------------------------------------------------------
// Singleton access
//-----------------------------------------------------------------------------
IGameSystem* GameLogSystem()
{
	return &s_EventLog;
}