// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "ControlCommandSettings.h"
#include "LogicActor.h"
#include "GameFramework/Actor.h"
#include "Utility/ParamAnimator.h"
#include "ControlUnitIcon.generated.h"

UCLASS()
class AUTOMATICA_API AControlUnitIcon : public ALogicActor
{
	GENERATED_BODY()
	AControlUnitIcon();

public:
	
	/** Set the command type for this unit icon */
	UFUNCTION(BlueprintCallable, Category = Logic)
	void SetCommand(ELogicControlType NewCommand);
	
	/** Get the command type for this unit icon */
	UFUNCTION(BlueprintCallable, Category = Logic)
	ELogicControlType GetCommand() const;
	
	/** Set the command type for this unit icon */
	UFUNCTION(BlueprintCallable, Category = Logic)
	void SetScale(float NewScale);
	
	/** Get the command type for this unit icon */
	UFUNCTION(BlueprintCallable, Category = Logic)
	float GetScale() const;
	
	/** Set the actor animated or static */
	UFUNCTION(BlueprintCallable, Category = Logic)
	void SetAnimated(bool bNewAnimated);
	
	/** Get the actor animated or static */
	UFUNCTION(BlueprintCallable, Category = Logic)
	bool GetAnimated() const;
	
	/** Get the actual display width (ignoring actor scale) */
	UFUNCTION(BlueprintCallable, Category = Logic)
	float GetWidth() const;
	
	/** Set the actor pivot centered */
	UFUNCTION(BlueprintCallable, Category = Logic)
	void SetCentered(bool bNewCentered);
	
	/** Get the padding of extendable wrappers */
	UFUNCTION(BlueprintCallable, Category = Logic)
	float GetLeftExtentPadding() const;
	
	/** Set the width extent in units */
	UFUNCTION(BlueprintCallable, Category = Logic)
	void SetInnerExtent(float NewInnerExtent);

	/** Whether the holding command supports nesting */
	bool SupportsNesting() const;

	/** Initialize self destruct on actor or component */
	void InitSelfDestruct();

	void SetVisibility(float VisibilityAmount) const;

	void SetColor(FLinearColor Color) const;

	uint8 Layer = 0;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	void TickProc_Visibility(uint8 &UnfinishedProcessors);
	void TickProc_InnerExtent(uint8 &UnfinishedProcessors);
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

	/** Command material map */
	UPROPERTY(EditDefaultsOnly, Category = "Logic|Setup")
	UControlCommandSettings* CommandMap;

	/** Static mesh to use for icon plane */
	UPROPERTY(EditDefaultsOnly, Category = "Logic|Setup")
	UStaticMesh* IconMesh;

	/** Relative width for the static icon mesh */
	UPROPERTY(EditDefaultsOnly, Category = "Logic|Setup")
	float MeshWidth;

	/** Relative width for the static icon mesh */
	UPROPERTY(EditDefaultsOnly, Category = "Logic|Setup|Animation")
	UCurveFloat* TransitionCurve;

	/** Relative width for the static icon mesh */
	UPROPERTY(EditDefaultsOnly, Category = "Logic|Setup|Animation", meta=(ClampMin=0, UIMin=1, ClampMax=8, UIMax=8))
	float TransitionTime;

private:
	void SetCommand();
	/** Update the actor after any properties did change */
	void SetterAftermath();

	float GetActualInnerExtent() const;
	
	UPROPERTY(EditAnywhere, Category=Logic)	TEnumAsByte<ELogicControlType> Command;
	UPROPERTY(EditAnywhere, Category=Logic) float Scale;
	UPROPERTY(EditAnywhere, Category=Logic) bool bAnimated;
	UPROPERTY(EditAnywhere, Category=Logic) bool bCentered;
	UPROPERTY(EditAnywhere, Category=Logic, meta=(EditCondition="Drawer.bUseInnerExtent", EditConditionHides = true)) float InnerExtent;
	UPROPERTY() FCommandSettingDrawer Drawer;
	UPROPERTY()	UStaticMeshComponent* Icon;
	UPROPERTY() UMaterialInstanceDynamic* IconMat;

	UPROPERTY() bool bToDelete = false;
	
	FParamAnimator Visibility;
	FParamAnimator InnerExtent_A;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, DisplayName="Actual Width", Category="Logic")
	float _ActualWidth = 0;
	UPROPERTY(VisibleAnywhere, DisplayName="Left Extent Padding", Category="Logic")
	float _LeftPadding = 0;
#endif
};
