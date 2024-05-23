// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlatformInvocator.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "BasePlatform.generated.h"

UENUM(BlueprintType)
enum class EPlatformBehavior: uint8
{
	OnDemand = 0,
	Loop
};

UCLASS()
class HOMEWORKPROJECT_API ABasePlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasePlatform();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PlatformMesh;

	FTimeline PlatformTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCurveFloat* TimelineCurve;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (MakeEditWidget))
	FVector EndLocation;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Transient)
	FVector StartLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPlatformBehavior PlatformBehavior = EPlatformBehavior::OnDemand;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LoopTime = 2;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	APlatformInvocator* PlatformInvocator;

	UFUNCTION()
	void OnPlatformInvoked();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void PlatformTimelineStart();
	void PlatformTimelineEnd();
	void PlatformTimelineUpdate(float Alpha);

	FTimerHandle PlatformLoopTimerHandle;
	bool bIsPlatformInEndLocation = false;
public:
	virtual void Tick(float DeltaTime) override;
};
