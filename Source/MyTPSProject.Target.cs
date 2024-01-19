// MyTPSProject

using UnrealBuildTool;
using System.Collections.Generic;

public class MyTPSProjectTarget : TargetRules
{
	public MyTPSProjectTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;

		ExtraModuleNames.AddRange( new string[] { "MyTPSProject" } );
	}
}
