// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTServices/BTService_Grenade.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/BaseCharacter.h"

UBTService_Grenade::UBTService_Grenade()
{
	NodeName = "Grenade";
}


void UBTService_Grenade::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!AIController || !Blackboard)
	{
		return;
	}

	ABaseCharacter* Character = Cast<ABaseCharacter>(AIController->GetPawn());
	if (!Character)
	{
		return;
	}

	AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!CurrentTarget)
	{
		Character->PlayerStopFire();
		return;
	}

	float DistSq = FVector::DistSquared(CurrentTarget->GetActorLocation(), Character->GetActorLocation());

	if (DistSq < FMath::Square(MaxGrenadeDistance) && DistSq > FMath::Square(MinGrenadeDistance))
	{
		Character->PlayerStopFire();
		Character->EquipPrimaryItem();
		return;
	}
}
