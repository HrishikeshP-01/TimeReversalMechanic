// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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

	bool bReversingTime;
};
