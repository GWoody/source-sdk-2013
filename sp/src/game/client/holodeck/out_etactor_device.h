/*
==============================================================================

	out_etactor_device.h
	Defines the device queue for the haptic feedback system. 

==============================================================================
*/

#ifndef __OUT_ETACTOR_DEVICE_H__
#define __OUT_ETACTOR_DEVICE_H__

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
	int			_dirtyCount;
};

//----------------------------------------------------------------------------
// The state of a single device.
//----------------------------------------------------------------------------
class CETactorDevice
{
public:
	CETactorDevice( etactorId_t id, CThread *owner );

	void		Update( bool enabled, unsigned char power, unsigned char freq );
	void		Commit();

	etactorId_t	GetId() const;

private:
	void		Init();

	etactorId_t	_id;
	CThread *	_owner;

	// State.
	CETactorVar<short>	_powfreq;
	CETactorVar<bool>	_enabled;

	bool		_initialized;
};

#endif // __OUT_ETACTOR_DEVICE_H__