/*
 * Export window information for the Windows-OpenGL GLX implementation.
 *
 * Authors: Alexander Gottwald
 */

#ifdef HAVE_XWIN_CONFIG_H
#include <xwin-config.h>
#endif
#include "win.h"
#include "winpriv.h"
#include "winwindow.h"

void
 winCreateWindowsWindow(WindowPtr pWin);

static void
winCreateWindowsWindowHierarchy(WindowPtr pWin)
{
    winWindowPriv(pWin);

    winDebug("winCreateWindowsWindowHierarchy - pWin:%08x XID:0x%x \n", pWin,
             pWin->drawable.id);

    /* recursively ensure parent window exists if it's not the root window */
    if (pWin->parent) {
        if (pWin->parent != pWin->drawable.pScreen->root)
            winCreateWindowsWindowHierarchy(pWin->parent);
    }

    /* ensure this window exists */
    if (pWinPriv->hWnd == NULL) {
        winCreateWindowsWindow(pWin);

        /* ... and if it's already been mapped, make sure it's visible */
        if (pWin->mapped) {
            /* Display the window without activating it */
            if (pWin->drawable.class != InputOnly)
                ShowWindow(pWinPriv->hWnd, SW_SHOWNOACTIVATE);

            /* Send first paint message */
            UpdateWindow(pWinPriv->hWnd);
        }
    }
}

/**
 * Return size and handles of a window.
 * If pWin is NULL, then the information for the root window is requested.
 */
HWND
winGetWindowInfo(WindowPtr pWin)
{
    winTrace("%s: pWin %p XID 0x%x\n", __FUNCTION__, pWin, pWin->drawable.id);

    /* a real window was requested */
    if (pWin != NULL) {
        /* Get the window and screen privates */
        ScreenPtr pScreen = pWin->drawable.pScreen;
        winPrivScreenPtr pWinScreen = winGetScreenPriv(pScreen);
        winScreenInfoPtr pScreenInfo = NULL;
        HWND hwnd = NULL;

        if (pWinScreen == NULL) {
            ErrorF("winGetWindowInfo: screen has no privates\n");
            return NULL;
        }

        hwnd = pWinScreen->hwndScreen;

        pScreenInfo = pWinScreen->pScreenInfo;
#ifdef XWIN_MULTIWINDOW
        /* check for multiwindow mode */
        if (pScreenInfo->fMultiWindow) {
            winWindowPriv(pWin);

            if (pWinPriv == NULL) {
                ErrorF("winGetWindowInfo: window has no privates\n");
                return hwnd;
            }

            if (pWinPriv->hWnd == NULL) {
                ErrorF("winGetWindowInfo: forcing window to exist\n");
                winCreateWindowsWindowHierarchy(pWin);
            }

            if (pWinPriv->hWnd != NULL) {
                /* copy window handle */
                hwnd = pWinPriv->hWnd;

                /* mark GLX active on that hwnd */
                pWinPriv->fWglUsed = TRUE;
            }

            return hwnd;
        }
#endif
#ifdef XWIN_MULTIWINDOWEXTWM
        /* check for multiwindow external wm mode */
        if (pScreenInfo->fMWExtWM) {
            win32RootlessWindowPtr pRLWinPriv
                = (win32RootlessWindowPtr) RootlessFrameForWindow(pWin, FALSE);

            if (pRLWinPriv == NULL) {
                ErrorF("winGetWindowInfo: window has no privates\n");
                return hwnd;
            }

            if (pRLWinPriv->hWnd != NULL) {
                /* copy window handle */
                hwnd = pRLWinPriv->hWnd;
            }
            return hwnd;
        }
#endif
    }
    else {
        ScreenPtr pScreen = g_ScreenInfo[0].pScreen;
        winPrivScreenPtr pWinScreen = winGetScreenPriv(pScreen);

        if (pWinScreen == NULL) {
            ErrorF("winGetWindowInfo: screen has no privates\n");
            return NULL;
        }

        ErrorF("winGetWindowInfo: returning root window\n");

        return pWinScreen->hwndScreen;
    }

    return NULL;
}

Bool
winCheckScreenAiglxIsSupported(ScreenPtr pScreen)
{
    winPrivScreenPtr pWinScreen = winGetScreenPriv(pScreen);
    winScreenInfoPtr pScreenInfo = pWinScreen->pScreenInfo;

#ifdef XWIN_MULTIWINDOW
    if (pScreenInfo->fMultiWindow)
        return TRUE;
#endif

#ifdef XWIN_MULTIWINDOWEXTWM
    if (pScreenInfo->fMWExtWM)
        return TRUE;
#endif

    return FALSE;
}
