/* ********************************************************************** */
/**
 * @brief   SBEAML: machdep implementation (sample && test application).
 * @author  eel3
 * @date    2017-09-01
 */
/* ********************************************************************** */

#include "sbeaml_md.h"

#include <cassert>
#include <chrono>
#include <mutex>

namespace {

/* ---------------------------------------------------------------------- */
/* Data structures */
/* ---------------------------------------------------------------------- */

/** Module context type. */
struct MODULE_CTX {
    bool initialized;
    bool prepared;
    SBEAML_EVENT_HANDLER_CELL handlers[SBEAML_CFG_MAX_EVENT_HANDLER];
    SBEAML_MESSAGE_CELL messages[SBEAML_CFG_MAX_MESSAGE];
    std::mutex mutex_for_api;

    MODULE_CTX() : initialized(false), prepared(false) {}
};

/* ---------------------------------------------------------------------- */
/* File scope variables */
/* ---------------------------------------------------------------------- */

/** Module context. */
MODULE_CTX module_ctx;

/* ---------------------------------------------------------------------- */
/* Template Functions */
/* ---------------------------------------------------------------------- */

/* ====================================================================== */
/**
 * @brief  Return the maximum number of elements.
 *
 * @param[in] (no_parameter_name)  An array.
 *
 * @return  Maximum number of elements.
 */
/* ====================================================================== */
template <typename T, size_t N>
inline size_t
NELEMS(const T (&)[N])
{
    return N;
}

} // namespace

/* ---------------------------------------------------------------------- */
/* Functions */
/* ---------------------------------------------------------------------- */

extern "C" {

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
    auto& mc = module_ctx;

    if (mc.initialized) {
        return SBEAML_E_STATUS;
    }

    mc.prepared = false;

    mc.initialized = true;

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
    auto& mc = module_ctx;

    if (!mc.initialized) {
        return;
    }

    mc.initialized = false;
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
    auto& mc = module_ctx;

    assert(mc.initialized);

    if (mc.prepared) {
        return SBEAML_E_STATUS;
    }

    for (size_t i { 0 }; i < NELEMS(mc.handlers); i++) {
        mc.handlers[i].empty = true;
    }

    for (size_t i { 0 }; i < NELEMS(mc.messages); i++) {
        mc.messages[i].empty = true;
    }

    mc.prepared = true;

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
    auto& mc = module_ctx;

    assert(mc.initialized);

    if (!mc.prepared) {
        return SBEAML_E_STATUS;
    }

    mc.prepared = false;

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
    auto& mc = module_ctx;

    assert(mc.initialized);

    for (size_t i { 0 }; i < NELEMS(mc.handlers); i++) {
        auto& cell = mc.handlers[i];
        if (cell.empty) {
            cell.empty = false;
            return &cell;
        }
    }

    return nullptr;
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
    auto& mc = module_ctx;

    assert(mc.initialized);

    for (size_t i { 0 }; i < NELEMS(mc.messages); i++) {
        auto& cell = mc.messages[i];
        if (cell.empty) {
            cell.empty = false;
            return &cell;
        }
    }

    return nullptr;
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

    using std::chrono::steady_clock;
    using std::chrono::milliseconds;
    using std::chrono::duration_cast;

    auto tp = steady_clock::now();
    auto ms = duration_cast<milliseconds>(tp.time_since_epoch());

    return static_cast<SBEAML_SYS_TICK_MSEC>(ms.count());
}

/* ********************************************************************** */
/**
 * @brief  A lock function for the library.
 */
/* ********************************************************************** */
void
sbeaml_md_LockForAPI(void)
{
    auto& mc = module_ctx;

    assert(mc.initialized);

    mc.mutex_for_api.lock();
}

/* ********************************************************************** */
/**
 * @brief  An unlock function for the library.
 */
/* ********************************************************************** */
void
sbeaml_md_UnlockForAPI(void)
{
    auto& mc = module_ctx;

    assert(mc.initialized);

    mc.mutex_for_api.unlock();
}

} // extern "C"
