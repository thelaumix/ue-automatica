﻿// Fill out your copyright notice in the Description page of Project Settings.

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

	virtual void HandleXRCamera() override;

public:

	float ZOffset = 88;
};
