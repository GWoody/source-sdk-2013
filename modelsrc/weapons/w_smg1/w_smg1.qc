/*
==============================================================================

Sub machinegun

==============================================================================
*/


$modelname weapons/w_smg1.mdl

$cdmaterials models/weapons/w_smg1

$origin 0 -12 3
$body studio "smg1_reference.smd"
$surfaceprop "weapon"

$mostlyopaque

//Muzzle flash
$attachment "muzzle" "valvebiped.muzzle" 0.5 0.1 0 RIGID

//Shell eject
$attachment 1 "valvebiped.w_smg1attach" -1.5 1.5 0.0 rotate -90 0 0 RIGID

$attachment "ammo" "ValveBiped.Weapon_bone" -7.00 -1.00 1.00 rotate 4 90 -87

$sequence idle "Idle" fps 30 loop LX rotate 180 activity ACT_VM_IDLE 1

$sequence attack1 "Idle" fps 20 frames 0 20 LX rotate 180 activity ACT_RANGE_ATTACK_SMG1 1 { 
	{ event AE_NPC_MUZZLEFLASH 0 "SMG1 MUZZLE" } { event 3002 0 } { event 6001 0 "0" } 
}

// E3 HACK: This fires the bullets from the NPC's "lefthand" attachment point.
//			Do we need a better solution for shooting from a far off-center scripted sequence?
$sequence attackoffcenter "Idle" fps 20 frames 0 20 LX rotate 180 { 
	{ event AE_NPC_MUZZLEFLASH 0 "SMG1 MUZZLE" } { event 3002 0 "lefthand" } { event 6001 0 "0" } 
}

$lod 20
{
	replacemodel "smg1_reference" "smg1_lod1"
}

$collisionmodel "smg1_physbox.smd" {
	$mass 8.0
}

