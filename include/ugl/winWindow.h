/******************************************************************************
 *   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
 *
 *   This file is part of Real VMX.
 *   Copyright (C) 2015 Surplus Users Ham Society
 *
 *   Real VMX is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Real VMX is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Real VMX.  If not, see <http://www.gnu.org/licenses/>.
 */

/* winWindow.h - Universal graphics librarye window header */

#ifndef _winWindow_h
#define _winWindow_h

#include "ugl.h"
#include "winTypes.h"
#include "winMsg.h"
#include "uglregn.h"

/* Defines */

/* Window attributes */
#define WIN_ATTRIB_CLIP_CHILDREN    0x00000001
#define WIN_ATTRIB_DBL_CLICK        0x00000002
#define WIN_ATTRIB_DISABLED         0x00000004
#define WIN_ATTRIB_DOUBLE_BUFFER    0x00000008
#define WIN_ATTRIB_FRAMED           0x00000010
#define WIN_ATTRIB_KBD_INPUT        0x00000020
#define WIN_ATTRIB_NO_ACTIVATE      0x00000040
#define WIN_ATTRIB_NO_CLOSE         0x00000080
#define WIN_ATTRIB_NO_KEYBOARD      0x00000100
#define WIN_ATTRIB_NO_MAXIMIZE      0x00000200
#define WIN_ATTRIB_NO_MINIMIZE      0x00000400
#define WIN_ATTRIB_NO_MOVE          0x00000800
#define WIN_ATTRIB_NO_POINTER       0x00001000
#define WIN_ATTRIB_NO_RESIZE        0x00002000
#define WIN_ATTRIB_OFF_SCREEN       0x00004000
#define WIN_ATTRIB_TOPMOST          0x00008000
#define WIN_ATTRIB_TRANSPARENT      0x00010000
#define WIN_ATTRIB_TRI_CLICK        (0x00020000 | WIN_ATTRIB_DBL_CLICK)
#define WIN_ATTRIB_VISIBLE          0x00040000
#define WIN_ATTRIB_NO_INPUT         (WIN_ATTRIB_NO_KEYBOARD | \
                                     WIN_ATTRIB_NO_POINTER)
#define WIN_DYNAMIC_ATTRIBUTES      (WIN_ATTRIB_DBL_CLICK | \
                                     WIN_ATTRIB_DISABLED | \
                                     WIN_ATTRIB_KBD_INPUT | \
                                     WIN_ATTRIB_NO_ACTIVATE | \
                                     WIN_ATTRIB_NO_INPUT | \
                                     WIN_ATTRIB_TOPMOST | \
                                     WIN_ATTRIB_TRI_CLICK)

/* Window states */
#define WIN_STATE_ACTIVE            0x00000001
#define WIN_STATE_DIRTY             0x00000002
#define WIN_STATE_HIDDEN            0x00000004
#define WIN_STATE_MANAGED           0x00000008
#define WIN_STATE_MAXIMIZED         0x00000010
#define WIN_STATE_MINIMIZED         0x00000020

/* Colors */
#define WIN_NUM_STANDARD_COLORS             16

#define WIN_INDEX_BLACK                      0
#define WIN_INDEX_BLUE                       1
#define WIN_INDEX_GREEN                      2
#define WIN_INDEX_CYAN                       3
#define WIN_INDEX_RED                        4
#define WIN_INDEX_MAGENTA                    5
#define WIN_INDEX_BROWN                      6
#define WIN_INDEX_LIGHTGRAY                  7
#define WIN_INDEX_DARKDRAY                   8
#define WIN_INDEX_LIGHTBLUE                  9
#define WIN_INDX_LIGHTGREEN                 10
#define WIN_INDEX_LIGHTCYAN                 11
#define WIN_INDEX_LIGHTRED                  12
#define WIN_INDEX_LIGHTMAGENTA              13
#define WIN_INDEX_YELLOW                    14
#define WIN_INDEX_WHITE                     15

/* Cursors */
#define WIN_NUM_STANDARD_CURSORS            17
#define WIN_CURSOR_INDEX_ARROW               0
#define WIN_CURSOR_INDEX_EDIT                1
#define WIN_CURSOR_INDEX_WAIT                2
#define WIN_CURSOR_INDEX_INVALID             3
#define WIN_CURSOR_INDEX_MOVE                4
#define WIN_CURSOR_INDEX_SIZE_HORIZ          5
#define WIN_CURSOR_INDEX_SIZE_LEFT           6
#define WIN_CURSOR_INDEX_SIZE_RIGHT          7
#define WIN_CURSOR_INDEX_SIZE_VERT           8
#define WIN_CURSOR_INDEX_SIZE_TOP            9
#define WIN_CURSOR_INDEX_SIZE_BOTTOM        10
#define WIN_CURSOR_INDEX_SIZE_TL_BR         11
#define WIN_CURSOR_INDEX_SIZE_TOP_LEFT      12
#define WIN_CURSOR_INDEX_SIZE_BOTTOM_RIGHT  13
#define WIN_CURSOR_INDEX_SIZE_TR_BL         14
#define WIN_CURSOR_INDEX_SIZE_TOP_RIGHT     15
#define WIN_CURSOR_INDEX_SIZE_BOTTOM_LEFT   16

