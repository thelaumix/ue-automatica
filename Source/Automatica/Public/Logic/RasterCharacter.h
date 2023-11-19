// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "LogicActor.h"
#include "RasterField.h"
#include "RasterCharacter.generated.h"

UCLASS()
class AUTOMATICA_API ARasterCharacter : public ALogicActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARasterCharacter();

	virtual void LogicReset() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnReceiveCommand(ELogicControlType Command) override;

	UFUNCTION(BlueprintImplementableEvent)
	void AnimateMovement(FVector From, FVector To);

	UFUNCTION(BlueprintImplementableEvent)
	void AnimateRotation(FRotator From, FRotator To);

	UFUNCTION(BlueprintImplementableEvent)
	void AnimateDeath();

	UFUNCTION(BlueprintImplementableEvent)
	void ResetCharacter();

	/** Callback that should be called by BP once the animations are done */
	UFUNCTION(BlueprintCallable, Category = "Character")
	void AnimationEnded();

	/** Fetch the raster field the char is currently attached to */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	ARasterField* GetCurrentRasterField();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Kill();

	bool IsDead() const;

	bool IsInGoal() const;

	ARasterField* GetCurrentField() const;

private:
	UPROPERTY() float StartRotation;
	UPROPERTY() float CurrentRotation;
	UPROPERTY() ARasterField* StartField;
	UPROPERTY() ARasterField* CurrentField;
	UPROPERTY() bool bIsDead;
	UPROPERTY() double CanNextDieIn;

	ARasterField* SweepRasterField(const FVector& Position) const;
	
};
