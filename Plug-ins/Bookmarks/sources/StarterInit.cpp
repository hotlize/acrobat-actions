/*********************************************************************

 Copyright (C) 2018-2019 Chun Tian (binghe)
 Copyright (C) 1998-2006 Adobe Systems Incorporated
 All rights reserved.

 NOTICE: Adobe permits you to use, modify, and distribute this file
 in accordance with the terms of the Adobe license agreement
 accompanying it. If you have received this file from a source other
 than Adobe, then your use, modification, or distribution of it
 requires the prior written permission of Adobe.

 ---------------------------------------------------------------------

 StarterInit.cpp

 - Skeleton .cpp file for a plug-in. It implements the basic
   handshaking methods required for all plug-ins.

*********************************************************************/

// Acrobat Headers.
#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#endif

#include <cstdio>
#include <cstring>

#include "Functions.hpp"

/** 
  Starter is a plug-in template that provides a minimal
  implementation for a plug-in. Developers may use this plug-in a
  basis for their plug-ins.
 */

/*-------------------------------------------------------
	Constants/Declarations
 -------------------------------------------------------*/
static AVMenuItem topMenuItem = NULL;
static AVMenuItem menuItem[3] = {NULL, NULL, NULL};
static AVMenu subMenu = NULL;

ACCB1 ASBool ACCB2 FindPluginMenu(void);

/*-------------------------------------------------------
	Core Handshake Callbacks
-------------------------------------------------------*/

/* PluginExportHFTs
** ------------------------------------------------------
**/

/**
** Create and register the HFT's.
**
** @return true to continue loading plug-in,
** false to cause plug-in loading to stop.
*/
ACCB1 ASBool ACCB2 PluginExportHFTs(void)
{
    return true;
}

/* PluginImportReplaceAndRegister
** ------------------------------------------------------
** */
/** 
	The application calls this function to allow it to
	<ul>
	<li> Import plug-in supplied HFTs.
	<li> Replace functions in the HFTs you're using (where allowed).
	<li> Register to receive notification events.
	</ul>

	@return true to continue loading plug-in,
	false to cause plug-in loading to stop.
*/
ACCB1 ASBool ACCB2 PluginImportReplaceAndRegister(void)
{
    return true;
}

static const char *pluginMenuName = "Extensions";

/* Find an existing "Plug-ins" menu from the menubar. */
ACCB1 ASBool ACCB2 FindPluginMenu(void)
{
    AVMenubar menubar = AVAppGetMenubar();
    AVMenu volatile commonMenu = NULL;
    ASAtom PluginMenuName = ASAtomFromString(pluginMenuName);

    if (!menubar) return false;

    /* Gets the number of menus in menubar. */
    AVTArraySize nMenu = AVMenubarGetNumMenus(menubar);

    for (AVMenuIndex index = 0; index < nMenu; ++index) {
	/* Acquires the menu with the specified index. */
	AVMenu menu = AVMenubarAcquireMenuByIndex(menubar, index);
	/* get the menu's language-independent name. */
	ASAtom name = AVMenuGetName(menu);
	if (name == PluginMenuName) {
	    AVAlertNote(ASAtomGetString(name));
	}
    }

    return true;
}

/* Collapse All Bookmarks */
ACCB1 void ACCB2 PluginCommand_0(void *clientData)
{
    // try to get front PDF document
    AVDoc avDoc = AVAppGetActiveDoc();
    PDDoc pdDoc = AVDocGetPDDoc(avDoc);
    PDBookmark rootBookmark = PDDocGetBookmarkRoot(pdDoc);

    // visit all bookmarks recursively
    VisitAllBookmarks(pdDoc, rootBookmark);

    return;
}

#define NOTESIZ 200
static char notes[NOTESIZ] = "";

