// 2023 Lukas A. Schopf // thelaumix productions


#include "Core/InteractionCatcher.h"

#include "Core/CAutomatica.h"


// Sets default values for this component's properties
UInteractionCatcher::UInteractionCatcher()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionCatcher::TriggerPressOnCatchedButton()
{
	if (CurrentlyTargetedButton)
		CurrentlyTargetedButton->Press();
}


// Called when the game starts
void UInteractionCatcher::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInteractionCatcher::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	AControlButton* ClosestButton = nullptr;
	double ClosestDistance = INFINITY;
	double ClosestHitDistance = INFINITY;

	if (UAutomatica::IsInteractionEnabled(this))
	{
		TArray<FHitResult> Hits;
		
		GetWorld()->SweepMultiByChannel(Hits, GetComponentLocation(), GetComponentLocation() + GetForwardVector() * 1000, FQuat::Identity, ECC_GameTraceChannel2, FCollisionShape::MakeSphere(5));

		for(auto Hit: Hits)
		{
			const auto Button = Cast<AControlButton>(Hit.GetActor());
			if (Button == nullptr) continue;

			const double ButtonDistance = FVector::Distance(Button->GetActorLocation(), GetComponentLocation());

			// Ignore if is farther away
			if (ClosestDistance <= ButtonDistance) continue;

			ClosestDistance = ButtonDistance;
			ClosestHitDistance = Hit.Distance;
			ClosestButton = Button;
		}

		FColor Col = FColor::Orange;
		if (ClosestButton)
		{
			//DrawDebugSphere(GetWorld(), GetComponentLocation() + GetForwardVector() * ClosestHitDistance, 5, 5, FColor::Orange);
			Col = FColor::Green;
		}
		
		DrawDebugLine(GetWorld(), GetComponentLocation(), GetComponentLocation() + GetForwardVector() * 1000, Col, false, -1, 0, 2);
	}

	if (CurrentlyTargetedButton != ClosestButton)
	{
		if (CurrentlyTargetedButton != nullptr)
			CurrentlyTargetedButton->InteractSetup_Release(this);
		if (ClosestButton != nullptr)
			ClosestButton->InteractSetup_Bind(this);

		CurrentlyTargetedButton = ClosestButton;
	}

	
}

