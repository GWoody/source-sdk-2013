/*
==============================================================================

w_shotgun

==============================================================================
*/

$modelname weapons/p_IRifle.mdl

$cdmaterials models/weapons/w_Irifle

// whole body
$origin 0 -24 5
$body studio "W_IRifle_reference.smd"

//Muzzle flash
$attachment "muzzle" "Valvebiped.muzzle" -8.5 0 0 rotate 0 0 -90

//Shell eject
$attachment "shell" "Valvebiped.w_shotgun" 1.0 -7.0 3

$attachment "ammo" "ValveBiped.w_shotgun" -6.80 -6.50 -4.60 rotate -5 -5 -3

$sequence idle1	"Idle" loop fps 30 activity ACT_VM_IDLE 1

$sequence fire	"Idle" fps 30 activity ACT_RANGE_ATTACK_AR2 1 {
{ event AE_NPC_MUZZLEFLASH 0 "COMBINE MUZZLE" } { event 3007 0 "1" }
}

$surfaceprop "weapon"
$collisionmodel "W_Irifle_physbox.smd" {
	// Mass in kilograms
	$mass 20.0
}


$lod 10
{
     replacemodel "w_IRifle_reference" "w_IRifle_lod1"
}

$lod 15
{
     replacemodel "w_IRifle_reference" "w_IRifle_lod2"
}

$lod 20
{
     replacemodel "w_IRifle_reference" "w_IRifle_lod3"
}

$lod 25
{
     replacemodel "w_IRifle_reference" "w_IRifle_lod4"
}
