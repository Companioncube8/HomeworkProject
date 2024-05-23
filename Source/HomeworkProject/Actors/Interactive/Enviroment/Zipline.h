// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HomeworkProject/Actors/Interactive/InteractiveActor.h"
#include "Zipline.generated.h"

UCLASS(Blueprintable)
class HOMEWORKPROJECT_API AZipline : public AInteractiveActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AZipline();

	virtual void OnConstruction(const FTransform& Transform) override;

	FVector GetTopPoint() const;
	FVector GetBottomPoint() const;
	FRotator GetCabelOrientation() const;
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters", meta = (ClampMin = "200", UIMin = "200"))
		float ZiplinePillarHeight = 200.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* ZiplineFirstPillarComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* ZiplineSecondPillarComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* ZiplineCabel;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float ZiplinePercentFromTop = 0.05;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float ZiplinePercentFromBottom = 0.97;

private:

	FVector GetBottomPointOfCable() const;
	FVector GetTopPointOfCable() const;

	UCapsuleComponent* GetZiplineInteractionCapsule() const;
};
