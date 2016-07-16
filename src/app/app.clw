; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CAboutDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "app.h"
LastPage=0

ClassCount=6
Class1=CAppApp
Class2=CAppDoc
Class3=CAppView
Class4=CMainFrame

ResourceCount=6
Resource1=IDR_MAINFRAME (English (U.S.))
Class5=CAboutDlg
Resource2=IDR_MAINFRAME
Class6=CSelectRangeDialog
Resource3=IDD_ABOUTBOX
Resource4=IDD_ABOUTBOX (English (U.S.))
Resource5=IDD_SELECTRANGEDIALOG
Resource6=IDD_SELECTRANGEDIALOG (English (U.S.))

[CLS:CAppApp]
Type=0
HeaderFile=app.h
ImplementationFile=app.cpp
Filter=N
LastObject=CAppApp

[CLS:CAppDoc]
Type=0
HeaderFile=appDoc.h
ImplementationFile=appDoc.cpp
Filter=N
BaseClass=CDocument
VirtualFilter=DC
LastObject=CAppDoc

[CLS:CAppView]
Type=0
HeaderFile=appView.h
ImplementationFile=appView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=CAppView


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
BaseClass=CFrameWnd
VirtualFilter=fWC
LastObject=CMainFrame




[CLS:CAboutDlg]
Type=0
HeaderFile=app.cpp
ImplementationFile=app.cpp
Filter=D
LastObject=CAboutDlg
BaseClass=CDialog
VirtualFilter=dWC

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=10
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_OPEN
Command2=ID_FILE_MRU_FILE1
Command3=ID_APP_EXIT
Command4=ID_VIEW_VERTICALZOOMIN
Command5=ID_VIEW_VERTICALZOOMOUT
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
Command8=ID_FUNCTIONS_SELECTRANGE
Command9=ID_PLAY
Command10=ID_STOP
Command11=ID_FUNCTIONS_FFTTESTER
Command12=ID_FUNCTIONS_FFTTOGRAPH
Command13=ID_FUNCTIONS_TESTLINEGRAPH
Command14=ID_FUNCTIONS_TESTSINEWAVE
Command15=ID_FUNCTIONS_TESTFILECONVERSION
Command16=ID_APP_ABOUT
CommandCount=16

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_EDIT_COPY
Command2=ID_VIEW_VERTICALZOOMOUT
Command3=ID_FILE_NEW
Command4=ID_FILE_OPEN
Command5=ID_FILE_SAVE
Command6=ID_VIEW_VERTICALZOOMIN
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_NEXT_PANE
Command11=ID_PREV_PANE
Command12=ID_EDIT_COPY
Command13=ID_EDIT_PASTE
Command14=ID_EDIT_CUT
Command15=ID_EDIT_UNDO
CommandCount=15

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_PLAY
Command3=ID_STOP
Command4=ID_VIEW_VERTICALZOOMIN
Command5=ID_VIEW_VERTICALZOOMOUT
Command6=ID_APP_ABOUT
CommandCount=6

[CLS:CSelectRangeDialog]
Type=0
HeaderFile=SelectRangeDialog.h
ImplementationFile=SelectRangeDialog.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC

[DLG:IDD_SELECTRANGEDIALOG]
Type=1
Class=CSelectRangeDialog
ControlCount=17
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_EDITSTARTHOURS,edit,1350631552
Control4=IDC_EDITSTARTMIN,edit,1350631552
Control5=IDC_EDITSTARTSEC,edit,1350631552
Control6=IDC_EDITSTARTFRAMES,edit,1350631552
Control7=IDC_EDITENDHOURS,edit,1350631552
Control8=IDC_EDITENDMIN,edit,1350631552
Control9=IDC_EDITENDSEC,edit,1350631552
Control10=IDC_EDITENDFRAMES,edit,1350631552
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_EDITLENGTH,edit,1350633600
Control17=IDC_STATIC,button,1342177287

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=9
Control1=IDC_STATIC,static,1342308480
Control2=IDC_STATIC,static,1342308352
Control3=IDOK,button,1342373889
Control4=IDC_HYPERLINK2,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_HYPERLINK1,static,1342308352

[DLG:IDD_SELECTRANGEDIALOG (English (U.S.))]
Type=1
Class=?
ControlCount=17
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_EDITSTARTHOURS,edit,1350631552
Control4=IDC_EDITSTARTMIN,edit,1350631552
Control5=IDC_EDITSTARTSEC,edit,1350631552
Control6=IDC_EDITSTARTFRAMES,edit,1350631552
Control7=IDC_EDITENDHOURS,edit,1350631552
Control8=IDC_EDITENDMIN,edit,1350631552
Control9=IDC_EDITENDSEC,edit,1350631552
Control10=IDC_EDITENDFRAMES,edit,1350631552
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_EDITLENGTH,edit,1350633600
Control17=IDC_STATIC,button,1342177287

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_FILE_MRU_FILE1
Command3=ID_APP_EXIT
Command4=ID_VIEW_VERTICALZOOMIN
Command5=ID_VIEW_VERTICALZOOMOUT
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
Command8=ID_FUNCTIONS_SELECTRANGE
Command9=ID_PLAY
Command10=ID_STOP
Command11=ID_APP_ABOUT
CommandCount=11

[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_PLAY
Command3=ID_STOP
Command4=ID_VIEW_VERTICALZOOMIN
Command5=ID_VIEW_VERTICALZOOMOUT
Command6=ID_APP_ABOUT
CommandCount=6

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_EDIT_COPY
Command2=ID_VIEW_VERTICALZOOMIN
Command3=ID_FILE_NEW
Command4=ID_FILE_OPEN
Command5=ID_FILE_SAVE
Command6=ID_VIEW_VERTICALZOOMOUT
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_NEXT_PANE
Command11=ID_PREV_PANE
Command12=ID_EDIT_COPY
Command13=ID_EDIT_PASTE
Command14=ID_EDIT_CUT
Command15=ID_EDIT_UNDO
CommandCount=15

