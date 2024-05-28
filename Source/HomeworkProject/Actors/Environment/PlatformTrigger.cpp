// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformTrigger.h"
#include "HomeworkProjectTypes.h"
#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

APlatformTrigger::APlatformTrigger()
{
	bReplicates = true;
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetCollisionProfileName(CollisionProfilePawnInteractionVolume);
	SetRootComponent(TriggerBox);
}

void APlatformTrigger::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APlatformTrigger::OnTriggerOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &APlatformTrigger::OnTriggerOverlapEnd);
}

void APlatformTrigger::SetIsActivated(bool bIsActivated_In)
{
	if (OnTriggerActivated.IsBound())
	{
		OnTriggerActivated.Broadcast(bIsActivated);
	}
}

void APlatformTrigger::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABaseCharacter* OtherPawn = Cast<ABaseCharacter>(OtherActor);
	if (!IsValid(OtherPawn))
	{
		return;
	}

	if (GetLocalRole() == ROLE_Authority || OtherPawn->IsLocallyControlled())
	{
		OverlappedPawns.AddUnique(OtherPawn);

		if (!bIsActivated && OverlappedPawns.Num() > 0)
		{
			bIsActivated = true;
			SetIsActivated(true);
		}
	}
}

void APlatformTrigger::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABaseCharacter* OtherPawn = Cast<ABaseCharacter>(OtherActor);
	if (!IsValid(OtherPawn))
	{
		return;
	}

	if (GetLocalRole() == ROLE_Authority || OtherPawn->IsLocallyControlled())
	{
		OverlappedPawns.RemoveSingleSwap(OtherPawn);

		if (bIsActivated && OverlappedPawns.Num() == 0)
		{
			bIsActivated = false;
			SetIsActivated(false);
		}
	}
}

void APlatformTrigger::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlatformTrigger, bIsActivated);
}

void APlatformTrigger::OnRep_IsActivated(bool bIsActivated_Old)
{
	SetIsActivated(bIsActivated);
}