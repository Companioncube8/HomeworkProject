// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlatform.h"

// Sets default values
ABasePlatform::ABasePlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* DefaultPlatformRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PlatformRoot"));
	RootComponent = DefaultPlatformRoot;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform"));
	PlatformMesh->SetupAttachment(DefaultPlatformRoot);
}

// Called when the game starts or when spawned
void ABasePlatform::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = PlatformMesh->GetRelativeLocation();

	FOnTimelineFloatStatic PlatformMovementTimelineUpdate;
	PlatformMovementTimelineUpdate.BindUObject(this, &ABasePlatform::PlatformTimelineUpdate);
	PlatformTimeline.AddInterpFloat(TimelineCurve, PlatformMovementTimelineUpdate);

	FOnTimelineEventStatic PlatformMovementFinished;
	PlatformMovementFinished.BindUObject(this, &ABasePlatform::PlatformTimelineEnd);
	PlatformTimeline.SetTimelineFinishedFunc(PlatformMovementFinished);

	if (IsValid(PlatformInvocator))
	{
		PlatformInvocator->OnInvocatorActivated.AddDynamic(this, &ABasePlatform::OnPlatformInvoked);
	}
}

void ABasePlatform::PlatformTimelineUpdate(const float Alpha)
{
	const FVector PlatformTargetLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
	PlatformMesh->SetRelativeLocation(PlatformTargetLocation);
}

void ABasePlatform::PlatformTimelineStart()
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

void ABasePlatform::PlatformTimelineEnd()
{
	if (PlatformBehavior == EPlatformBehavior::Loop)
	{
		GetWorldTimerManager().SetTimer(PlatformLoopTimerHandle, this, &ABasePlatform::PlatformTimelineStart, LoopTime, false);
	}

	bIsPlatformInEndLocation = !bIsPlatformInEndLocation;
}

void ABasePlatform::OnPlatformInvoked()
{
	PlatformTimelineStart();
}

void ABasePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlatformTimeline.TickTimeline(DeltaTime);
}