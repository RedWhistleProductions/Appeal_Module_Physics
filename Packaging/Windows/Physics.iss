#define ModuleName "Physics"
#define ModuleVersion "1.0.0-beta.1"

[Setup]
AppId={{1CDA6F01-47AC-4F75-A460-BA673068F009}
AppName=Appeal {#ModuleName} Module
AppVersion={#ModuleVersion}
AppPublisher=Red Whistle
DefaultDirName={localappdata}\Programs\Appeal Modules\{#ModuleName}
DefaultGroupName=Appeal Modules
DisableProgramGroupPage=yes
OutputDir=..\..\Dist
OutputBaseFilename=Appeal-{#ModuleName}-Module-{#ModuleVersion}-windows-x64-installer
Compression=lzma
SolidCompression=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=lowest
UninstallDisplayName=Appeal {#ModuleName} Module
LicenseFile=..\..\LICENSE

[Files]
Source: "..\..\{#ModuleName}.dll"; DestDir: "{code:GetAppealResources}\Modules\{#ModuleName}"; Flags: ignoreversion

[Dirs]
Name: "{code:GetAppealResources}\Modules\{#ModuleName}"

[Code]
function GetAppealResources(Param: String): String;
var
  OverridePath: String;
begin
  OverridePath := ExpandConstant('{param:APPEALRESOURCES|}');
  if OverridePath <> '' then
    Result := OverridePath
  else
    Result := ExpandConstant('{userdocs}\Appeal\Resources');
end;