/* Fix FitType of all Bookmarks */
ACCB1 void ACCB2 PluginCommand_1(void *clientData)
{
    // try to get front PDF document
    AVDoc avDoc = AVAppGetActiveDoc();
    PDDoc pdDoc = AVDocGetPDDoc(avDoc);
    PDBookmark rootBookmark = PDDocGetBookmarkRoot(pdDoc);
    int acc = 0;

    // visit all bookmarks recursively, fixing FitView
    acc = FixAllBookmarks(pdDoc, rootBookmark, acc);
#ifdef WIN_PLATFORM
    _snprintf(notes, NOTESIZ, "Changed %d bookmarks.", acc);
#else
    snprintf(notes, NOTESIZ, "Changed %d bookmarks.", acc);
#endif
    AVAlertNote((const char*) notes);

    return;
}

/* Capitalize all Bookmarks */
ACCB1 void ACCB2 PluginCommand_2(void *clientData)
{
    // try to get front PDF document
    AVDoc avDoc = AVAppGetActiveDoc();
    PDDoc pdDoc = AVDocGetPDDoc(avDoc);
    PDBookmark rootBookmark = PDDocGetBookmarkRoot(pdDoc);
    int acc = 0;

    // visit all bookmarks recursively, capitalizing them
    acc = CapitalizeAllBookmarks(pdDoc, rootBookmark, acc);
#ifdef WIN_PLATFORM
    _snprintf(notes, NOTESIZ, "Changed %d bookmarks.", acc);
#else
    snprintf(notes, NOTESIZ, "Changed %d bookmarks.", acc);
#endif
    AVAlertNote((const char*) notes);

    return;
}

ACCB1 ASBool ACCB2 PluginIsEnabled(void *clientData)
{
    // this code make it is enabled only if there is an open PDF document.
    return (AVAppGetActiveDoc() != NULL);
}

ACCB1 ASBool ACCB2 PluginSetMenu()
{
    AVMenubar menubar = AVAppGetMenubar();
    AVMenu volatile commonMenu = NULL;
    
    if (!menubar) return false;
    
DURING
    // Create our menu, title is not important
    subMenu = AVMenuNew("XXX", "CHUN:PluginsMenu", gExtensionID);

    // Create our menuitem
    topMenuItem = AVMenuItemNew("Chun Tian", "CHUN:PluginsMenuItem",
				subMenu, true, NO_SHORTCUT, 0, NULL,
				gExtensionID);
    // Command 0
    int i = 0;
    menuItem[i] = AVMenuItemNew("Capitalize All Bookmarks", "CHUN:Cap_Bookmarks",
				NULL, /* submenu */
				true, /* longMenusOnly */
				NO_SHORTCUT, 0 /* flags */,
				NULL /* icon */, gExtensionID);
    AVMenuItemSetExecuteProc
    (menuItem[i], ASCallbackCreateProto(AVExecuteProc, PluginCommand_2), NULL);

    AVMenuItemSetComputeEnabledProc
    (menuItem[i], ASCallbackCreateProto(AVComputeEnabledProc, PluginIsEnabled),
     (void *)pdPermEdit);
    AVMenuAddMenuItem(subMenu, menuItem[i], APPEND_MENUITEM);

    // Command 1
    i++;
    menuItem[i] = AVMenuItemNew("Fix FitType of All Bookmarks", "CHUN:Fix_Bookmarks",
				NULL, /* submenu */
				true, /* longMenusOnly */
				NO_SHORTCUT, 0 /* flags */,
				NULL /* icon */, gExtensionID);
    AVMenuItemSetExecuteProc
    (menuItem[i], ASCallbackCreateProto(AVExecuteProc, PluginCommand_1), NULL);
    
    AVMenuItemSetComputeEnabledProc
    (menuItem[i], ASCallbackCreateProto(AVComputeEnabledProc, PluginIsEnabled),
     (void *)pdPermEdit);
    AVMenuAddMenuItem(subMenu, menuItem[i], APPEND_MENUITEM);

    /* Acquire() needs a Release() */
    commonMenu = AVMenubarAcquireMenuByName(menubar, pluginMenuName);
    // if "Extensions" menu doesn't exist, create one.
    if (!commonMenu) {
	commonMenu = AVMenuNew(pluginMenuName, pluginMenuName, gExtensionID);
	AVMenubarAddMenu(menubar, commonMenu, APPEND_MENU);
    }

    AVMenuAddMenuItem(commonMenu, topMenuItem, APPEND_MENUITEM);
    AVMenuRelease(commonMenu);
    
HANDLER
    if (commonMenu) AVMenuRelease (commonMenu);
    return false;
    
END_HANDLER
    return true;
}

