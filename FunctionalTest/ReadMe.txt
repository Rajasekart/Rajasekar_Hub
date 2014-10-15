CyberSwitching Functional Test Application V1.04
------------------------------------------------
1. Updated to retry the command to fetch Voltage value for three times if it fails.
2. Updated to retry the command to switch the outlet state for three times if it fails.

CyberSwitching Functional Test Application V1.03
------------------------------------------------
1. Updated to fetch and print the Current Value and Voltage Value while ON and OFF Outlets.
2. Updated to log the Error details after each process.

CyberSwitching Functional Test Application V1.02
------------------------------------------------
1. Updated the UI controls and updated functionalities.

================================================================================
    MICROSOFT FOUNDATION CLASS LIBRARY : FunctionalTest Project Overview
===============================================================================

The application wizard has created this FunctionalTest application for
you.  This application not only demonstrates the basics of using the Microsoft
Foundation Classes but is also a starting point for writing your application.

This file contains a summary of what you will find in each of the files that
make up your FunctionalTest application.

FunctionalTest.vcproj
    This is the main project file for VC++ projects generated using an application wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    application wizard.

FunctionalTest.h
    This is the main header file for the application.  It includes other
    project specific headers (including Resource.h) and declares the
    CFunctionalTestApp application class.

FunctionalTest.cpp
    This is the main application source file that contains the application
    class CFunctionalTestApp.

FunctionalTest.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
    Visual C++. Your project resources are in 1033.

res\FunctionalTest.ico
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file FunctionalTest.rc.

res\FunctionalTest.rc2
    This file contains resources that are not edited by Microsoft
    Visual C++. You should place all resources not editable by
    the resource editor in this file.


/////////////////////////////////////////////////////////////////////////////

The application wizard creates one dialog class:

FunctionalTestDlg.h, FunctionalTestDlg.cpp - the dialog
    These files contain your CFunctionalTestDlg class.  This class defines
    the behavior of your application's main dialog.  The dialog's template is
    in FunctionalTest.rc, which can be edited in Microsoft Visual C++.


/////////////////////////////////////////////////////////////////////////////

Other Features:

ActiveX Controls
    The application includes support to use ActiveX controls.

/////////////////////////////////////////////////////////////////////////////

Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named FunctionalTest.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

FunctionalTest.manifest
	Application manifest files are used by Windows XP to describe an applications
	dependency on specific versions of Side-by-Side assemblies. The loader uses this
	information to load the appropriate assembly from the assembly cache or private
	from the application. The Application manifest  maybe included for redistribution
	as an external .manifest file that is installed in the same folder as the application
	executable or it may be included in the executable in the form of a resource.
/////////////////////////////////////////////////////////////////////////////

Other notes:

The application wizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

If your application uses MFC in a shared DLL, you will need
to redistribute the MFC DLLs. If your application is in a language
other than the operating system's locale, you will also have to
redistribute the corresponding localized resources MFC90XXX.DLL.
For more information on both of these topics, please see the section on
redistributing Visual C++ applications in MSDN documentation.

/////////////////////////////////////////////////////////////////////////////
