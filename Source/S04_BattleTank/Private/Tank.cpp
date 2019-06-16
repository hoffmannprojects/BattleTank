// Fill out your copyright notice in the Description page of Project Settings.

#include "Tank.h"
#include "TankBarrel.h"
#include "Engine/World.h"
#include "TankAimingComponent.h"
#include "TankNavMovementComponent.h"
#include "Projectile.h"

// Sets default values
ATank::ATank()
{
	//Doesn't need to tick every frame.
	PrimaryActorTick.bCanEverTick = false;

	// No need to protect pointers here, as they are added at construction.
	TankAimingComponent = CreateDefaultSubobject<UTankAimingComponent>("AimingComponent");
}

#pragma region Overrides

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();
}
#pragma endregion

void ATank::SetBarrelReference(UTankBarrel* BarrelToSet)
{
	TankAimingComponent->SetBarrelReference(BarrelToSet);
	Barrel = BarrelToSet;
}

void ATank::SetTurretReference(UTankTurret* TurretToSet) const
{
	TankAimingComponent->SetTurretReference(TurretToSet);
}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATank::AimAt(const FVector TargetLocation) const
{
	TankAimingComponent->AimAt(TargetLocation, LaunchSpeed);
}

void ATank::Fire()
{
	bool bIsReloaded = (GetWorld()->GetTimeSeconds() - LastFireTime) > ReloadTimeSeconds;

	if (ensure(Barrel != nullptr) && bIsReloaded)
	{
		// Spawns a Projectile.
		auto Projectile = GetWorld()->SpawnActor<AProjectile>(
			ProjectileBP,
			Barrel->GetSocketLocation("ProjectileStart"),
			Barrel->GetSocketRotation("ProjectileStart")
			);

		Projectile->LaunchProjectile(LaunchSpeed);
		LastFireTime = GetWorld()->GetTimeSeconds();
	}
}