// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeRevComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimeReversal/TimeReversalCharacter.h"
#include "FramePackage.h"
#include "Components/StaticMeshComponent.h"

// Sets default values for this component's properties
UTimeRevComponent::UTimeRevComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bReversingTime = false;

	bOutOfData = true;
	RecordedTime = 0.0f;
}


// Called when the game starts
void UTimeRevComponent::BeginPlay()
{
	Super::BeginPlay();

	ATimeReversalCharacter* Player = Cast<ATimeReversalCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	Player->RevTimeDelegate.AddDynamic(this, &UTimeRevComponent::SetReversingTime);
}


// Called every frame
void UTimeRevComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bReversingTime) // Recording time data
	{
		while (RecordedTime >= 15.0f)
		{
			auto Head = StoredFrames.GetHead();
			float HeadDT = Head->GetValue().DeltaTime;
			StoredFrames.RemoveNode(Head);
			RecordedTime -= HeadDT;
		}

		AActor* Owner = GetOwner();
		TArray<UActorComponent*> Components = Owner->GetComponentsByClass(UStaticMeshComponent::StaticClass());
		if (Components.Num() > 0)
		{
			UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(Components[0]);
			if (SMC)
			{
				FFramePackage Package(Owner->GetActorLocation(),
					Owner->GetActorRotation(),
					SMC->GetPhysicsLinearVelocity(),
					SMC->GetPhysicsAngularVelocityInDegrees(),
					DeltaTime);

				StoredFrames.AddTail(Package);
				RecordedTime += DeltaTime;
				bOutOfData = false;
			}
		}
	}
	else if (!bOutOfData) // Reversing time
	{

	}
}

void UTimeRevComponent::SetReversingTime(bool InReversingTime)
{
	bReversingTime = InReversingTime;

	if (bReversingTime) { UE_LOG(LogTemp, Log, TEXT("Reversing Time")); }
	else { UE_LOG(LogTemp, Log, TEXT("Stopped Reversing Time")); }
}