// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactive/Enviroment/Door.h"

#include "HomeworkProjectTypes.h"

// Sets default values
ADoor::ADoor()
{
	USceneComponent* DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorRoot"));
	SetRootComponent(DefaultSceneRoot);

	DoorPivot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorPivot"));
	DoorPivot->SetupAttachment(GetRootComponent());

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(DoorPivot);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	if (DoorAnimationCurve)
	{
		FOnTimelineFloatStatic DoorAnimationDelegate;
		DoorAnimationDelegate.BindUObject(this, &ADoor::UpdateDoorAnimation);
		DoorOpenAnimTimeline.AddInterpFloat(DoorAnimationCurve, DoorAnimationDelegate);

		FOnTimelineEventStatic DoorOpenDelegate;
		DoorOpenDelegate.BindUObject(this, &ADoor::OnDoorAnimationFinished);
		DoorOpenAnimTimeline.SetTimelineFinishedFunc(DoorOpenDelegate);
	}
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DoorOpenAnimTimeline.TickTimeline(DeltaTime);
}

void ADoor::UpdateDoorAnimation(float Alpha)
{
	float YawAngle = FMath::Lerp(AngleClosed, AngleOpened, FMath::Clamp(Alpha, 0.f, 1.f));
	DoorPivot->SetRelativeRotation(FRotator(0.f, YawAngle, 0.f));
}

void ADoor::OnDoorAnimationFinished()
{
	SetActorTickEnabled(false);
}

void ADoor::InteractWithDoot()
{
	SetActorTickEnabled(true);
	if (bIsOpened)
	{
		DoorOpenAnimTimeline.Reverse();
	}
	else
	{
		DoorOpenAnimTimeline.Play();
	}
	bIsOpened = !bIsOpened;
}

FName ADoor::GetActionEventName() const
{
	return  ActionInteract;
}


void ADoor::Interact(ABaseCharacter* Character)
{
	ensureMsgf(DoorAnimationCurve, TEXT("Dore animation curve is not set"));
	InteractWithDoot();
}
