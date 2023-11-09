// 2023 Lukas A. Schopf // thelaumix productions


#include "Core/CAutomatica.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(AutomaticaCore);

void UAutomatica::Init()
{
	Super::Init();

	if (UGameplayStatics::DoesSaveGameExist(SAVE_NAME, 0))
	{
		Saved = Cast<UGameProgress>(UGameplayStatics::LoadGameFromSlot(SAVE_NAME, 0));
		UE_LOGFMT(AutomaticaCore, Warning, "Savegame read");
	} else
	{
		Saved = Cast<UGameProgress>(UGameplayStatics::CreateSaveGameObject(UGameProgress::StaticClass()));
		UE_LOGFMT(AutomaticaCore, Warning, "Savegame created");
	}
}

UGameProgress* UAutomatica::GetProgress(UObject* Outer, bool& Valid)
{
	if (const auto A = Get(Outer))
	{
		Valid = true;
		return A->Saved;
	}
	Valid = false;
	return nullptr;
}

bool UAutomatica::WriteProgress(UObject* Outer)
{
	const auto A = Get(Outer);
	if (A == nullptr)
		return false;

	return UGameplayStatics::SaveGameToSlot(A->Saved, SAVE_NAME, 0);
}

UAutomatica* UAutomatica::Get(const UObject* Outer)
{
	return Cast<UAutomatica>(UGameplayStatics::GetGameInstance(Outer));
}