/* Fonts */
#define WIN_NUM_STANDARD_FONTS               3

#define WIN_FONT_INDEX_SYSTEM                0
#define WIN_FONT_INDEX_SMALL                 1
#define WIN_FONT_INDEX_FIXED                 2

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* Macros */

/* Colors */
#define WIN_BLACK               (winColorGet(UGL_NULL, WIN_INDEX_BLACK))
#define WIN_BLUE                (winColorGet(UGL_NULL, WIN_INDEX_BLUE))
#define WIN_GREEN               (winColorGet(UGL_NULL, WIN_INDEX_GREEN))
#define WIN_CYAN                (winColorGet(UGL_NULL, WIN_INDEX_CYAN))
#define WIN_RED                 (winColorGet(UGL_NULL, WIN_INDEX_RED))
#define WIN_MAGENTA             (winColorGet(UGL_NULL, WIN_INDEX_MAGENTA))
#define WIN_BROWN               (winColorGet(UGL_NULL, WIN_INDEX_BROWN))
#define WIN_LIGHTGRAY           (winColorGet(UGL_NULL, WIN_INDEX_LIGHTGRAY))
#define WIN_DARKGRAY            (winColorGet(UGL_NULL, WIN_INDEX_DARKGRAY))
#define WIN_LIGHTBLUE           (winColorGet(UGL_NULL, WIN_INDEX_LIGHTBLUE))
#define WIN_LIGHTGREEN          (winColorGet(UGL_NULL, WIN_INDEX_LIGHTGREEN))
#define WIN_LIGHTCYAN           (winColorGet(UGL_NULL, WIN_INDEX_LIGHTCYAN))
#define WIN_LIGHTRED            (winColorGet(UGL_NULL, WIN_INDEX_LIGHTRED))
#define WIN_LIGHTMAGENTA        (winColorGet(UGL_NULL, WIN_INDEX_LIGHTMAGENTA))
#define WIN_YELLOW              (winColorGet(UGL_NULL, WIN_INDEX_YELLOW))
#define WIN_WHITE               (winColorGet(UGL_NULL, WIN_INDEX_WHITE))

/* Cursors */
#define WIN_CURSOR_ARROW \
    (winCursorImageGet(UGL_NULL, WIN_CURSOR_INDEX_ARROW))

#define WIN_CURSOR_EDIT \
    (winCursorImageGet(UGL_NULL, WIN_CURSOR_INDEX_EDIT))

#define WIN_CURSOR_WAIT \
    (winCursorImageGet(UGL_NULL, WIN_CURSOR_INDEX_WAIT))

#define WIN_CURSOR_INVALID \
    (winCursorImageGet(UGL_NULL, WIN_CURSOR_INDEX_INVALID))

#define WIN_CURSOR_MOVE \
    (winCursorImageGet(UGL_NULL, WIN_CURSOR_INDEX_MOVE))

#define WIN_CURSOR_SIZE_HORIZ \
    (winCursorImageGet(UGL_NULL, WIN_CURSOR_INDEX_SIZE_HORIZ))

#define WIN_CURSOR_SIZE_LEFT \
    (winCursorImageGet(UGL_NULL, WIN_CURSOR_INDEX_SIZE_LEFT))

#define WIN_CURSOR_SIZE_RIGHT \
    (winCursorImageGet(UGL_NULL, WIN_CURSOR_INDEX_SIZE_RIGHT))

#define WIN_CURSOR_SIZE_VERT \
    (winCursorImageGet(UGL_NULL, WIN_CURSOR_INDEX_SIZE_VERT))

#define WIN_CURSOR_SIZE_TOP \
    (winCursorImageGet(UGL_NULL, WIN_CURSOR_INDEX_SIZE_TOP))

#define WIN_CURSOR_SIZE_BOTTOM \
    (winCursorImageGet(UGL_NULL, WIN_CURSOR_INDEX_SIZE_BOTTOM))

#define WIN_CURSOR_SIZE_TL_BR \
    (winCursorImageGet(UGL_NULL, WIN_CURSOR_INDEX_SIZE_TL_BR))

