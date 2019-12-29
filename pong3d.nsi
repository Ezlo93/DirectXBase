!include "MUI.nsh"

Name "Pong3D Setup"

OutFile "pong3d_setup.exe"
InstallDir $PROGRAMFILES64\Pong3D

!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

Section ""
  SetOutPath $INSTDIR
  File ".\x64\Release\pong3d.exe"
  File /r data
  WriteUninstaller $INSTDIR\uninstall.exe
SectionEnd

Section "Uninstall"
  Delete $INSTDIR\uninstall.exe
  Delete $INSTDIR\pong3d.exe
  RMDir /R $INSTDIR\data
  RMDir $INSTDIR
SectionEnd