// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class GhostController : ModuleRules
{
	public GhostController(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"Sockets",
			"Networking"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
