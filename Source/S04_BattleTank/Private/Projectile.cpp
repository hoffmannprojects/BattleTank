// Copyright Tim Hoffmann (@timdhoffmann).

#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "TimerManager.h"

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>("CollisionMesh");
	SetRootComponent(CollisionMesh);
	CollisionMesh->SetNotifyRigidBodyCollision(true);
	CollisionMesh->SetVisibility(false);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	// Prevents projectile from flying off immediately.
	ProjectileMovementComponent->bAutoActivate = false;

	LaunchBlast = CreateDefaultSubobject<UParticleSystemComponent>("LaunchBlast");
	LaunchBlast->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	ImpactExplosion = CreateDefaultSubobject<UParticleSystemComponent>("ImpactExplosion");
	ImpactExplosion->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ImpactExplosion->bAutoActivate = false;

	ImpactRadialForce = CreateDefaultSubobject <URadialForceComponent>("ImpactRadialForce");
	// Needs to be attached to the component with relative transform in order to fly with the projectile!
	ImpactRadialForce->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Subscribes to the OnComponentHit physics event.
	// CollisionMesh needs to be defined first.
	CollisionMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

void AProjectile::LaunchProjectile(const float Speed) const
{
	ProjectileMovementComponent->SetVelocityInLocalSpace(FVector::ForwardVector * Speed);

	ProjectileMovementComponent->Activate();
}

#pragma region Event Handlers

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	FVector NormalImpulse, const FHitResult& Hit)
{
	LaunchBlast->Deactivate();
	ImpactExplosion->Activate();
	ImpactRadialForce->FireImpulse();

	// Applies damage.
	// Empty TArray to ignore no actors.
	const auto ActorsToIgnore = TArray<AActor*>();
	UGameplayStatics::ApplyRadialDamage(this, Damage, this->GetActorLocation(), ImpactRadialForce->Radius, UDamageType::StaticClass(), ActorsToIgnore);

	// Sets a new root component before destroying the current root component.
	SetRootComponent(ImpactExplosion);
	CollisionMesh->DestroyComponent();

	// Sets up a timer to  destroy this projectile after a delay.
	FTimerHandle ProjectileTimerHandle;
	GetWorld()->GetTimerManager().SetTimer
	(
		ProjectileTimerHandle,
		this,
		&AProjectile::OnProjectileTimerExpired,
		DestroyDelay
	);
}

void AProjectile::OnProjectileTimerExpired()
{
	this->Destroy();
}

#pragma endregion