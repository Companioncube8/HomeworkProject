// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_EndSlide.h"

#include "HomeworkProject/Characters/BaseCharacter.h"
#include "HomeworkProject/Components/MovemenyComponents/BaseCharacterMovementComponent.h"

void UAnimNotify_EndSlide::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	ABaseCharacter* CharacterOwner = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (IsValid(CharacterOwner))
	{
		CharacterOwner->GetBaseCharacterMovementComponent()->StopSlide();
	}
	
}
