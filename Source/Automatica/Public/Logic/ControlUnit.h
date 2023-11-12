// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "ControlUnitIcon.h"
#include "LogicActor.h"
#include "GameFramework/Actor.h"
#include "ControlUnit.generated.h"

USTRUCT()
struct FControlUnitCommandInstance
{
	GENERATED_BODY()

	/** Icon instance */
	UPROPERTY() AControlUnitIcon* Icon;
	UPROPERTY() uint8 Layer = 0;
};

UCLASS()
class AUTOMATICA_API AControlUnit : public ALogicActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AControlUnit();

	/** Add the supplied command to the unit */
	UFUNCTION(BlueprintCallable, Category="Control Unit")
	void AddCommand(ELogicControlType Command);
	
	/** Removes the last command from back */
	UFUNCTION(BlueprintCallable, Category="Control Unit")
	void Backspace();
	
	/** Closes the current nesting and moves one layer up */
	UFUNCTION(BlueprintCallable, Category="Control Unit")
	void CloseNesting();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditDefaultsOnly, Category="Control Unit|Construction")
	UStaticMesh* ScreenMesh;
	UPROPERTY(EditDefaultsOnly, Category="Control Unit|Construction")
	TSubclassOf<AControlUnitIcon> IconClass;
	
	UPROPERTY(VisibleInstanceOnly, Category="Debug", DisplayName="Current Layer")
	uint8 CurrentLayer = 0;

	UPROPERTY(EditDefaultsOnly, Category="Control Unit|Construction", meta=(ClampMin=0, UIMin=0))
	float ScreenMeshWidth = 30;
	UPROPERTY(EditAnywhere, Category="Control Unit", meta=(ClampMin=0, UIMin=0))
	float ScreenWidth = 373;
	UPROPERTY(EditAnywhere, Category="Control Unit", meta=(ClampMin=0, UIMin=0))
	float ScreenHeight = 60;

	UPROPERTY()	UStaticMeshComponent* Screen;
	UPROPERTY() TArray<FControlUnitCommandInstance> Commands;

protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, Category="Debug", DisplayName="Command to add")
	TEnumAsByte<ELogicControlType> _D_CommandToAdd;

	UFUNCTION(CallInEditor, Category="Debug", DisplayName="Add")
	void _D_AddCommand();
	UFUNCTION(CallInEditor, Category="Debug", DisplayName="Backspace")
	void _D_Backspace();
	UFUNCTION(CallInEditor, Category="Debug", DisplayName="End Nesting (if active)")
	void _D_EndNested();
#endif
};