/* PluginInit
** ------------------------------------------------------
**/
/** 
	The main initialization routine.
	
	@return true to continue loading the plug-in, 
	false to cause plug-in loading to stop.
*/
ACCB1 ASBool ACCB2 PluginInit(void)
{
    return PluginSetMenu();
}

/* PluginUnload
** ------------------------------------------------------
**/
/** 
	The unload routine.
	Called when your plug-in is being unloaded when the application quits.
	Use this routine to release any system resources you may have
	allocated.

	Returning false will cause an alert to display that unloading failed.
	@return true to indicate the plug-in unloaded.
*/
ACCB1 ASBool ACCB2 PluginUnload(void)
{
    return true;
}

/* GetExtensionName
** ------------------------------------------------------
*/
/**
	Returns the unique ASAtom associated with your plug-in.
	@return the plug-in's name as an ASAtom.
*/
ASAtom GetExtensionName()
{
    return ASAtomFromString("CHUN:Bookmarks");
}

/** PIHandshake
    function provides the initial interface between your plug-in and the application.
    This function provides the callback functions to the application that allow it to
    register the plug-in with the application environment.

    Required Plug-in handshaking routine: <b>Do not change its name!</b>
	
    @param handshakeVersion the version this plug-in works with. There are two versions possible, the plug-in version
    and the application version. The application calls the main entry point for this plug-in with its version.
    The main entry point will call this function with the version that is earliest.
    @param handshakeData OUT the data structure used to provide the primary entry points for the plug-in. These
    entry points are used in registering the plug-in with the application and allowing the plug-in to register for
    other plug-in services and offer its own.
    @return true to indicate success, false otherwise (the plug-in will not load).
*/
ACCB1 ASBool ACCB2 PIHandshake(Uns32 handshakeVersion, void *handshakeData)
{
    if (handshakeVersion == HANDSHAKE_V0200) {
	/* Cast handshakeData to the appropriate type */
	PIHandshakeData_V0200 *hsData = (PIHandshakeData_V0200 *)handshakeData;
	
	/* Set the name we want to go by */
	hsData->extensionName = GetExtensionName();
	
	/* If you export your own HFT, do so in here */
	hsData->exportHFTsCallback =
	  (void*)ASCallbackCreateProto(PIExportHFTsProcType, &PluginExportHFTs);
	
	/*
	** If you import plug-in HFTs, replace functionality, and/or want to register for
	** notifications before
	** the user has a chance to do anything, do so in here.
	*/
	hsData->importReplaceAndRegisterCallback =
	    (void*)ASCallbackCreateProto(PIImportReplaceAndRegisterProcType,
					 &PluginImportReplaceAndRegister);

	/* Perform your plug-in's initialization in here */
	hsData->initCallback = (void*)ASCallbackCreateProto(PIInitProcType, &PluginInit);
	
	/* Perform any memory freeing or state saving on "quit" in here */
	hsData->unloadCallback =
	    (void*)ASCallbackCreateProto(PIUnloadProcType, &PluginUnload);
	
	/* All done */
	return true;
	
    } /* Each time the handshake version changes, add a new "else if" branch */
    
    /*
     ** If we reach here, then we were passed a handshake version number we don't know about.
     ** This shouldn't ever happen since our main() routine chose the version number.
     */
    return false;
}
