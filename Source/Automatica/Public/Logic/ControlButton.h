// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "LogicActor.h"
//#include "Core/InteractionCatcher.h"
#include "ControlButton.generated.h"

class UInteractionCatcher;
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

/** Add / Remove mode for counter modifier */
UENUM(BlueprintType)
enum EControlButtonCounterModifier: uint8
{
	Add			= 0,
	Subtract	= 1
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

	/** The command to emit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Control Button", meta=(EditCondition="Type == 1", EditConditionHides = true))
	TEnumAsByte<EControlButtonCounterModifier> Modifier;

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

	void InteractSetup_Bind(UInteractionCatcher* Catcher);
	void InteractSetup_Release(UInteractionCatcher* Catcher);
	
protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	void ConstructShape(EControlButtonShape Shape) const;

	void HandleInteractionCatcherUpdate();

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
	UStaticMesh* PlaneMesh;
	UPROPERTY(EditDefaultsOnly, Category="Control Button|Setup")
	UControlCommandSettings* CommandMap;
	UPROPERTY(EditDefaultsOnly, Category="Control Button|Setup")
	float ScaleModifier;
	UPROPERTY(EditDefaultsOnly, Category="Control Button|Setup")
	float IconScaleModifier;
	
	UPROPERTY(EditDefaultsOnly, Category="Control Button|Setup|Materials")
	UMaterialInterface* MatIconPlus;
	UPROPERTY(EditDefaultsOnly, Category="Control Button|Setup|Materials")
	UMaterialInterface* MatIconMinus;
	UPROPERTY(EditDefaultsOnly, Category="Control Button|Setup|Materials")
	UMaterialInterface* MatIconBackspace;

private:
	UPROPERTY() UStaticMeshComponent* CFrame;
	UPROPERTY() UStaticMeshComponent* CFace;
	UPROPERTY() UStaticMeshComponent* CIcon;

	UPROPERTY() TArray<UInteractionCatcher*> CatchersActive;
};
