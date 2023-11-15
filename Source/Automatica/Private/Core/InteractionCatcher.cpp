﻿// 2023 Lukas A. Schopf // thelaumix productions


#include "Core/InteractionCatcher.h"


// Sets default values for this component's properties
UInteractionCatcher::UInteractionCatcher()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
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

	TArray<FHitResult> Hits;
	GetWorld()->SweepMultiByChannel(Hits, GetComponentLocation(), GetComponentLocation(), FQuat::Identity, ECC_GameTraceChannel2, FCollisionShape::MakeSphere(40));

	AControlButton* ClosestButton = nullptr;
	double ClosestDistance = INFINITY;
	
	for(auto Hit: Hits)
	{
		const auto Button = Cast<AControlButton>(Hit.GetActor());
		if (Button == nullptr) continue;

		const double ButtonDistance = FVector::Distance(Button->GetActorLocation(), GetComponentLocation());

		// Ignore if is farther away
		if (ClosestDistance <= ButtonDistance) continue;

		ClosestDistance = ButtonDistance;
		ClosestButton = Button;
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

