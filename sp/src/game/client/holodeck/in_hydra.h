/*
==============================================================================

	in_hydra.h
	Defines the Hydra head tracking interface. 

==============================================================================
*/

#ifndef __IN_HYDRA_H__
#define __IN_HYDRA_H__

class CUserCmd;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class IRazerHydra
{
public:
	virtual ~IRazerHydra() {}

	// Singleton methods.
	static IRazerHydra &	Get()		{ return *_instance; }
	static void		Create();
	static void		Destroy()			{ delete _instance; }

	virtual void	CreateMove( CUserCmd *cmd ) = 0;

protected:
	IRazerHydra() {}

private:
	static IRazerHydra *	_instance;
};

#endif // __IN_HYDRA_H__