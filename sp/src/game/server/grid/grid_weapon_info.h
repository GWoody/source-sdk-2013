/*
===============================================================================

	grid_weapon_info.h
	Defines the Grid class that loads weapon scripts.

===============================================================================
*/

#ifndef __GRID_WEAPON_PARSER_H__
#define __GRID_WEAPON_PARSER_H__

#include "string_t.h"

class KeyValues;

namespace grid
{
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CBlock
	{
	public:
		virtual void	Parse( KeyValues *kv ) = 0;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CBulletInfo : public CBlock
	{
	public:
						CBulletInfo();
		virtual void	Parse( KeyValues *kv );

		// Accessors.
		int				GetCount() const				{ return _count; }
		int				GetDamage() const				{ return _damage; }
		int				GetAmmoType() const				{ return _ammotype; }
		int				GetMaxDistance() const			{ return _maxDistance; }

	private:
		int				_count;
		int				_damage;
		int				_ammotype;
		int				_maxDistance;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CShootInfo : public CBlock
	{
	public:
						CShootInfo();
		virtual void	Parse( KeyValues *kv );

		// Accessors.
		bool			IsSemiAuto() const				{ return _semiauto; }
		float			GetRate() const					{ return _rate; }

	private:
		bool			_semiauto;
		float			_rate;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CEffectInfo : public CBlock
	{
	public:
						CEffectInfo();
		virtual void	Parse( KeyValues *kv );

		bool			DoesEjectShells() const			{ return _ejectShell; }
		const char *	GetMuzzleParticleName() const	{ return STRING( _muzzleparticle ); }
		int				GetMuzzleAttachment() const		{ return _muzzleattachment; }

	private:
		bool			_ejectShell;
		string_t		_muzzleparticle;
		int				_muzzleattachment;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CModelInfo : public CBlock
	{
	public:
						CModelInfo();
		virtual void	Parse( KeyValues *kv );

		// Accessors.
		const QAngle &	GetAngle() const				{ return _angle; }
		const char *	GetWeapon() const				{ return STRING( _weapon ); }
		const char *	GetShell() const				{ return STRING( _shell ); }

	private:
		QAngle			_angle;
		string_t		_weapon;
		string_t		_shell;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CHudInfo : public CBlock
	{
	public:
						CHudInfo();
		virtual void	Parse( KeyValues *kv );

		// Accessors.
		const char *	GetFont() const					{ return STRING( _font ); }
		char			GetCharacter() const			{ return _character; }

	private:
		string_t		_font;
		char			_character;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CSoundInfo : public CBlock
	{
	public:
						CSoundInfo();
		virtual void	Parse( KeyValues *kv );

		// Accessors.
		const char *	GetFire() const					{ return STRING( _shoot ); }
		const char *	GetEmpty() const				{ return STRING( _empty ); }

	private:
		string_t		_shoot;
		string_t		_empty;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CHapticInfo : public CBlock
	{
	public:
						CHapticInfo();
		virtual void	Parse( KeyValues *kv );

		// Accessors.
		int				GetPower() const				{ return _power; }
		int				GetFrequency() const			{ return _freq; }
		float			GetDuration() const				{ return _duration; }

	private:
		int				_power;
		int				_freq;
		float			_duration;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CWeaponInfo
	{
	public:
						CWeaponInfo();
		
		virtual void	Parse( const char *scriptpath );

		CBulletInfo &	GetBullet()						{ return _bullet; }
		CShootInfo &	GetShoot()						{ return _shoot; }
		CEffectInfo &	GetEffect()						{ return _effect; }
		CModelInfo &	GetModel()						{ return _model; }
		CHudInfo &		GetHud()						{ return _hud; }
		CSoundInfo &	GetSound()						{ return _sound; }
		CHapticInfo &	GetHaptic()						{ return _haptic; }

	private:
		CBulletInfo		_bullet;
		CShootInfo		_shoot;
		CEffectInfo		_effect;
		CModelInfo		_model;
		CHudInfo		_hud;
		CSoundInfo		_sound;
		CHapticInfo		_haptic;
	};
}

#endif // __GRID_WEAPON_PARSER_H__