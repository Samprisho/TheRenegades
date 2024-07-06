// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TheRenegades : ModuleRules
{
	public TheRenegades(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayTags",
			"UMG",
			"PBCharacterMovement"
		});
	}
}
