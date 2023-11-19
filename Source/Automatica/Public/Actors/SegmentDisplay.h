// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "SegmentDisplay.generated.h"

UCLASS()
class AUTOMATICA_API ASegmentDisplay : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASegmentDisplay();

	UFUNCTION(BlueprintCallable, Category = "Segment Display")
	void SetValue(int Value);

	UFUNCTION(BlueprintCallable, Category = "Segment Display")
	void SetNullerDimness(double Dimness);

	UFUNCTION(BlueprintCallable, Category = "Segment Display")
	void SetDigits(uint8 NewDigits);

	UFUNCTION(BlueprintCallable, Category = "Segment Display")
	void SetColor(FLinearColor Color);

protected:

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere, Category = "Segment Display", meta=(ClampMin = 0, UIMin = 0, ClampMax = 8, UIMax = 8))
	uint8 Digits;

	UPROPERTY(EditAnywhere, Category = "Segment Display")
	int InitialValue;

	UPROPERTY(EditAnywhere, Category = "Segment Display")
	bool bZeroPadded;
	
	UPROPERTY(EditAnywhere, Category = "Setup", meta=(ClampMin = 0, UIMin = 0, ClampMax = 1, UIMax = 1))
	double NullerDimness;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UFont* SegmentFont;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UMaterialInterface* SegmentFontMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	double DigitOffset;

private:

	void SetUpTextComp(UTextRenderComponent* Comp) const;
	
	UPROPERTY()	USceneComponent* TextContainer;
	UPROPERTY()	UTextRenderComponent* Nuller;
	UPROPERTY()	TArray<UTextRenderComponent*> Texts;
	UPROPERTY()	int CurrentValue;
	UPROPERTY()	FLinearColor CurrentColor;
};
