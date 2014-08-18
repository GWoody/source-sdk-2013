/*
===============================================================================

	grid_prediction.cpp
	Exposes the stub prediction system to the engine.

===============================================================================
*/

#include "cbase.h"
#include "prediction.h"
#include "gamemovement.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Expose interface to engine
static CPrediction g_Prediction;
CPrediction *prediction = &g_Prediction;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CPrediction, IPrediction, VCLIENT_PREDICTION_INTERFACE_VERSION, g_Prediction );

