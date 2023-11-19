// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "LogicActor.h"
#include "RasterField.generated.h"

class ARasterCharacter;

UENUM(BlueprintType)
enum ERasterFieldType: uint8
{
	Normal		= 0,
	Lift		= 1,
	Gasfire		= 2,
	Beet		= 3
};

UCLASS()
class AUTOMATICA_API ARasterField : public ALogicActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARasterField();

	virtual void LogicReset() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void ResetField();

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Raster Field")
	TEnumAsByte<ERasterFieldType> Type;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Raster Field", meta=(EditCondition="Type==1", EditConditionHides=true))
	bool bLiftStartElevated;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Raster Field", meta=(EditCondition="Type==2", EditConditionHides=true))
	bool bFireActive;

	UFUNCTION(BlueprintImplementableEvent, Category="Raster Field")
	void OnPowerStateChange(bool bPowered);

	virtual void OnReceiveCommand(ELogicControlType Command) override;

	UFUNCTION(BlueprintCallable, Category="Raster Field")
	void SetDeadly(bool bDeadly);
	
	UPROPERTY()
	bool bIsDeadly;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Raster Field")
	FVector GetCharAnchor();

	void HandleCharEntering(ARasterCharacter* Character);
	void HandleCharLeaving(ARasterCharacter* Character);

	bool IsGoalType() const;

	void SetCharacterRegisterd(ARasterCharacter* Character, bool bRegistered);

	UFUNCTION(BlueprintCallable, Category= "Raster Field")
	void SetZOffset(double Offset);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category= "Raster Field")
	double GetZOffset() const;

private:
	UPROPERTY()
	bool bIsPowered;
	
	UPROPERTY()
	double ZOffset;

	UPROPERTY()
	TArray<ARasterCharacter*> HoldingChars;
};
