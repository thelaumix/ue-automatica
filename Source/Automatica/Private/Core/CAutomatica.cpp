// 2023 Lukas A. Schopf // thelaumix productions


#include "Core/CAutomatica.h"

#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
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

bool UAutomatica::BindLogicActor(ALogicActor* LogicActor)
{
	UAutomatica* A;
	if (!Get(LogicActor, A))
		return false;

	if (A->RegisteredLogicNetworkActors.Contains(LogicActor))
		return false;
	A->RegisteredLogicNetworkActors.Add(LogicActor);
	return true;
}

bool UAutomatica::ReleaseLogicActor(ALogicActor* LogicActor)
{
	UAutomatica* A;
	if (!Get(LogicActor, A))
		return false;

	if (!A->RegisteredLogicNetworkActors.Contains(LogicActor))
		return false;
	A->RegisteredLogicNetworkActors.Remove(LogicActor);
	return true;
}

bool UAutomatica::SendLogicCommand(ALogicActor* Sender, int32 Channel, ELogicControlType Command)
{
	UAutomatica* A;
	if (!Get(Sender, A))
		return false;

	for(auto Actor: A->RegisteredLogicNetworkActors)
	{
		// If target actor does not accept input, skip it
		if (!Actor->bUseChannelBusInput)
			continue;

		// If target actor does not accept any of the channels as input, skip
		if ((Channel & Actor->ChannelIn) == 0)
			continue;

		// Otherwise, send command to actor
		Actor->PushCommandToReceive(Command);
	}

	return true;
}

template <typename LogicActorType>
TArray<LogicActorType*> UAutomatica::GetLogicActorsOfType(ALogicActor* Requester)
{
	TArray<LogicActorType*> Matches;
	UAutomatica* A;
	if (!Get(Requester, A))
		return Matches;
	
	for(auto Actor: A->RegisteredLogicNetworkActors)
	{
		LogicActorType* Casted = Cast<LogicActorType>(Actor);
		if (Casted != nullptr)
			Matches.Add(Casted);
	}

	return Matches;
}

TArray<ALogicActor*> UAutomatica::GetRegisteredLogicActors(const ALogicActor* Requester)
{
	UAutomatica* A;
	if (!Get(Requester, A))
		return TArray<ALogicActor*>();

	return A->RegisteredLogicNetworkActors;
}

UAutomatica* UAutomatica::Get(const UObject* Outer)
{
	return Cast<UAutomatica>(UGameplayStatics::GetGameInstance(Outer));
}

bool UAutomatica::Get(const UObject* Outer, UAutomatica*& Instance)
{
	Instance = Get(Outer);
	return Instance != nullptr;
}

UAutomatica* UAutomatica::BP_GetAutomaticaInstance(const UObject* Outer)
{
	return Get(Outer);
}

bool UAutomatica::IsInteractionEnabled(const UObject* Outer)
{
	UAutomatica* A;
	if (!Get(Outer, A))
		return false;

	return A->bInteractionEnabled && A->RunningControlUnits.Num() == 0;
}

void UAutomatica::PlayEnableSound(UAutomatica* A)
{
	UFMODEvent* Event = LoadObject<UFMODEvent>(A, TEXT("/Game/FMOD/Events/Glob/Unlock_Actions.Unlock_Actions"));
	UFMODBlueprintStatics::PlayEvent2D(A, Event, true);
}

void UAutomatica::SetInteractionEnabled(const UObject* Outer, const bool bEnabled)
{
	UAutomatica* A;
	if (!Get(Outer, A))
		return;

	if (A->bInteractionEnabled != bEnabled && bEnabled)
	{
		PlayEnableSound(A);
	}

	A->bInteractionEnabled = bEnabled;
}

void UAutomatica::SetControlUnitPlaying(AControlUnit* Unit, const bool bPlaying)
{
	UAutomatica* A;
	if (!Get(Unit, A))
		return;

	const bool bIntEnabledBefore = IsInteractionEnabled(Unit);
	
	if (bPlaying && !A->RunningControlUnits.Contains(Unit))
		A->RunningControlUnits.Add(Unit);
	else if (!bPlaying && A->RunningControlUnits.Contains(Unit))
		A->RunningControlUnits.Remove(Unit);

	const bool bIntEnabledAfter = IsInteractionEnabled(Unit);

	if (bIntEnabledAfter != bIntEnabledBefore && bIntEnabledAfter)
		PlayEnableSound(A);
}
