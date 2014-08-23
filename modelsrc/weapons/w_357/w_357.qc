/*
==============================================================================

World model pistol

==============================================================================
*/

$modelname weapons/W_357.mdl
$cdmaterials models\weapons\w_357

// whole body
$body studio "w_357_reference.smd"

//Muzzle flash
$attachment 0 "MAXSceneRoot.Cylinder01" -3.00 0.00 -9.00 RIGID
//Shell eject
$attachment 1 "MAXSceneRoot.Cylinder01" 0 0 0 RIGID

$sequence idle "w_357_reference" FPS 30 loop activity ACT_VM_IDLE 1

$sequence attack1 "w_357_reference" FPS 30 activity ACT_RANGE_ATTACK_PISTOL 1 { 
{ event 5003 0 "2" }
}

//$sequence attack1 "Idle" fps 20 frames 0 20 LX rotate 180 


/*
==============================================================================

Animation descriptions



==============================================================================
*/
$surfaceprop "weapon"

$collisionmodel "w_357_physbox.smd" {
	$mass 2.0
}
