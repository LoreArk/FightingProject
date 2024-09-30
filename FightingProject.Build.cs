// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FightingProject : ModuleRules
{
	public FightingProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[] { "FightingProject/Public" });
        PrivateIncludePaths.AddRange(new string[] { "FightingProject/Private" });
        PrivateIncludePaths.AddRange(new string[] { "FightingProject/" });

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "GeometryCollectionEngine", "AIModule", "GameplayTasks", "NavigationSystem", "LevelSequence", "MovieScene", "TemplateSequence", "GameplayCameras", "MotionWarping" });
	}
}
