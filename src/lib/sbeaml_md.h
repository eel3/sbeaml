/* ********************************************************************** */
/**
 * @brief   SBEAML: machdep interfaces.
 * @author  eel3
 * @date    2017-08-18
 */
/* ********************************************************************** */

#ifndef SBEAML_MD_H_INCLUDED
#define SBEAML_MD_H_INCLUDED

#include "sbeaml.h"
#include "sbeaml_private.h"

/* ---------------------------------------------------------------------- */
/* Functions */
/* ---------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif /* def __cplusplus */

/* ********************************************************************** */
/**
 * @brief  Initialize the machdep library.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_RES     No system resources.
 * @retval SBEAML_E_STATUS  Internal status error.
 * @retval SBEAML_E_SYS     Error caused by underlying library routines.
 *
 * @note  This function will be called in sbeaml_Initialize().
 */
/* ********************************************************************** */
extern SBEAML_ERR
sbeaml_md_Initialize(void);

/* ********************************************************************** */
/**
 * @brief  Finalize the machdep library.
 *
 * @note  This function will be called in sbeaml_Finalize().
 */
/* ********************************************************************** */
extern void
sbeaml_md_Finalize(void);

/* ********************************************************************** */
/**
 * @brief  Prepare the machdep library before main loop.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_STATUS  Internal status error.
 *
 * @note  This function will be called in sbeaml_PrepareBeforeMainLoop().
 */
/* ********************************************************************** */
extern SBEAML_ERR
sbeaml_md_PrepareBeforeMainLoop(void);

/* ********************************************************************** */
/**
 * @brief  Cleanup the machdep library after main loop.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_STATUS  Internal status error.
 *
 * @note  This function will be called in sbeaml_CleanupAfterMainLoop().
 */
/* ********************************************************************** */
extern SBEAML_ERR
sbeaml_md_CleanupAfterMainLoop(void);

/* ********************************************************************** */
/**
 * @brief  Allocate memory space for SBEAML_EVENT_HANDLER_CELL type.
 *
 * @retval !=NULL  Exit success.
 * @retval   NULL  Exit failure.
 */
/* ********************************************************************** */
extern SBEAML_EVENT_HANDLER_CELL *
sbeaml_md_AllocEventHandlerCell(void);

/* ********************************************************************** */
/**
 * @brief  Deallocate memory space for SBEAML_EVENT_HANDLER_CELL type.
 *
 * @param[in,out] cell  memory space to deallocate.
 */
/* ********************************************************************** */
extern void
sbeaml_md_DeallocEventHandlerCell(SBEAML_EVENT_HANDLER_CELL * const cell);

/* ********************************************************************** */
/**
 * @brief  Allocate memory space for SBEAML_MESSAGE_CELL type.
 *
 * @retval !=NULL  Exit success.
 * @retval   NULL  Exit failure.
 */
/* ********************************************************************** */
extern SBEAML_MESSAGE_CELL *
sbeaml_md_AllocMessageCell(void);

/* ********************************************************************** */
/**
 * @brief  Deallocate memory space for SBEAML_MESSAGE_CELL type.
 *
 * @param[in,out] cell  memory space to deallocate.
 */
/* ********************************************************************** */
extern void
sbeaml_md_DeallocMessageCell(SBEAML_MESSAGE_CELL * const cell);

/* ********************************************************************** */
/**
 * @brief  Get system tick value.
 *
 * @return  System tick in milliseconds.
 */
/* ********************************************************************** */
extern SBEAML_SYS_TICK_MSEC
sbeaml_md_GetTick(void);

/* ********************************************************************** */
/**
 * @brief  Peek event.
 *
 * @return  Event ID.
 */
/* ********************************************************************** */
extern SBEAML_EVENT_ID
sbeaml_md_PeekEvent(void);

/* ********************************************************************** */
/**
 * @brief  A lock function for the library.
 */
/* ********************************************************************** */
extern void
sbeaml_md_LockForAPI(void);

/* ********************************************************************** */
/**
 * @brief  An unlock function for the library.
 */
/* ********************************************************************** */
extern void
sbeaml_md_UnlockForAPI(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* def __cplusplus */

#endif /* ndef SBEAML_MD_H_INCLUDED */
