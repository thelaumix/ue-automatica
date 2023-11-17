// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Logic/ControlButton.h"
#include "InteractionCatcher.generated.h"


UCLASS(ClassGroup=(Automatica), meta=(BlueprintSpawnableComponent))
class AUTOMATICA_API UInteractionCatcher : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractionCatcher();

	UFUNCTION(BlueprintCallable, Category="Logic|Interaction")
	void TriggerPressOnCatchedButton();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY() AControlButton* CurrentlyTargetedButton;
};
