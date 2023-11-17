// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "FMODAudioComponent.h"
#include "FMODEvent.h"
#include "GameFramework/Actor.h"
#include "Elevator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FElevatorMovementEvent_d);

/**  */
UCLASS()
class AUTOMATICA_API AElevator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AElevator();

	/** Floor height in Unreal units (cm)  */
	UPROPERTY(EditInstanceOnly, Category = "Elevator", meta=(ClampMin=0, UIMin=0))
	float FloorHeight;
	/** Speed in cm/s the elevator shall travel at min */
	UPROPERTY(EditInstanceOnly, Category = "Elevator", meta=(ClampMin=0, UIMin=0))
	float MinTravelSpeed;
	/** Speed in cm/s the elevator shall travel at max */
	UPROPERTY(EditInstanceOnly, Category = "Elevator", meta=(ClampMin=0, UIMin=0))
	float MaxTravelSpeed;
	/** Distance that the elevator will use to accelerate / decelerate */
	UPROPERTY(EditInstanceOnly, Category = "Elevator", meta=(ClampMin=0, UIMin=0))
	float AccelerationRegion;
	/** Floor height in Unreal units (cm)  */
	UPROPERTY(EditInstanceOnly, Category = "Elevator")
	float ApproachingOffset;
	
	/** Switches to the next upper floor */
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Elevator")
	void TravelUp();

	/** Switches to the next lower floor */
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Elevator")
	void TravelDown();

	/** Travels to a specific z position */
	UFUNCTION(BlueprintCallable, Category = "Elevator")
	void TravelTo(float ZPosition);

	/** Travels to a specific floor index */
	UFUNCTION(BlueprintCallable, Category = "Elevator")
	void TravelToFloor(int FloorIndex);

	/** Approaches specific floor from below by the given height */
	UFUNCTION(BlueprintCallable, Category = "Elevator")
	void ApproachFloor(int FloorIndex);

	UPROPERTY(BlueprintAssignable, Category = "Automatica|Elevator")
	FElevatorMovementEvent_d OnElevatorStop;
	
	UPROPERTY(BlueprintAssignable, Category = "Automatica|Elevator")
	FElevatorMovementEvent_d OnElevatorStart;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	UFMODEvent* ElevatorSound;

private:
	float StartingZPosition;
	float TargetZPosition;

	UPROPERTY()
	UFMODAudioComponent* ElevatorSoundInstance;
};