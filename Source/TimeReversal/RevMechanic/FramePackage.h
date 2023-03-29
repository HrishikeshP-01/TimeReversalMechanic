// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Math/Vector.h"
#include "Math/Rotator.h"
#include "Math/Vector2D.h"

/**
 * 
 */
struct FFramePackage
{
	/*forceinline means that the code for the function would directly be added to where this function is called by the compiler during compilation.
	This means that during execution the flow doesn't have to jump to this class to implement the function code as the code was aldready added during compilation.*/
	FORCEINLINE FFramePackage();
	FORCEINLINE FFramePackage(FVector InLocation, FRotator InRotation, FVector InLinearVelocity, FVector InAngularVelocity, float InDeltaTime);

	FVector Location;
	FRotator Rotation;
	FVector LinearVelocity;
	FVector AngularVelocity;

	/*If we want to implement reversal for our player too. As it's a first person character we only need the above attributes along with where the camera looks each frame*/
	bool bIsPlayerPawn;
	FVector2D MouseXY;

	float DeltaTime;
};

FFramePackage::FFramePackage()
{

}

FFramePackage::FFramePackage(FVector InLocation, FRotator InRotation, FVector InLinearVelocity, FVector InAngularVelocity, float InDeltaTime) :
	Location(InLocation), Rotation(InRotation), LinearVelocity(InLinearVelocity), AngularVelocity(InAngularVelocity), DeltaTime(InDeltaTime)
{
	/* The above line means that I'm assigning the input parameters to the corresponding variables.
	* This is the equivalent of writing: Location = InLocation; etc. inside the body.
	* This way we can do the assignment in 1 line
	*/
}