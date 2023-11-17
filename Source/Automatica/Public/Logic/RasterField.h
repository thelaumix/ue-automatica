// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "LogicActor.h"
#include "RasterField.generated.h"

UENUM(BlueprintType)
enum ERasterFieldType: uint8
{
	Normal		= 0,
	Lift		= 1,
	OnFire		= 2,
	Beet		= 3
};

UCLASS()
class AUTOMATICA_API ARasterField : public ALogicActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARasterField();

	UFUNCTION(BlueprintImplementableEvent, Category="Raster Field")
	void SetFireActive(bool bActive);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Raster Field")
	TEnumAsByte<ERasterFieldType> Type;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Raster Field", meta=(EditCondition="Type==1", EditConditionHides=true))
	bool bLiftStartElevated;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Raster Field", meta=(EditCondition="Type==2", EditConditionHides=true))
	bool bFireActive;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
