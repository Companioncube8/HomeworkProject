// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Items/Powerups/Medkit.h"

#include "Characters/BaseCharacter.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"

bool UMedkit::Consume(ABaseCharacter* ConsumeTarget)
{
	UCharacterAttributeComponent* CharacterAttribute = ConsumeTarget->GetCharacterAttributeComponent_Mutable();
	CharacterAttribute->AddHealth(Health);
	this->ConditionalBeginDestroy();
	return true;
}
