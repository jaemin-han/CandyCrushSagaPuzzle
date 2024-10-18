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

	TileType = GetTileType();
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

bool ATile::IsMatching(const ATile* OtherTile) const
{
	return this->TileType == OtherTile->TileType;
}

FName ATile::GetTileType() const
{
	// get random integer
	int32 index = FMath::RandRange(0, 3);

	if (index == 0)
	{
		return FName("Red");
	}
	else if (index == 1)
	{
		return FName("Blue");
	}
	else if (index == 2)
	{
		return FName("Green");
	}
	else if (index == 3)
	{
		return FName("White");
	}
	return FName("White");
}

void ATile::DestoryAndSpawnEmitter()
{
	Destroy();
	// Spawn Effect On this Part

	// // // // // 
}

void ATile::SetTargetLocation(FVector NewTargetLocation)
{
	TargetLocation = NewTargetLocation;
}

void ATile::StartMoving()
{
	bIsMoving = true;
	OnTileStartMovingDelegate.Broadcast();
}

void ATile::StopMoving()
{
	bIsMoving = false;
	OnTileStopMovingDelegate.Broadcast();
}

void ATile::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);

	OnTileClicked.Broadcast(this);

	UE_LOG(LogTemp, Warning, TEXT("NotifyActorOnClicked"));
}
