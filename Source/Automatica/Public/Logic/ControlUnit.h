// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "ControlUnitIcon.h"
#include "LogicActor.h"
#include "GameFramework/Actor.h"
#include "ControlUnit.generated.h"

UCLASS()
class AUTOMATICA_API AControlUnit : public ALogicActor
{
	GENERATED_BODY()

public:
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

	/** Starts sequence playback */
	void PlaySequence();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	void HandleLayer(const uint8 BatchLayer, int& Index, double LeftOffset, AControlUnitIcon* Container = nullptr);
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditDefaultsOnly, Category="Control Unit|Construction")
	UStaticMesh* ScreenMesh;
	UPROPERTY(EditDefaultsOnly, Category="Control Unit|Construction")
	TSubclassOf<AControlUnitIcon> IconClass;
	UPROPERTY(EditDefaultsOnly, Category="Control Unit|Construction")
	UMaterialInterface* ScreenMaterial;
	
	UPROPERTY(VisibleInstanceOnly, Category="Debug", DisplayName="Current Layer")
	uint8 CurrentLayer = 0;
	
	UPROPERTY(EditDefaultsOnly, Category="Control Unit")
	double SequenceTickTime = 0.6;

	UPROPERTY(EditDefaultsOnly, Category="Control Unit|Construction", meta=(ClampMin=0, UIMin=0))
	float ScreenMeshWidth = 30;
	UPROPERTY(EditAnywhere, Category="Control Unit", meta=(ClampMin=0, UIMin=0))
	float ScreenWidth = 373;
	UPROPERTY(EditAnywhere, Category="Control Unit", meta=(ClampMin=0, UIMin=0))
	float ScreenHeight = 60;
	UPROPERTY(EditAnywhere, Category="Control Unit", meta=(ClampMin=0, UIMin=0))
	float ScreenPadding = 20;
	UPROPERTY(EditAnywhere, Category="Control Unit", meta=(ClampMin=0, UIMin=0))
	float GridGap = 3;
	UPROPERTY(EditAnywhere, Category="Control Unit", meta=(ClampMin=0.5, UIMin=0.5, ClampMax=0.9, UIMax=0.9))
	float LayerScaleFactor = 0.8;

	UPROPERTY()	UStaticMeshComponent* Screen;
	UPROPERTY() TArray<AControlUnitIcon*> Commands;
	UPROPERTY() TArray<AControlUnitIcon*> CommandChildBuffer;
	UPROPERTY() TArray<int> ContainerIndexStack;
	UPROPERTY(VisibleInstanceOnly, Category="Debug", DisplayName="Command Index Pointer") int CommandIndexPtr = -1;

	UPROPERTY() bool bIsPlaying;
	UPROPERTY() int SequencePointer;
	UPROPERTY() double SequenceLastTime;
};
