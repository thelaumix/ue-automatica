// 2023 Lukas A. Schopf // thelaumix productions


#include "Core/CAutomatica.h"

#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
#include "Actors/General/Elevator.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Logic/RasterCharacter.h"

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

FLinearColor UAutomatica::GetChannelColor(int32 Channel)
{
	if (Channel == Red)
		return FLinearColor(1, 0, 0.21374);
	if (Channel == Green)
		return FLinearColor(0, 0.739583, 0.1457);
	if (Channel == Blue)
		return FLinearColor(0, 0.078477, 0.739583);
	if (Channel == (Blue | Red))
		return FLinearColor(0.588911, 0, 0.739583);
	if (Channel == (Red | Green))
		return FLinearColor(0.833333, 0.514311, 0);
	if (Channel == (Blue | Green))
		return FLinearColor(0, 0.833333, 0.750738);

	return FLinearColor(1, 1, 1);
}

bool UAutomatica::BindLogicActor(ALogicActor* LogicActor)
{
	UAutomatica* A;
	if (!Get(LogicActor, A))
		return false;

	if (A->RegisteredLogicNetworkActors.Contains(LogicActor))
		return false;

	// Put all characters at the end
	if (LogicActor->IsA(ARasterCharacter::StaticClass()))
		A->RegisteredLogicNetworkActors.Add(LogicActor);
	else
		A->RegisteredLogicNetworkActors.Insert(LogicActor, 0);
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

bool UAutomatica::GSendLogicCommand(const UObject* Outer, const ELogicControlType Command, const int32 Channel)
{
	UAutomatica* A;
	if (!Get(Outer, A))
		return false;

	A->SendLogicCommand(Command, Channel);

	return true;
}

bool UAutomatica::SendLogicCommand(const ELogicControlType Command, const int32 Channel)
{
	if (RegisteredLogicNetworkActors.Num() == 0) return false;
	
	for(auto Actor: RegisteredLogicNetworkActors)
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

void UAutomatica::PlayEnableSound(UAutomatica* A, const bool bWasReset)
{
	// Reset all
	int NotInGoal = 0;
	if (bWasReset)
	{
		for(auto Actor: A->RegisteredLogicNetworkActors)
		{
			ARasterCharacter* Char = Cast<ARasterCharacter>(Actor);
			if (Char && !Char->IsInGoal())
				NotInGoal++;
		}
	}

	if (!bWasReset || NotInGoal > 0)
	{
		UFMODEvent* Event = LoadObject<UFMODEvent>(A, TEXT("/Game/FMOD/Events/Glob/Unlock_Actions.Unlock_Actions"));
		UFMODBlueprintStatics::PlayEvent2D(A, Event, true);
		
		for(auto Actor: A->RegisteredLogicNetworkActors)
		{
			Actor->LogicReset();
		}

		if (bWasReset)
			A->OnSequenceEnd.Broadcast(false);
	} else if (bWasReset)
	{
		// RE-ENABLE when in goal
		SetInteractionEnabled(A, false);
		UFMODEvent* Event = LoadObject<UFMODEvent>(A, TEXT("/Game/FMOD/Events/Glob/Success.Success"));
		UFMODBlueprintStatics::PlayEvent2D(A, Event, true);
		// Handle goaling
		A->OnSequenceEnd.Broadcast(true);
	}
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
		PlayEnableSound(A, true);
}

void UAutomatica::Elev_TravelToFloor(int Floor)
{
	AElevator::BP_GetElevator(this)->TravelToFloor(Floor);
}

void UAutomatica::Elev_ApproachFloor(int Floor)
{
	AElevator::BP_GetElevator(this)->ApproachFloor(Floor);
}
