// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FramePackage.h"
#include "TimeRevComponent.generated.h"

// Only by adding Blueprintable can this component be used to make a ComponentBP
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class TIMEREVERSAL_API UTimeRevComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTimeRevComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION() // Make sure to use this for binding to work later
		void SetReversingTime(bool InReversingTime);
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Is player reversing time
	bool bReversingTime;

	// Out of time data, can't keep reversing if true
	bool bOutOfData;

	/* While reversing time we are going to add the real delta time to the RunningTime 
	 & add the delta time of the FramePackage to the ReverseRunningTime
	 While reversing things the current delta time i.e., the frame rate might not be equal to the frame rate
	 while we were recording forward time data. So if we reverse it directly the speed of the reversing will not be
	 equal to the speed or position etc. of the objects while they were moving forward. So we need these 2 varables.*/
	// Actual time passed since we started reversing time
	float RunningTime;
	// Running count of FramePackage delta times
	float LeftReverseRunningTime;
	float RightReverseRunningTime;

	// Sum of the reversed FramePackage delta time
	float ReverseRunningTime;

	// Total amount of time recorded in FramePackages
	float RecordedTime;

	TDoubleLinkedList<FFramePackage> StoredFrames;

	void SetActorVariables(FVector Location, FRotator Rotation, FVector LinearVelocity, FVector AngularVelocity);
};
