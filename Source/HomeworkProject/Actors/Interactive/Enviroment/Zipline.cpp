// Fill out your copyright notice in the Description page of Project Settings.



#include "Zipline.h"

#include "Components/CapsuleComponent.h"
#include "HomeworkProject/HomeworkProjectTypes.h"

// Sets default values
AZipline::AZipline()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ZiplineRoot"));

	ZiplineFirstPillarComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FirstPillar"));
	ZiplineFirstPillarComponent->SetupAttachment(RootComponent);

	ZiplineSecondPillarComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondPillar"));
	ZiplineSecondPillarComponent->SetupAttachment(RootComponent);

	ZiplineCabel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cabel"));
	ZiplineCabel->SetupAttachment(RootComponent);

	InteractionVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(CollisionProfilePawnInteractionVolume);
	InteractionVolume->SetGenerateOverlapEvents(true);
}


void AZipline::OnConstruction(const FTransform& Transform)
{

	UStaticMesh* ZiplineFirstPillarMesh = ZiplineFirstPillarComponent->GetStaticMesh();
	if (IsValid(ZiplineFirstPillarMesh))
	{
		float MeshHight = ZiplineFirstPillarMesh->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshHight))
		{
			ZiplineFirstPillarComponent->SetRelativeScale3D(FVector(1.f, 1.f, ZiplinePillarHeight / MeshHight));
		}
	}

	UStaticMesh* ZiplineSecondPillarMesh = ZiplineSecondPillarComponent->GetStaticMesh();
	if (IsValid(ZiplineSecondPillarMesh))
	{
		float MeshHight = ZiplineSecondPillarMesh->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshHight))
		{
			ZiplineSecondPillarComponent->SetRelativeScale3D(FVector(1.f, 1.f, ZiplinePillarHeight / MeshHight));
		}
	}

	FVector FirstTop = ZiplineFirstPillarComponent->GetRelativeLocation() + ZiplinePillarHeight * 0.5 * FVector::UpVector;
	FVector SecondTop = ZiplineSecondPillarComponent->GetRelativeLocation() + ZiplinePillarHeight * 0.5 * FVector::UpVector;

	UStaticMesh* CabelMesh = ZiplineCabel->GetStaticMesh();
	if (IsValid(CabelMesh))
	{
		float MeshWidth = CabelMesh->GetBoundingBox().GetSize().X;
		if (!FMath::IsNearlyZero(MeshWidth))
		{
			ZiplineCabel->SetRelativeScale3D(FVector(FVector::Distance(FirstTop, SecondTop) / MeshWidth, 1.f, 1.f));
		}
	}
	ZiplineCabel->SetRelativeLocation((FirstTop + SecondTop) * 0.5);

	ZiplineCabel->SetRelativeRotation((FirstTop - SecondTop).Rotation());

	
	GetZiplineInteractionCapsule()->SetRelativeLocation((FirstTop + SecondTop) * 0.5);

	FRotator InteractiveVolumeRotation = (FirstTop - SecondTop).Rotation();
	InteractiveVolumeRotation.Pitch += 90;
	GetZiplineInteractionCapsule()->SetRelativeRotation(InteractiveVolumeRotation);
	GetZiplineInteractionCapsule()->SetCapsuleHalfHeight(FVector::Distance(FirstTop, SecondTop) * 0.5);
}

UCapsuleComponent* AZipline::GetZiplineInteractionCapsule() const
{
	return StaticCast<UCapsuleComponent*>(InteractionVolume);
}

FVector AZipline::GetTopPointOfCable() const
{
	FVector FirstTop = ZiplineFirstPillarComponent->GetComponentLocation() + ZiplinePillarHeight * 0.5 * FVector::UpVector;
	FVector SecondTop = ZiplineSecondPillarComponent->GetComponentLocation() + ZiplinePillarHeight * 0.5 * FVector::UpVector;
	return FirstTop.Z > SecondTop.Z ? FirstTop : SecondTop;
}

FVector AZipline::GetBottomPointOfCable() const
{
	FVector FirstTop = ZiplineFirstPillarComponent->GetComponentLocation() + ZiplinePillarHeight * 0.5 * FVector::UpVector;
	FVector SecondTop = ZiplineSecondPillarComponent->GetComponentLocation() + ZiplinePillarHeight * 0.5 * FVector::UpVector;
	return FirstTop.Z < SecondTop.Z ? FirstTop : SecondTop;
}


FVector AZipline::GetBottomPoint() const
{
	return FMath::Lerp(GetTopPointOfCable(), GetBottomPointOfCable(), ZiplinePercentFromBottom);
}


FVector AZipline::GetTopPoint() const
{
	return FMath::Lerp(GetTopPointOfCable(), GetBottomPointOfCable(), ZiplinePercentFromTop);
}

FRotator AZipline::GetCabelOrientation() const
{
	return  ZiplineCabel->GetRelativeRotation();
}