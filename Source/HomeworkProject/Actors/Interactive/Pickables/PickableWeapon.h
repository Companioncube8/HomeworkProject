// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactive/Pickables/PickableItem.h"
#include "PickableWeapon.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class HOMEWORKPROJECT_API APickableWeapon : public APickableItem
{
	GENERATED_BODY()
public:
	APickableWeapon();
	virtual void Interact(ABaseCharacter* Character) override;
	virtual FName GetActionEventName() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* WeaponMesh;
};
