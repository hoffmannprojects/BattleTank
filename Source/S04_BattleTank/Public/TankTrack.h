// Copyright Tim Hoffmann (@timdhoffmann).

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "TankTrack.generated.h"

#pragma region Forward Declarations

class ASprungWheel;

#pragma endregion

/**
 * Moves the tank.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent)) // Makes the component appear in the "add component" blueprint menu.
class S04_BATTLETANK_API UTankTrack : public UStaticMeshComponent
{
	GENERATED_BODY()

#pragma region Functions

public:

	// Sets the throttle on a single track.
	UFUNCTION(BlueprintCallable, Category = Input)
		void SetThrottle(const float Throttle) const;

private:

	UTankTrack();

	virtual void BeginPlay() override;

	void DriveTrack(float CurrentThrottle) const;

	TArray<ASprungWheel*> GetWheels() const;

#pragma endregion

#pragma region Variables

private:

	// Max force per track in Newtons.
	// Assume 40t tank, 1g acceleration.
	UPROPERTY(EditDefaultsOnly, Category = Setup)
		float MaxDrivingForce = 400000.f;

#pragma endregion
};
