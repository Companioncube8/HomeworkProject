// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Items/Powerups/Adrenaline.h"

#include "Characters/BaseCharacter.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"

bool UAdrenaline::Consume(ABaseCharacter* ConsumeTarget)
{
	UCharacterAttributeComponent* CharacterAttribute = ConsumeTarget->GetCharacterAttributeComponent_Mutable();
	CharacterAttribute->RestoreFullStamina();
	this->ConditionalBeginDestroy();
	return true;
}
