// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"


// Sets default values
ATile::ATile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CubeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	SetRootComponent(CubeMeshComponent);
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

	TileType = GetRandomTileTypeEnum();
}

void ATile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsMoving)
	{
		FVector CurrentLocation = GetActorLocation();
		FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaSeconds, 5.0f);
		SetActorLocation(NewLocation);

		// if (FVector::Dist(NewLocation, TargetLocation) < KINDA_SMALL_NUMBER)
		if (FVector::Dist(NewLocation, TargetLocation) < 10.0f)
		{
			SetActorLocation(TargetLocation);
			StopMoving();
		}
	}
}

ETileType ATile::GetRandomTileTypeEnum() const
{
	int32 Index = FMath::RandRange(0, static_cast<int>(ETileType::White));

	switch (Index)
	{
	case 0:
		return ETileType::Red;
	case 1:
		return ETileType::Green;
	case 2:
		return ETileType::Blue;
	case 3:
		return ETileType::White;
	default:
		return ETileType::White;
	}
}

void ATile::DestroyAndSpawnEmitter()
{
	Destroy();
	// Spawn Effect On this Part

	// // // // // 
}

void ATile::StartMoving()
{
	bIsMoving = true;
	OnTileStartMoveDelegate.Broadcast();
}

void ATile::StopMoving()
{
	bIsMoving = false;
	OnTileStopMoveDelegate.Broadcast();
}

void ATile::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);

	OnTileClicked.Broadcast(this);

	UE_LOG(LogTemp, Warning, TEXT("NotifyActorOnClicked"));
}

void ATile::SetTargetLoc(const FVector& NewLocation)
{
	TargetLocation = NewLocation;
}

void ATile::SetMaterialEmission(const bool bEmission) const
{
	if (bEmission)
		CubeMeshComponent->SetScalarParameterValueOnMaterials(FName("Emission"), 20.0f);
	else
		CubeMeshComponent->SetScalarParameterValueOnMaterials(FName("Emission"), 1.0f);
}
