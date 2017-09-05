/* ********************************************************************** */
/**
 * @brief   SBEAML: machdep implementation (sample code).
 * @author  eel3
 * @date    2017-09-05
 */
/* ********************************************************************** */

#include "sbeaml_md.h"

#include <stddef.h>

#ifdef SBEAML_CFG_USE_ASSERT_H
#include <assert.h>
#else
#define assert(cond)
#endif

/* ---------------------------------------------------------------------- */
/* Data structures */
/* ---------------------------------------------------------------------- */

/** Module context type. */
typedef struct {
    bool initialized;
    bool prepared;
    SBEAML_EVENT_HANDLER_CELL handlers[SBEAML_CFG_MAX_EVENT_HANDLER];
    SBEAML_MESSAGE_CELL messages[SBEAML_CFG_MAX_MESSAGE];
} MODULE_CTX;

/* ---------------------------------------------------------------------- */
/* File scope variables */
/* ---------------------------------------------------------------------- */

/** Module context. */
static MODULE_CTX module_ctx;

/* ---------------------------------------------------------------------- */
/* Function-like macros */
/* ---------------------------------------------------------------------- */

/* ====================================================================== */
/**
 * @brief  Return the maximum number of elements.
 *
 * @param[in] array  An array.
 *
 * @return  Maximum number of elements.
 */
/* ====================================================================== */
#define NELEMS(array) (sizeof(array) / sizeof((array)[0]))

/* ---------------------------------------------------------------------- */
/* Functions */
/* ---------------------------------------------------------------------- */

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
SBEAML_ERR
sbeaml_md_Initialize(void)
{
    MODULE_CTX * const mc = &module_ctx;

    if (mc->initialized) {
        return SBEAML_E_STATUS;
    }

    mc->prepared = false;

    mc->initialized = true;

    return SBEAML_E_OK;
}

/* ********************************************************************** */
/**
 * @brief  Finalize the machdep library.
 *
 * @note  This function will be called in sbeaml_Finalize().
 */
/* ********************************************************************** */
void
sbeaml_md_Finalize(void)
{
    MODULE_CTX * const mc = &module_ctx;

    if (!mc->initialized) {
        return;
    }

    mc->initialized = false;
}

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
SBEAML_ERR
sbeaml_md_PrepareBeforeMainLoop(void)
{
    MODULE_CTX * const mc = &module_ctx;
    size_t i;

    assert(mc->initialized);

    if (mc->prepared) {
        return SBEAML_E_STATUS;
    }

    for (i = 0; i < NELEMS(mc->handlers); i++) {
        mc->handlers[i].empty = true;
    }

    for (i = 0; i < NELEMS(mc->messages); i++) {
        mc->messages[i].empty = true;
    }

    mc->prepared = true;

    return SBEAML_E_OK;
}

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
SBEAML_ERR
sbeaml_md_CleanupAfterMainLoop(void)
{
    MODULE_CTX * const mc = &module_ctx;

    assert(mc->initialized);

    if (!mc->prepared) {
        return SBEAML_E_STATUS;
    }

    mc->prepared = false;

    return SBEAML_E_OK;
}

/* ********************************************************************** */
/**
 * @brief  Allocate memory space for SBEAML_EVENT_HANDLER_CELL type.
 *
 * @retval !=NULL  Exit success.
 * @retval   NULL  Exit failure.
 */
/* ********************************************************************** */
SBEAML_EVENT_HANDLER_CELL *
sbeaml_md_AllocEventHandlerCell(void)
{
    MODULE_CTX * const mc = &module_ctx;
    size_t i;

    assert(mc->initialized);

    for (i = 0; i < NELEMS(mc->handlers); i++) {
        SBEAML_EVENT_HANDLER_CELL *cell;

        cell = &mc->handlers[i];
        if (cell->empty) {
            cell->empty = false;
            return cell;
        }
    }

    return NULL;
}

/* ********************************************************************** */
/**
 * @brief  Deallocate memory space for SBEAML_EVENT_HANDLER_CELL type.
 *
 * @param[in,out] cell  Memory space to deallocate.
 */
/* ********************************************************************** */
void
sbeaml_md_DeallocEventHandlerCell(SBEAML_EVENT_HANDLER_CELL * const cell)
{
    assert(module_ctx.initialized);

    cell->empty = true;
}

/* ********************************************************************** */
/**
 * @brief  Allocate memory space for SBEAML_MESSAGE_CELL type.
 *
 * @retval !=NULL  Exit success.
 * @retval   NULL  Exit failure.
 */
/* ********************************************************************** */
SBEAML_MESSAGE_CELL *
sbeaml_md_AllocMessageCell(void)
{
    MODULE_CTX * const mc = &module_ctx;
    size_t i;

    assert(mc->initialized);

    for (i = 0; i < NELEMS(mc->messages); i++) {
        SBEAML_MESSAGE_CELL *cell;

        cell = &mc->messages[i];
        if (cell->empty) {
            cell->empty = false;
            return cell;
        }
    }

    return NULL;
}

/* ********************************************************************** */
/**
 * @brief  Deallocate memory space for SBEAML_MESSAGE_CELL type.
 *
 * @param[in,out] cell  Memory space to deallocate.
 */
/* ********************************************************************** */
void
sbeaml_md_DeallocMessageCell(SBEAML_MESSAGE_CELL * const cell)
{
    assert(module_ctx.initialized);

    cell->empty = true;
}

/* ********************************************************************** */
/**
 * @brief  Get system tick value.
 *
 * @return  System tick in milliseconds.
 */
/* ********************************************************************** */
SBEAML_SYS_TICK_MSEC
sbeaml_md_GetTick(void)
{
    assert(module_ctx.initialized);

    /* TODO: Need to implement this function. */

    return 0;
}

/* ********************************************************************** */
/**
 * @brief  Peek event.
 *
 * @return  Event ID.
 */
/* ********************************************************************** */
SBEAML_EVENT_ID
sbeaml_md_PeekEvent(void)
{
    assert(module_ctx.initialized);

    /* TODO: Need to implement this function. */

    return SBEAML_EVENT_ID_NONE;
}

/* ********************************************************************** */
/**
 * @brief  A lock function for the library.
 */
/* ********************************************************************** */
void
sbeaml_md_LockForAPI(void)
{
    assert(module_ctx.initialized);

    /* TODO: Need to implement this function. */
}

/* ********************************************************************** */
/**
 * @brief  An unlock function for the library.
 */
/* ********************************************************************** */
void
sbeaml_md_UnlockForAPI(void)
{
    assert(module_ctx.initialized);

    /* TODO: Need to implement this function. */
}
