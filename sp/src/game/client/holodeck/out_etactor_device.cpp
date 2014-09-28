/*
==============================================================================

	out_etactor_device.cpp
	Implements the device queue for the haptic feedback system. 

==============================================================================
*/

#include "cbase.h"
#include "out_etactor_device.h"

#include <tenslib.h>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const int TENS_TIMEOUT_MS = 20;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template<class T>
CETactorVar<T>::CETactorVar()
{
#pragma omp atomic
	_val = 0;

#pragma omp atomic
	_dirty = true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template<class T>
void CETactorVar<T>::Set( const T &val )
{
#pragma omp atomic
	T localVal = _val;

	if( localVal == val )
	{
		return;
	}
		
#pragma omp atomic
	_val = val;

#pragma omp atomic
	_dirty = true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template<class T>
T CETactorVar<T>::Get() const
{
#pragma omp atomic
	T localVal = _val;
	return localVal;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template<class T>
bool CETactorVar<T>::IsDirty() const
{
#pragma omp atomic
	bool localDirty = _dirty;
	return localDirty;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template<class T>
void CETactorVar<T>::Clean()
{
#pragma omp atomic
	_dirty = false;
}

//=============================================================================
//=============================================================================

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CETactorDevice::CETactorDevice( etactorId_t id, CThread *owner )
{
#pragma omp atomic
	_id = id;

#pragma omp atomic
	_owner = owner;

	_initialized = false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CETactorDevice::Update( bool enabled, unsigned char power, unsigned char freq )
{
	short powfreq = ( power << 8 ) | freq;
	_powfreq.Set( powfreq );

	_enabled.Set( enabled );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CETactorDevice::Commit()
{
	if( !_initialized )
	{
		Init();
	}

	if( _enabled.IsDirty() || _powfreq.IsDirty() )
	{
#pragma omp atomic
		etactorId_t localId = _id;

		tens_settarget( localId );
		_owner->Sleep( TENS_TIMEOUT_MS );

		if( _enabled.IsDirty() )
		{
			bool enabled = _enabled.Get();

			tens_enable( enabled, enabled );
			_owner->Sleep( TENS_TIMEOUT_MS );

			_enabled.Clean();
		}

		if( _powfreq.IsDirty() )
		{
			unsigned short powfreq = _powfreq.Get();

			unsigned char power = ( powfreq >> 8 ) & 0xFF;
			unsigned char freq = powfreq & 0xFF;

			tens_control( power, freq );
			_owner->Sleep( TENS_TIMEOUT_MS );

			_powfreq.Clean();
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
etactorId_t CETactorDevice::GetId() const
{
	return _id;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CETactorDevice::Init()
{
#pragma omp atomic
	etactorId_t localId = _id;

	tens_settarget( localId );
	_owner->Sleep( TENS_TIMEOUT_MS );

	tens_enable( false, false );
	_owner->Sleep( TENS_TIMEOUT_MS );

	tens_control( 0, 0 );
	_owner->Sleep( TENS_TIMEOUT_MS );

	tens_chargerate( 7 );
	_owner->Sleep( TENS_TIMEOUT_MS );

	_initialized = true;
}