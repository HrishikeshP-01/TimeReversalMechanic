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
		/* First we ensure that the recorded time doesn't exceed 15s.
		 So we remove the head FramePackage until the time is below 15s*/
		while (RecordedTime >= 15.0f)
		{
			auto Head = StoredFrames.GetHead();
			float HeadDT = Head->GetValue().DeltaTime;
			StoredFrames.RemoveNode(Head);
			RecordedTime -= HeadDT;
		}

		/* In this example I am only going to store the data of Static mesh components inside actors.
		 I also assume that each actor only has 1 static mesh component
		 & that static mesh component is also the root which is why I use the owner's location & rotation
		 I need to cast to static mesh component so as to get the linear & angular velocity.
		 
		 Optimization - This can be optimized so that we do the casting call etc. only at the begin play & store the SMC/Owner variable
		 so that we don't have to keep casting every frame */
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
				// Once we add a FramePackage we will have some data so bOutOfData will be false
				bOutOfData = false;
			}
		}
	}
	else if (!bOutOfData) // Reversing time
	{
		/* First we are going to implement reversal under the assumption that frame rate at the time of recording time data
		is same as the frame rate when we are reversing so we just have to use the frame packages in reverse order.
		This will give us an idea of how our logic works & then we can factor in the variation in frame rate. */
		auto Tail = StoredFrames.GetTail();
		if (Tail) // If tail is valid
		{
			AActor* Owner = GetOwner();
			Owner->SetActorLocation(Tail->GetValue().Location);
			Owner->SetActorRotation(Tail->GetValue().Rotation);

			TArray<UActorComponent*> Components = Owner->GetComponentsByClass(UStaticMeshComponent::StaticClass());
			if (Components.Num() > 0)
			{
				UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(Components[0]);
				if (SMC)
				{
					SMC->SetPhysicsLinearVelocity(Tail->GetValue().LinearVelocity);
					SMC->SetPhysicsAngularVelocityInDegrees(Tail->GetValue().AngularVelocity);
				}
			}

			if (StoredFrames.GetHead() == Tail) // Check if it's the last node
			{
				RecordedTime = 0.0f;
				bOutOfData = true;
			}
			else
			{
				RecordedTime -= Tail->GetValue().DeltaTime;
			}

			StoredFrames.RemoveNode(Tail);
		}
	}
}

void UTimeRevComponent::SetReversingTime(bool InReversingTime)
{
	bReversingTime = InReversingTime;

	if (bReversingTime) { UE_LOG(LogTemp, Log, TEXT("Reversing Time")); }
	else { UE_LOG(LogTemp, Log, TEXT("Stopped Reversing Time")); }
}