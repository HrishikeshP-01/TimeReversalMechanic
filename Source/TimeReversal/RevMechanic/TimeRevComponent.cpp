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
		RunningTime = 0.0f;
		LeftReverseRunningTime = 0.0f;
		RightReverseRunningTime = 0.0f;

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
		RunningTime += DeltaTime;

		auto Right = StoredFrames.GetTail();
		auto Left = Right->GetPrevNode();
		if (!Left) { return; }
		LeftReverseRunningTime = RightReverseRunningTime + Right->GetValue().DeltaTime;

		while (RunningTime > LeftReverseRunningTime)
		{
			RightReverseRunningTime += Right->GetValue().DeltaTime;
			// RightReverseRunningTime = LeftReverseRunningTime;
			Right = Left;
			LeftReverseRunningTime += Left->GetValue().DeltaTime;
			Left = Left->GetPrevNode();

			auto Tail = StoredFrames.GetTail();
			RecordedTime -= Tail->GetValue().DeltaTime;
			StoredFrames.RemoveNode(Tail);
			if (Left == StoredFrames.GetHead())
			{
				bOutOfData = true;
				//RecordedTime = 0.0f;
				break;
			}
		}
		// Double checking to make sure RecordedTime is between RightReverseRunningTime & LeftReverseRunningTime
		if (RunningTime >= RightReverseRunningTime && RunningTime <= LeftReverseRunningTime)
		{
			/*Now we have to compute the alpha for the future interp functions. To get the alpha we use the LRRT & RRRT as intervals
			& RT as value. We have to convert this such that we get the ratio/fraction which is between 0 & 1*/
			float DT = RunningTime - RightReverseRunningTime;
			float Interval = LeftReverseRunningTime - RightReverseRunningTime;
			float fraction = DT / Interval;

			FVector InterpLoc = FMath::VInterpTo(Right->GetValue().Location, Left->GetValue().Location, fraction, 1.0f);
			FRotator InterpRot = FMath::RInterpTo(Right->GetValue().Rotation, Left->GetValue().Rotation, fraction, 1.0f);
			FVector InterpLinVel = FMath::VInterpTo(Right->GetValue().LinearVelocity, Left->GetValue().LinearVelocity, fraction, 1.0f);
			FVector InterpAngVel = FMath::VInterpTo(Right->GetValue().AngularVelocity, Left->GetValue().AngularVelocity, fraction, 1.0f);

			SetActorVariables(InterpLoc, InterpRot, InterpLinVel, InterpAngVel);
		}
	}
}

void UTimeRevComponent::SetReversingTime(bool InReversingTime)
{
	bReversingTime = InReversingTime;

	if (bReversingTime) { UE_LOG(LogTemp, Log, TEXT("Reversing Time")); }
	else { UE_LOG(LogTemp, Log, TEXT("Stopped Reversing Time")); }
}

void UTimeRevComponent::SetActorVariables(FVector Location, FRotator Rotation, FVector LinearVelocity, FVector AngularVelocity)
{
	AActor* Owner = GetOwner();
	Owner->SetActorLocation(Location);
	Owner->SetActorRotation(Rotation);

	TArray<UActorComponent*> Components = Owner->GetComponentsByClass(UStaticMeshComponent::StaticClass());
	if (Components.Num() > 0)
	{
		UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(Components[0]);
		if (SMC)
		{
			SMC->SetPhysicsLinearVelocity(LinearVelocity);
			SMC->SetPhysicsAngularVelocityInDegrees(AngularVelocity);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Executed: SetActorVariables"));
}