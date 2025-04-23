# Microsoft Developer Studio Project File - Name="FOX" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=FOX - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FOX.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FOX.mak" CFG="FOX - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FOX - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FOX - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FOX - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\src" /I "..\..\src\audio" /I "..\..\src\video" /I "..\..\src\video\wincommon" /I "..\..\src\video\windx8" /I "..\..\src\events" /I "..\..\src\joystick" /I "..\..\src\cdrom" /I "..\..\src\thread" /I "..\..\src\thread\win32" /I "..\..\src\timer" /I "..\..\include" /I "..\..\include\FOX" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ENABLE_WINDIB" /D "ENABLE_DIRECTX" /D _WIN32_WINNT=0x0400 /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x415 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 winmm.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /machine:I386

!ELSEIF  "$(CFG)" == "FOX - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "..\..\src" /I "..\..\src\audio" /I "..\..\src\video" /I "..\..\src\video\wincommon" /I "..\..\src\video\windx8" /I "..\..\src\events" /I "..\..\src\joystick" /I "..\..\src\cdrom" /I "..\..\src\thread" /I "..\..\src\thread\win32" /I "..\..\src\timer" /I "..\..\include" /I "..\..\include\FOX" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "ENABLE_WINDIB" /D "ENABLE_DIRECTX" /D _WIN32_WINNT=0x0400 /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "FOX - Win32 Release"
# Name "FOX - Win32 Debug"
# Begin Source File

SOURCE=..\..\src\active.c
# End Source File
# Begin Source File

SOURCE=..\..\src\audio.c
# End Source File
# Begin Source File

SOURCE=..\..\src\audio_c.h
# End Source File
# Begin Source File

SOURCE=..\..\src\audiocvt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\audiomem.c
# End Source File
# Begin Source File

SOURCE=..\..\src\audiomem.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bcursor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\blit.c
# End Source File
# Begin Source File

SOURCE=..\..\src\blit.h
# End Source File
# Begin Source File

SOURCE=..\..\src\cursor.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cursor_c.h
# End Source File
# Begin Source File

SOURCE=..\..\src\dcursor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\directx.h
# End Source File
# Begin Source File

SOURCE=..\..\src\fox.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_blit_0.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_blit_1.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_blit_A.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_blit_N.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_dibaudio.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_dibaudio.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_dibevents.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_dibevents_c.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_dibvideo.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_dibvideo.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_draw2d.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_dx8audio.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_dx8audio.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_dx8events.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_dx8events_c.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_dx8video.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_dx8video.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_dx8yuv.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_dx8yuv.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_endian.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_error.c
# End Source File
# Begin Source File

SOURCE=..\..\Src\FOX_error_c.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_events.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_events_c.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_expose.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_fatal.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_fatal.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_filesys.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_gamma.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_getenv.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_glfuncs.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_keyboard.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_leaks.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_lowvideo.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_memops.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_mixer.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_mouse.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_pixels.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_pixels_c.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_quit.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_resize.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_RLEaccel.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_RLEaccel_c.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_rwops.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_stretch.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_stretch_c.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_surface.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_sysaudio.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_sysevents.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_sysevents.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_sysmouse.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_sysmouse_c.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_sysmutex.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_syssem.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_systhread.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_systhread.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_systhread_c.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_systimer.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_systimer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_sysvideo.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_syswm.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_syswm_c.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_thread.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_thread_c.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_timer.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_timer_c.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_video.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_vkeys.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_wave.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_wave.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_wingl.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_wingl_c.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_yuv.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_yuv_mmx.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_yuv_sw.c
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_yuv_sw_c.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FOX_yuvfuncs.h
# End Source File
# Begin Source File

SOURCE=..\..\src\wmmsg.h
# End Source File
# End Target
# End Project
