// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "ControlUnitIcon.h"
#include "LogicActor.h"
#include "ControlButton.generated.h"

class AControlButton;

/** Display shape for control buttons */
UENUM(BlueprintType)
enum EControlButtonShape: uint8
{
	Squared,
	Round
};

/** Functional type for control buttons */
UENUM(BlueprintType)
enum EControlButtonType: uint8
{
	Command			= 0,
	CounterModifier	= 1,
	Backspace		= 2
};

/** Button options for building */
USTRUCT(BlueprintType)
struct FControlButtonSetup
{
	GENERATED_BODY()

	/** The button's functional type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Control Button")
	TEnumAsByte<EControlButtonType> Type;

	/** The command to emit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Control Button", meta=(EditCondition="Type == 0", EditConditionHides = true))
	TEnumAsByte<ELogicControlType> Command;

	/** Button holding the command the counter shall be modified for */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Control Button", meta=(EditCondition="Type == 1", EditConditionHides = true))
	AControlButton* RelatedCounterButton;
};

UCLASS()
class AUTOMATICA_API AControlButton : public ALogicActor
{
	GENERATED_BODY()

public:
	AControlButton();

	/** Emits a button press */
	UFUNCTION(BlueprintCallable, Category="Control Button")
	void Press();

	/** Sets up the button with the provided setup */
	UFUNCTION(BlueprintCallable, Category="Control Button")
	void SetButtonSetup(FControlButtonSetup ButtonSetup);
	
protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	void ConstructShape(EControlButtonShape Shape) const;
	void ConstructIcon(bool bMakeIcon);

	/** Button setup. Should be set via SetButtonSetup() on runtime */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Control Button")
	FControlButtonSetup Setup;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Control Button|Setup")
	UStaticMesh* FrameMeshSquare;
	UPROPERTY(EditDefaultsOnly, Category="Control Button|Setup")
	UStaticMesh* FaceMeshSquare;
	UPROPERTY(EditDefaultsOnly, Category="Control Button|Setup")
	UStaticMesh* FrameMeshRound;
	UPROPERTY(EditDefaultsOnly, Category="Control Button|Setup")
	UStaticMesh* FaceMeshRound;
	UPROPERTY(EditDefaultsOnly, Category="Control Button|Setup")
	TSubclassOf<AControlUnitIcon> IconClass;
	UPROPERTY(EditDefaultsOnly, Category="Control Button|Setup")
	float ScaleModifier;
	UPROPERTY(EditDefaultsOnly, Category="Control Button|Setup")
	float IconScaleModifier;

private:
	UPROPERTY() UStaticMeshComponent* CFrame;
	UPROPERTY() UStaticMeshComponent* CFace;
	UPROPERTY() AControlUnitIcon* CIcon;
};
