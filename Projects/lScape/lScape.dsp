# Microsoft Developer Studio Project File - Name="lScape" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=lScape - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lScape.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lScape.mak" CFG="lScape - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lScape - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "lScape - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lScape - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../glBase" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "lScape - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../glBase" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "lScape - Win32 Release"
# Name "lScape - Win32 Debug"
# Begin Group "glBase"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\glBase\boundbox.c
# End Source File
# Begin Source File

SOURCE=..\glBase\boundbox.h
# End Source File
# Begin Source File

SOURCE=..\glBase\cache.c
# End Source File
# Begin Source File

SOURCE=..\glBase\cache.h
# End Source File
# Begin Source File

SOURCE=..\glBase\callback.c
# End Source File
# Begin Source File

SOURCE=..\glBase\callback.h
# End Source File
# Begin Source File

SOURCE=..\glBase\camera.c
# End Source File
# Begin Source File

SOURCE=..\glBase\camera.h
# End Source File
# Begin Source File

SOURCE=..\glBase\error.c
# End Source File
# Begin Source File

SOURCE=..\glBase\error.h
# End Source File
# Begin Source File

SOURCE=..\glBase\freeCamera.c
# End Source File
# Begin Source File

SOURCE=..\glBase\freeCamera.h
# End Source File
# Begin Source File

SOURCE=..\glBase\frustum.c
# End Source File
# Begin Source File

SOURCE=..\glBase\frustum.h
# End Source File
# Begin Source File

SOURCE=..\glBase\general.h
# End Source File
# Begin Source File

SOURCE=..\glBase\main.c
# End Source File
# Begin Source File

SOURCE=..\glBase\main.h
# End Source File
# Begin Source File

SOURCE=..\glBase\opengl.c
# End Source File
# Begin Source File

SOURCE=..\glBase\opengl.h
# End Source File
# Begin Source File

SOURCE=..\glBase\pbuffer.c
# End Source File
# Begin Source File

SOURCE=..\glBase\pbuffer.h
# End Source File
# Begin Source File

SOURCE=..\glBase\pic.c
# End Source File
# Begin Source File

SOURCE=..\glBase\pic.h
# End Source File
# Begin Source File

SOURCE=..\glBase\plane.c
# End Source File
# Begin Source File

SOURCE=..\glBase\plane.h
# End Source File
# Begin Source File

SOURCE=..\glBase\textureFont.c
# End Source File
# Begin Source File

SOURCE=..\glBase\textureFont.h
# End Source File
# Begin Source File

SOURCE=..\glBase\textureManager.c
# End Source File
# Begin Source File

SOURCE=..\glBase\textureManager.h
# End Source File
# Begin Source File

SOURCE=..\glBase\tgafile.c
# End Source File
# Begin Source File

SOURCE=..\glBase\tgafile.h
# End Source File
# Begin Source File

SOURCE=..\glBase\vector.c
# End Source File
# Begin Source File

SOURCE=..\glBase\vector.h
# End Source File
# Begin Source File

SOURCE=..\glBase\viewpoint.c
# End Source File
# Begin Source File

SOURCE=..\glBase\viewpoint.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\app.c
# End Source File
# Begin Source File

SOURCE=.\gpl.txt
# End Source File
# Begin Source File

SOURCE=.\lscape.c
# End Source File
# Begin Source File

SOURCE=.\lscape.h
# End Source File
# Begin Source File

SOURCE=.\lscollide.c
# End Source File
# Begin Source File

SOURCE=.\lsrender.c
# End Source File
# Begin Source File

SOURCE=.\lstexture.c
# End Source File
# Begin Source File

SOURCE=.\lsvis.c
# End Source File
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# End Target
# End Project
