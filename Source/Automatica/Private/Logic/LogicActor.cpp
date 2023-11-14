// 2023 Lukas A. Schopf // thelaumix productions


#include "Logic/LogicActor.h"
#include "Core/CAutomatica.h"


void ALogicActor::PushCommandToReceive(ELogicControlType Command)
{
	OnReceiveCommand(Command);
	Bp_OnReceiveCommand(Command);
}

void ALogicActor::BeginPlay()
{
	UAutomatica::BindLogicActor(this);
	Super::BeginPlay();
}

void ALogicActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UAutomatica::ReleaseLogicActor(this);
	Super::EndPlay(EndPlayReason);
}

void ALogicActor::SendCommand(ELogicControlType Command)
{
	UAutomatica::SendLogicCommand(this, ChannelOut, Command);
}