#define WIN_CURSOR_SIZE_TOP_LEFT \
    (winCursorImageGet(UGL_NULL, WIN_CURSOR_INDEX_SIZE_TOP_LEFT))

#define WIN_CURSOR_SIZE_BOTTOM_RIGHT \
    (winCursorImageGet(UGL_NULL, WIN_CURSOR_INDEX_SIZE_BOTTOM_RIGHT))

#define WIN_CURSOR_SIZE_TR_BL \
    (winCursorImageGet(UGL_NULL, WIN_CURSOR_INDEX_SIZE_TR_BL))

#define WIN_CURSOR_SIZE_TOP_RIGHT \
    (winCursorImageGet(UGL_NULL, WIN_CURSOR_INDEX_SIZE_TOP_RIGHT))

#define WIN_CURSOR_SIZE_BOTTOM_LEFT \
    (winCursorImageGet(UGL_NULL, WIN_CURSOR_INDEX_SIZE_BOTTOM_LEFT))

/* Types */

typedef UGL_STATUS  (WIN_CB) (
    WIN_ID     winId,
    WIN_MSG *  pMsg,
    void *     pData,
    void *     pParam
    );

typedef struct win_cb_item {
    UGL_UINT32  filterMin;
    UGL_UINT32  filterMax;
    WIN_CB *    pCallback;
    void *      pParam;
} WIN_CB_ITEM;

/******************************************************************************
 *
 * winCount - Get number of child windows
 *
 * RETURNS: Number of child windows or 0
 */

