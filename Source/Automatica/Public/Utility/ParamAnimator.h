// 2023 Lukas A. Schopf // thelaumix productions

#pragma once

#include "CoreMinimal.h"

/**
 * Wrapper for animatable parameters
 */
class AUTOMATICA_API FParamAnimator
{
public:
	FParamAnimator();
	explicit FParamAnimator(UObject* Outer);
	explicit FParamAnimator(UObject* Outer, const float AnimationTime, bool bUnscaled = true);
	explicit FParamAnimator(UObject* Outer, UCurveFloat* InterpolationCurve, bool bUnscaled = true);
	explicit FParamAnimator(UObject* Outer, const float AnimationTime, UCurveFloat* InterpolationCurve, bool bUnscaled = true);
	~FParamAnimator();

	/** Updates the current value. This will override any animation progress. */
	void OverrideValue(double Value);
	/** Updates the time. */
	void SetTime(double NewTime);
	/** Updates the interpolator curve. */
	void SetInterpolator(UCurveFloat* NewInterpolator);
	/** Animates the value towards the given target */
	void Set(double Target);
	/** Fetches the current animation value */
	double Get() const;
	/** Fetches the current animation value if it did changed since the last fetch */
	bool Get(double& Value);
	/** Fetches the targeted value */
	double GetTarget() const;
	/** Whether the animation is currently in idle */
	bool IsIdle() const;

private:
	double Time = 0;
	double StartingTime = 0;
	double From = 0;
	double To = 0;
	double LastFetched = 0;
	bool bUseUnscaled = true;

	double GetCtxTime() const;

	UCurveFloat* Interpolator;
	UObject* OuterContext;
};
