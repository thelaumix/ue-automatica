// 2023 Lukas A. Schopf // thelaumix productions


#include "..\..\Public\Utility\ParamAnimator.h"

#include "Core/CAutomatica.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"


FParamAnimator::FParamAnimator()
{
	Time = 1;
	Interpolator = nullptr;
	bUseUnscaled = true;
	OuterContext = nullptr;
}

FParamAnimator::FParamAnimator(UObject* Outer)
{
	Time=1;
	Interpolator = nullptr;
	bUseUnscaled = true;
	OuterContext = Outer;
}

FParamAnimator::FParamAnimator(UObject* Outer, const float AnimationTime, bool bUnscaled)
{
	Time = AnimationTime;
	Interpolator = nullptr;
	bUseUnscaled = bUnscaled;
	OuterContext = Outer;
}

FParamAnimator::FParamAnimator(UObject* Outer, UCurveFloat* InterpolationCurve, bool bUnscaled)
{
	Time = 1;
	Interpolator = InterpolationCurve;
	bUseUnscaled = bUnscaled;
	OuterContext = Outer;
}

FParamAnimator::FParamAnimator(UObject* Outer, const float AnimationTime, UCurveFloat* InterpolationCurve, const bool bUnscaled)
{
	Time = AnimationTime;
	Interpolator = InterpolationCurve;
	bUseUnscaled = bUnscaled;
	OuterContext = Outer;
}

FParamAnimator::~FParamAnimator()
{
	
}

void FParamAnimator::OverrideValue(const double Value)
{
	From = Value;
	To = Value;
}

void FParamAnimator::SetTime(const double NewTime)
{
	Time = NewTime;
}

void FParamAnimator::SetInterpolator(UCurveFloat* NewInterpolator)
{
	Interpolator = NewInterpolator;
}

void FParamAnimator::Set(const double Target)
{
	StartingTime = GetCtxTime();
	From = To;
	To = Target;
}

double FParamAnimator::Get() const
{
	const double Elapsed = GetCtxTime() - StartingTime;
	const double Percentage = FMath::Clamp(Elapsed / Time, 0, 1);

	if (Interpolator)
		return FMath::Lerp(From, To, Interpolator->GetFloatValue(Percentage));
	return FMath::Lerp(From, To, Percentage);
}

bool FParamAnimator::Get(double& Value)
{
	Value = Get();
	if (Value == LastFetched)
		return false;

	LastFetched = Value;
	return true;
}

double FParamAnimator::GetTarget() const
{
	return To;
}

bool FParamAnimator::IsIdle() const
{
	const double Elapsed = GetCtxTime() - StartingTime;
	return Elapsed >= Time;
}

double FParamAnimator::GetCtxTime() const
{
	if (OuterContext == nullptr)
	{
		UE_LOGFMT(AutomaticaCore, Error, "Missing context on parameter animator");
		return -1;
	}
	
	if (bUseUnscaled)
		return UGameplayStatics::GetRealTimeSeconds(OuterContext);
	
	return UGameplayStatics::GetTimeSeconds(OuterContext);
}
