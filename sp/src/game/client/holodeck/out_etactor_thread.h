/*
==============================================================================

	out_etactor_thread.h
	Defines the device queue for the haptic feedback system. 

==============================================================================
*/

#ifndef __OUT_ETACTOR_THREAD_H__
#define __OUT_ETACTOR_THREAD_H__

typedef unsigned char etactorId_t;
extern const int TENS_TIMEOUT_MS;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template<typename T>
class CETactorVar
{
public:
	CETactorVar();

	void		Set( const T &val );
	T			Get() const;

	bool		IsDirty() const;
	void		Clean();

private:
	T			_val;
	bool		_dirty;
};

//----------------------------------------------------------------------------
// The state of a single device.
//----------------------------------------------------------------------------
class CETactorState
{
public:
	CETactorState( etactorId_t id, CThread *owner );

	void		Update( bool enabled, unsigned char power, unsigned char freq );
	void		Commit();

	etactorId_t	GetId() const;

private:
	void		Init();

	etactorId_t	_id;
	CThread *	_owner;

	CETactorVar<short>	_powfreq;
	CETactorVar<bool>	_enabled;

	bool		_initialized;
};

#endif // __OUT_ETACTOR_THREAD_H__