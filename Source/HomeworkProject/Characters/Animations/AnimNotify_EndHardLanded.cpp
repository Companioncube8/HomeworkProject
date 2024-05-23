// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_EndHardLanded.h"

#include "HomeworkProject/Characters/FPPlayerCharacter.h"

void UAnimNotify_EndHardLanded::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AFPPlayerCharacter* CharacterOwner = Cast<AFPPlayerCharacter>(MeshComp->GetOwner());
	if (IsValid(CharacterOwner))
	{
		CharacterOwner->EndHardLanded();
	}
}
