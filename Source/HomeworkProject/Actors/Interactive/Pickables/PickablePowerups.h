// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactive/Pickables/PickableItem.h"
#include "PickablePowerups.generated.h"

class ABaseCharacter;
/**
 * 
 */
UCLASS(Blueprintable)
class HOMEWORKPROJECT_API APickablePowerups : public APickableItem
{
	GENERATED_BODY()

public:
	APickablePowerups();

	virtual void Interact(ABaseCharacter* Character) override;
	virtual FName GetActionEventName() const override;

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PowerupMesh;
};