UGL_SIZE  winCount (
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winFirst - Get window first child
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winFirst (
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winLast - Get window last child
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winLast (
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winNext - Get next window
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winNext (
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winPrev - Get previous window
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winPrev (
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winNth - Get n:th window
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winNth (
    WIN_ID   winId,
    UGL_ORD  n
    );

/******************************************************************************
 *
 * winLock - Get excluse access to window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winLock (
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winUnlock - Give up excluse access to window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winUnlock (
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winStateGet - Get window state
 *
 * RETURNS: Window state or UGL_STATUS_ERROR
 */

UGL_UINT32  winStateGet (
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winAttribGet - Get public window attributes
 *
 * RETURNS: Window attributes or zero
 */

UGL_UINT32  winAttribGet (
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winVisibleSet - Check if a window is visible
 *
 * RETURNS: UGL_TRUE or UGL_FALSE
 */

UGL_STATUS  winVisibleSet (
    WIN_ID    winId,
    UGL_BOOL  visible
    );

/******************************************************************************
 *
 * winVisibleGet - Check if a window is visible
 *
 * RETURNS: UGL_TRUE or UGL_FALSE
 */

UGL_BOOL  winVisibleGet (
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winMgrGet - Get window manager for window
 *
 * RETURNS: Pointer to window manager or UGL_NULL
 */

WIN_MGR_ID  winMgrGet (
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winWindowToScreen - Convert window coordinates to screen coordinates
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winWindowToScreen (
    WIN_ID       winId,
    UGL_POINT *  pPoints,
    UGL_SIZE     numPoints
    );

/******************************************************************************
 *
 * winWindowRectToScreen - Convert window rectangles to screen coordinates
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winWindowRectToScreen (
    WIN_ID       winId,
    UGL_RECT *   pRects,
    UGL_SIZE     numRects
    );

/******************************************************************************
 *
 * winScreenToWindow - Convert screen coordinates to window coordinates
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winScreenToWindow (
    WIN_ID       winId,
    UGL_POINT *  pPoints,
    UGL_SIZE     numPoints
    );

/******************************************************************************
 *
 * winDrawRectGet - Get window drawing rectangle
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winDrawRectGet (
    WIN_ID      winId,
    UGL_RECT *  pRect
    );

/******************************************************************************
 *
 * winGetFromPoint - Get window at point
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winGetFromPoint (
    WIN_MGR_ID   winMgrId,
    UGL_POINT *  pPoint
    );

/******************************************************************************
 *
 * winColorGet - Get color from window standard colors or zero
 *
 * RETURNS: Window color
 */

UGL_COLOR  winColorGet (
    WIN_ID   winId,
    UGL_ORD  index
    );

/******************************************************************************
 *
 * winCursorImageGet - Get cursor from window cursors
 *
 * RETURNS: Cursor image id or UGL_NULL
 */

UGL_CDDB_ID  winCursorImageGet (
    WIN_ID   winId,
    UGL_ORD  index
    );

/******************************************************************************
 *
 * winPointerGrabGet - Get window which has focus
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winPointerGrabGet (
    WIN_MGR_ID  winMgrId
    );

/******************************************************************************
 *
 * winSend - Prepare and send message to window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winSend (
    WIN_ID        winId,
    UGL_MSG_TYPE  msgType,
    const void *  pMsgData,
    UGL_SIZE      dataSize
    );

/******************************************************************************
 *
 * winMsgSend - Send message to window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winMsgSend (
    WIN_ID     winId,
    WIN_MSG *  pMsg
    );

/******************************************************************************
 *
 * winMsgHandle - Handle window message
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winMsgHandle (
    WIN_ID        winId,
    WIN_CLASS_ID  classId,
    WIN_MSG *     pMsg
    );

/******************************************************************************
 *
 * winCreate - Create window
 *
 * RETURNS: WIN_ID or UGL_NULL
 */

WIN_ID  winCreate (
    WIN_APP_ID           appId,
    WIN_CLASS_ID         classId,
    UGL_UINT32           attributes,
    UGL_POS              x,
    UGL_POS              y,
    UGL_SIZE             width,
    UGL_SIZE             height,
    void *               pAppData,
    UGL_SIZE             appDataSize,
    const WIN_CB_ITEM *  pCallbackArray
    );

/******************************************************************************
 *
 * winDestroy - Destroy window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winDestroy (
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winFree - Free resources used by window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winFree (
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winCbAdd - Add callback to window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winCbAdd (
    WIN_ID      winId,
    UGL_UINT32  filterMin,
    UGL_UINT32  filterMax,
    WIN_CB *    pCallback,
    void *      pParam
    );

/******************************************************************************
 *
 * winSizeSet - Set size of a window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winSizeSet (
    WIN_ID    winId,
    UGL_SIZE  width,
    UGL_SIZE  height
    );

/******************************************************************************
 *
 * winRectSet - Set dimensions of a window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winRectSet (
    WIN_ID            winId,
    const UGL_RECT *  pRect
    );

/******************************************************************************
 *
 * winRectGet - Get window rectangle
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winRectGet (
    WIN_ID      winId,
    UGL_RECT *  pRect
    );

/******************************************************************************
 *
 * winZPosSet - Set window depth position
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winZPosSet (
    WIN_ID   winId,
    UGL_ORD  zPos
    );

/******************************************************************************
 *
 * winZPosGet - Get window depth position
 *
 * RETURNS: Window z position or 0
 */

UGL_ORD  winZPosGet (
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winActivate - Activate window
 *
 * RETURNS: UGL_STATUS_OK, UGL_STATUS_PERMISSION_DENIED or UGL_STATUS_ERROR
 */

UGL_STATUS  winActivate (
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winIsActive - Check if a window is active
 *
 * RETURNS: UGL_TRUE or UGL_FALSE
 */

UGL_BOOL  winIsActive (
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winAttach - Attach window to hiearachy
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winAttach (
    WIN_ID  childId,
    WIN_ID  parentId,
    WIN_ID  nextId
    );

/******************************************************************************
 *
 * winManage - Manage window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winManage (
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winDrawStart - Start window drawing
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_GC_ID  winDrawStart (
    WIN_ID     winId,
    UGL_GC_ID  gcId,
    UGL_BOOL   clipToDirty
    );

/******************************************************************************
 *
 * winDrawEnd - End window drawing
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winDrawEnd (
    WIN_ID     winId,
    UGL_GC_ID  gcId,
    UGL_BOOL   clearDirty
    );

/******************************************************************************
 *
 * winInvalidate - Invalidate window and children
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS winInvalidate(
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winRectInvalidate - Invalidate rectangle of window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winRectInvalidate (
    WIN_ID            winId,
    const UGL_RECT *  pRect
    );

/******************************************************************************
 *
 * winDirtySet - Mark window as dirty
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winDirtySet (
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winDirtyGet - Get next window on dirty list
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winDirtyGet (
    WIN_APP_ID  appId
    );

/******************************************************************************
 *
 * winUpdate - Request to update window directly
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */
 
UGL_STATUS  winUpdate (
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winDeadGet - Get next window on destroy list
 *
 * RETURNS: Window id or UGL_NULL
 */

WIN_ID  winDeadGet (
    WIN_APP_ID  appId
    );

/******************************************************************************
 *
 * winShow - Show window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winShow (
    WIN_ID  winId
    );

/******************************************************************************
 *
 * winHide - Hide window
 *
 * RETURNS: UGL_STATUS_OK or UGL_STATUS_ERROR
 */

UGL_STATUS  winHide (
    WIN_ID  winId
    );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASMLANGUAGE */

#endif /* _winWindow_h */

