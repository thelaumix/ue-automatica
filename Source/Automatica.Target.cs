// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class AutomaticaTarget : TargetRules
{
	public AutomaticaTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		
		//GlobalDefinitions.Add("ALLOW_CONSOLE_IN_SHIPPING=1");

		ExtraModuleNames.AddRange( new string[] { "Automatica" } );
	}
}
