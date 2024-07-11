// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AttachArrow.generated.h"

class AProjectile;
/**
 * 
 */
UCLASS()
class HOMEWORKPROJECT_API UAnimNotify_AttachArrow : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	FName EquippedSocketName;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
