/*
==============================================================================

	out_etactor.h
	Defines the haptic feedback interface. 

==============================================================================
*/

#ifndef __OUT_ETACTOR_H__
#define __OUT_ETACTOR_H__

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class IETactor
{
public:
	virtual ~IETactor() {}

	// Singleton methods.
	static IETactor &	Get()		{ return *_instance; }
	static void		Create();
	static void		Destroy()		{ delete _instance; }

	virtual void	SetPower( unsigned char power ) = 0;
	virtual void	SetFrequency( unsigned char freq ) = 0;
	virtual void	SetEnabled( bool enabled ) = 0;
	virtual void	SetTarget( unsigned char target ) = 0;
	
protected:
	IETactor() {}

private:
	static IETactor *	_instance;
};

#endif // __OUT_ETACTOR_H__