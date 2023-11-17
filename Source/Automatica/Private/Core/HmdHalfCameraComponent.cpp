// 2023 Lukas A. Schopf // thelaumix productions


#include "Automatica/Public/Core/HmdHalfCameraComponent.h"

#include "IXRCamera.h"
#include "IXRTrackingSystem.h"

UHmdHalfCameraComponent::UHmdHalfCameraComponent()
{
	ZOffset = 88;
}

void UHmdHalfCameraComponent::HandleXRCamera()
{
	IXRTrackingSystem* XRSystem = GEngine->XRSystem.Get();
	auto XRCamera = XRSystem->GetXRCamera();

	if (!XRCamera.IsValid())
	{
		return;
	}

	const FTransform ParentWorld = CalcNewComponentToWorld(FTransform());

	XRCamera->SetupLateUpdate(ParentWorld, this, bLockToHmd == 0);

	if (bLockToHmd)
	{
		FQuat Orientation;
		FVector Position;
		if (XRCamera->UpdatePlayerCamera(Orientation, Position))
		{
			FVector HalfHeightPosition = Position;
			//HalfHeightPosition.Z -= ZOffset;
			SetRelativeTransform(FTransform(Orientation, HalfHeightPosition));
		}
		else
		{
			ResetRelativeTransform();
		}
	}

	XRCamera->OverrideFOV(this->FieldOfView);
}
