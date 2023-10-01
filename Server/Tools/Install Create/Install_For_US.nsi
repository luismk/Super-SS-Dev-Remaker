; Instalador do PangYa Brasil Season 8
; Atualização 31/08/2022 12:03


#Configurações

!include "MUI.nsh"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\nsis.bmp"
!define MUI_ABORTWARNING
!insertmacro MUI_PAGE_LICENSE "${NSISDIR}\license.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_LANGUAGE "PortugueseBR"

#Descrições

Name "PangYa_Setup_BR.R7.825"
BrandingText "PangYa"
OutFile ".\PangYa_Setup_BR.R7.825.exe"
InstallDir "$PROGRAMFILES\NtreevSoft\PangYa_BR_EN"


Icon "${NSISDIR}\contrib\graphics\icons\pixel-install.ico"
UninstallIcon "${NSISDIR}\contrib\graphics\icons\pixel-uninstall.ico"
 
 
#Seleção de Arquivos

Section "Arquivos do jogo"
SetOutPath "$INSTDIR"
File "bs_notice_popup01.jpg"
File "bs_ppl.ini"
File "d3dx9_31.dll"
File "dbghelp.dll"
File "Disco_Notice.jpg"
File "english.dat"
File "event01.jpg"
File "event02.jpg"
File "event_notice.ini"
File "GameGuard.des"
File "ijl15.dll"
File "korea.dat"
File "LoadingRes.dll"
File "Mss32.dll"
File "msvcp100.dll"
File "msvcp110.dll"
File "msvcp60.dll"
File "msvcp71.dll"
File "msvcr100.dll"
File "msvcr71.dll"
File "notice_popup.ini"
File "notice_popup00.jpg"
File "notice_popup01.jpg"
File "notice_popup02.jpg"
File "notice_popup03.jpg"
File "notice_popup04.jpg"
File "notice_popup05.jpg"
File "notice_popup06.jpg"
File "notice_popup07.jpg"
File "PangyaSetup.bmp"
File "PangyaUS.ini"
File "pcbang.ini"
File "ppl.ini"
File "ProjectG.exe"
File "projectg700gb+.pak"
File "projectg701gb.pak"
File "projectg702gb.pak"
File "projectg703gb.pak"
File "projectg704gb.pak"
File "projectg705gb.pak"
File "projectg706gb.pak"
File "projectg707gb.pak"
File "projectg708gb.pak"
File "projectg709gb.pak"
File "projectg710gb.pak"
File "projectg711gb.pak"
File "projectg712gb.pak"
File "projectg713gb.pak"
File "projectg714gb.pak"
File "projectg715gb.pak"
File "projectg716gb.pak"
File "projectg717gb.pak"
File "projectg718gb.pak"
File "projectg719gb.pak"
File "projectg720gb.pak"
File "projectg721gb.pak"
File "projectg722gb.pak"
File "projectg723gb.pak"
File "projectg724gb.pak"
File "projectg725gb.pak"
File "projectg726gb.pak"
File "projectg727gb.pak"
File "projectg728gb.pak"
File "projectg729gb.pak"
File "projectg730gb.pak"
File "projectg801gb.pak"
File "projectg802gb.pak"
File "projectg803gb.pak"
File "projectg804gb.pak"
File "projectg805gb.pak"
File "projectg806gb.pak"
File "projectg807gb.pak"
File "projectg808gb.pak"
File "projectg809gb.pak"
File "projectg810gb.pak"
File "projectg811gb.pak"
File "projectg812gb.pak"
File "projectg813gb.pak"
File "projectg813gba.pak"
File "projectg814gb.pak"
File "projectg815gb.pak"
File "projectg816gb.pak"
File "projectg817gb.pak"
File "projectg818gb.pak"
File "projectg819gb.pak"
File "projectg820gb.pak"
File "projectg820gba.pak"
File "projectg821gb.pak"
File "projectg822gb.pak"
File "projectg823gb.pak"
File "projectg824gb.pak"
File "projectg824gba.pak"
File "projectg825gb.pak"
File "ScreenCape.dll"
File "update.exe"
File "wangreal.dll"
SetOutPath "$INSTDIR\mss\"
File "mss\mssa3d.m3d"
File "mss\mssdolby.flt"
File "mss\mssds3d.flt"
File "mss\mssds3d.m3d"
File "mss\mssdsp.flt"
File "mss\mssdx7.m3d"
File "mss\msseax.flt"
File "mss\msseax.m3d"
File "mss\mssmp3.asi"
File "mss\mssrsx.m3d"
File "mss\msssoft.m3d"
File "mss\msssrs.flt"
File "mss\mssvoice.asi"
SetOutPath "$INSTDIR\score_ppl\"
File "score_ppl\score_notice_0.tga"
File "score_ppl\score_notice_1.tga"
File "score_ppl\score_notice_2.tga"
File "score_ppl\score_notice_3.tga"
File "score_ppl\score_notice_4.tga"
File "score_ppl\score_notice_5.tga"
File "score_ppl\score_notice_6.tga"
File "score_ppl\score_notice_7.tga"
File "score_ppl\score_notice_8.tga"
File "score_ppl\score_notice_9.tga"
File "score_ppl\score_notice_10.tga"
File "score_ppl\score_ppl.ini"
SectionEnd


Section "Atalhos no Menu Iniciar"
 SetShellVarContext all
 CreateDirectory "$SMPROGRAMS\PangYa_BR_EN"
 CreateShortCut  "$SMPROGRAMS\PangYa_BR_EN\PangYa BR!.lnk" "$INSTDIR\update.exe"

SectionEnd

Section "Atalho no Desktop"
 CreateShortCut "$DESKTOP\PangYa BR!.lnk" "$INSTDIR\update_br.exe" "" "$INSTDIR\update_br.exe"
SectionEnd

Section "Adicionando funçoes de desinstação no Painel de controle do windows"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\PangYa_BR_EN" "DisplayIcon" "$INSTDIR\update.exe,0"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\PangYa_BR_EN" "DisplayName" "PangYa_BR_EN (NtreevSoft)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\PangYa_BR_EN" "Install_Dir" "$INSTDIR"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\PangYa_BR_EN" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd


  #Desinstalação

  #Excluir os arquivos das pastas do jogo
  Delete "$INSTDIR\*.*"
  Delete "$INSTDIR\GameGuard\*.*"
  Delete "$INSTDIR\mss\*.*"
  Delete "$INSTDIR\emblem\*.*"
  Delete "$INSTDIR\score_ppl\*.*"
  
  #Excluir Pastas do jogo
  RMDir "$INSTDIR\GameGuard"
  RMDir "$INSTDIR\mss"
  RMDir "$INSTDIR\emblem"
  RMDir "$INSTDIR\score_ppl"
  
  #Desistalar icones do menu iniciar
  SetShellVarContext all

  Delete "$SMPROGRAMS\PangYa_BR_EN\PangYa BR!.lnk"

  
  #excluir as pastas do menu iniciar
  RMDir "$SMPROGRAMS\PangYa_BR_EN"
  
  #Desistalar icone do Desktop
  Delete "$DESKTOP\PangYa BR!.lnk"
 
  
SectionEnd

  FunctionEnd


