// MyTPSProject

using UnrealBuildTool;
using System.Collections.Generic;

public class MyTPSProjectEditorTarget : TargetRules
{
	public MyTPSProjectEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;

		ExtraModuleNames.AddRange( new string[] { "MyTPSProject" } );
	}
}
