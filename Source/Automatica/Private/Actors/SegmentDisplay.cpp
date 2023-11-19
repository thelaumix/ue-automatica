// 2023 Lukas A. Schopf // thelaumix productions


#include "Actors/SegmentDisplay.h"


// Sets default values
ASegmentDisplay::ASegmentDisplay()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitialValue = 0;
	Digits = 2;
	DigitOffset = 0.897125;
	NullerDimness = 0.1;
	bZeroPadded = false;

	CurrentColor = FColor::White;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Display"));

	TextContainer = CreateDefaultSubobject<USceneComponent>(TEXT("Digits"));
	TextContainer->SetupAttachment(RootComponent);

	Nuller = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Nuller"));
	Nuller->SetupAttachment(TextContainer);
}

void ASegmentDisplay::SetValue(const int Value)
{
	CurrentValue = Value;
	int AvailableDigits = Digits;
	const bool bNegative = Value < 0;

	if (bNegative)
		AvailableDigits--;

	int AbsValue = bNegative ? (Value * -1) : Value;

	AbsValue = FMath::Min(AbsValue, FMath::Pow(10, AvailableDigits * 1.0) - 1);

	FString Text = "";
	int i=0;
	for (i=0; i < AvailableDigits; i++)
	{
		const int Power = FMath::Pow(10, i * 1.0);
		if (bZeroPadded && i > 0 && Power > AbsValue)
			break;
		const int BaseVal = FMath::FloorToInt(AbsValue / Power * 1.0) % 10;
		Texts[i]->SetText(FText::FromString(FString::FromInt(BaseVal)));
	}

	if (bNegative)
		Texts[i++]->SetText(FText::FromString("-"));

	
	for (; i < Digits; i++)
	{
		Texts[i]->SetText(FText::FromString(""));
	}
}

void ASegmentDisplay::SetNullerDimness(const double Dimness)
{
	NullerDimness = Dimness;
	SetColor(CurrentColor);
}

void ASegmentDisplay::SetDigits(const uint8 NewDigits)
{
	Digits = NewDigits;
	SetValue(CurrentValue);
}

void ASegmentDisplay::SetColor(FLinearColor Color)
{
	CurrentColor = Color;
	
	Nuller->SetTextRenderColor(FLinearColor::LerpUsingHSV(FColor::Black, CurrentColor, NullerDimness).ToFColor(false));

	for(const auto Texel: Texts)
		Texel->SetTextRenderColor(CurrentColor.ToFColor(false));
}

void ASegmentDisplay::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetUpTextComp(Nuller);
	Texts.Empty();
	FString NullerValue = "";

	Nuller->SetRelativeLocation(FVector(-.1, 0, 0));

	for(uint8 i = 0; i < Digits; i++)
	{
		NullerValue += "8";
		FTransform NewTrans(FVector(0, DigitOffset * i, 0));
		auto Tx = Cast<UTextRenderComponent>(AddComponentByClass(UTextRenderComponent::StaticClass(), true, NewTrans, false));
		//auto Tx = CreateDefaultSubobject<UTextRenderComponent>(FName(TEXT("Digit_") + FString::FromInt(i)));
		Tx->AttachToComponent(TextContainer, FAttachmentTransformRules::KeepRelativeTransform);
		SetUpTextComp(Tx);

		Tx->SetText(FText::FromString("0"));

		Texts.Add(Tx);
	}

	Nuller->SetText(FText::FromString(NullerValue));

	SetValue(InitialValue);
	SetColor(CurrentColor);
}

void ASegmentDisplay::SetUpTextComp(UTextRenderComponent* Comp) const
{
	if (SegmentFont)
		Comp->SetFont(SegmentFont);
	if (SegmentFontMaterial)
		Comp->SetMaterial(0, SegmentFontMaterial);

	Comp->HorizontalAlignment = EHTA_Right;
	Comp->VerticalAlignment = EVRTA_TextTop;
}

