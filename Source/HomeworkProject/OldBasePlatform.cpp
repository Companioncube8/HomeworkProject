// Fill out your copyright notice in the Description page of Project Settings.


#include "OldBasePlatform.h"

// Sets default values
AOldBasePlatform::AOldBasePlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* DefaultPlatformRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PlatformRoot"));
	RootComponent = DefaultPlatformRoot;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform"));
	PlatformMesh->SetupAttachment(DefaultPlatformRoot);
}

// Called when the game starts or when spawned
void AOldBasePlatform::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = PlatformMesh->GetRelativeLocation();

	FOnTimelineFloatStatic PlatformMovementTimelineUpdate;
	PlatformMovementTimelineUpdate.BindUObject(this, &AOldBasePlatform::PlatformTimelineUpdate);
	PlatformTimeline.AddInterpFloat(TimelineCurve, PlatformMovementTimelineUpdate);

	FOnTimelineEventStatic PlatformMovementFinished;
	PlatformMovementFinished.BindUObject(this, &AOldBasePlatform::PlatformTimelineEnd);
	PlatformTimeline.SetTimelineFinishedFunc(PlatformMovementFinished);

	if (IsValid(PlatformInvocator))
	{
		PlatformInvocator->OnInvocatorActivated.AddDynamic(this, &AOldBasePlatform::OnPlatformInvoked);
	}
}

void AOldBasePlatform::PlatformTimelineUpdate(const float Alpha)
{
	const FVector PlatformTargetLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
	PlatformMesh->SetRelativeLocation(PlatformTargetLocation);
}

void AOldBasePlatform::PlatformTimelineStart()
{
	if (bIsPlatformInEndLocation)
	{
		PlatformTimeline.Reverse();
	}
	else
	{
		PlatformTimeline.Play();
	}

}

void AOldBasePlatform::PlatformTimelineEnd()
{
	if (PlatformBehavior == EPlatformBehavior::Loop)
	{
		GetWorldTimerManager().SetTimer(PlatformLoopTimerHandle, this, &AOldBasePlatform::PlatformTimelineStart, LoopTime, false);
	}

	bIsPlatformInEndLocation = !bIsPlatformInEndLocation;
}

void AOldBasePlatform::OnPlatformInvoked()
{
	PlatformTimelineStart();
}

void AOldBasePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlatformTimeline.TickTimeline(DeltaTime);
}