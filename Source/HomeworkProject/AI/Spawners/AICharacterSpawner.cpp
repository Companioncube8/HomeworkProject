// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Spawners/AICharacterSpawner.h"

#include "Actors/Interactive/Interface/Interactive.h"
#include "AI/Characters/AICharacter.h"

// Sets default values
AAICharacterSpawner::AAICharacterSpawner()
{
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnRoot"));
	SetRootComponent(SceneRoot);

}

// Called when the game starts or when spawned
void AAICharacterSpawner::BeginPlay()
{
	Super::BeginPlay();
	if (SpawnTrigger.GetInterface())
	{
		TriggerHandle = SpawnTrigger->AddOnInteractionUFunction(this, FName("SpawnAI"));
	}
	if (bIsSpawnOnStart)
	{
		SpawnAI();
	}
}

void AAICharacterSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnsubscribeFromTrigger();
	Super::EndPlay(EndPlayReason);
}

void AAICharacterSpawner::SpawnAI()
{
	if (!bCanSpawn || !CharacterClass)
	{
		return;
	}

	AAICharacter* AICharacter = GetWorld()->SpawnActor<AAICharacter>(CharacterClass, GetTransform());
	if (!AICharacter->Controller)
	{
		AICharacter->SpawnDefaultController();
	}

	if (bDoOnce)
	{
		UnsubscribeFromTrigger();
		bCanSpawn = false;
	}
}

void AAICharacterSpawner::UnsubscribeFromTrigger()
{
	if (TriggerHandle.IsValid() && SpawnTrigger.GetInterface())
	{
		SpawnTrigger->RemoveOnInteractionDelegate(TriggerHandle);
	}
}

void AAICharacterSpawner::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.Property->GetName() == GET_MEMBER_NAME_STRING_CHECKED(AAICharacterSpawner, SpawnTriggerActor))
	{
		SpawnTrigger = SpawnTriggerActor;
		if (SpawnTrigger.GetInterface())
		{
			if (!SpawnTrigger->HasOnInteractionCallback())
			{
				SpawnTriggerActor = nullptr;
				SpawnTrigger = nullptr;
			}
		} else
		{
			SpawnTriggerActor = nullptr;
			SpawnTrigger = nullptr;
		}
	}
}
