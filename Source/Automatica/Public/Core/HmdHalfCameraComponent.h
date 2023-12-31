﻿// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "HmdHalfCameraComponent.generated.h"

/**
 * 
 */
UCLASS(meta=(BlueprintSpawnableComponent))
class AUTOMATICA_API UHmdHalfCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

	UHmdHalfCameraComponent();

	virtual void HandleXRCamera() override;

public:

	UPROPERTY(EditDefaultsOnly)
	float ZOffset;
};
