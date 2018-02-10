/* ********************************************************************** */
/**
 * @brief   SBEAML: public API interfaces.
 * @author  eel3
 * @date    2018-01-29
 */
/* ********************************************************************** */

#ifndef SBEAML_H_INCLUDED
#define SBEAML_H_INCLUDED

#include <stdint.h>
#ifdef __cplusplus
#   include <cstdbool>
#else  /* def __cplusplus */
#   include <stdbool.h>
#endif /* def __cplusplus */

#include "sbeaml_types.h"

/* ---------------------------------------------------------------------- */
/* Error type and codes */
/* ---------------------------------------------------------------------- */

/** SBEAML module error type. */
typedef int32_t SBEAML_ERR;

#define SBEAML_E_OK        ((SBEAML_ERR)  0)      /**< Exit success. */
#define SBEAML_E_NG        ((SBEAML_ERR) -1)      /**< Exit failure. */

#define SBEAML_E_PRM       (SBEAML_E_NG - 1)      /**< Parameter error (perhaps arguments error). */
#define SBEAML_E_RES       (SBEAML_E_NG - 2)      /**< No system resources. */
#define SBEAML_E_STATUS    (SBEAML_E_NG - 3)      /**< Internal status error. */
#define SBEAML_E_SYS       (SBEAML_E_NG - 4)      /**< Error caused by underlying library routines. */

/* ---------------------------------------------------------------------- */
/* Data types */
/* ---------------------------------------------------------------------- */

/** Event handler tag type (must be greater than 0). */
typedef int32_t SBEAML_EVENT_HANDLER_TAG;
/** Invalid tag value. */
#define SBEAML_EVENT_HANDLER_TAG_INVALID 0

/** Timer ID type (must be greater than or equal to 0). */
typedef uint32_t SBEAML_TIMER_ID;

/** System tick type (milliseconds). */
typedef int32_t SBEAML_SYS_TICK_MSEC;

/* ---------------------------------------------------------------------- */
/* Data structures */
/* ---------------------------------------------------------------------- */

/** Event handler type. */
typedef struct SBEAML_EVENT_HANDLER SBEAML_EVENT_HANDLER;
/** Event handler type. */
struct SBEAML_EVENT_HANDLER {
    void (*on_init)(void * const user_data);
    void (*on_appear)(void * const user_data);
    void (*on_event)(void * const user_data, const SBEAML_EVENT_ID id);
    void (*on_timer)(void * const user_data, const SBEAML_TIMER_ID id);
    void (*on_disappear)(void * const user_data);
    void (*on_destroy)(void * const user_data);
    void (*release_user_data)(void * const user_data);
    void *user_data;
    SBEAML_EVENT_HANDLER_TAG tag;
};

/** Generic handler type. */
typedef struct SBEAML_GENERIC_HANDLER SBEAML_GENERIC_HANDLER;
/** Generic handler type. */
struct SBEAML_GENERIC_HANDLER {
    void (*func)(void * const user_data);
    void (*release_user_data)(void * const user_data);
    void *user_data;
};

/** Timer handler type. */
typedef SBEAML_GENERIC_HANDLER SBEAML_TIMER_HANDLER;
/** Message type. */
typedef SBEAML_GENERIC_HANDLER SBEAML_MESSAGE;

/** Preparation parameters. */
typedef struct SBEAML_PREPARE_PARAMS SBEAML_PREPARE_PARAMS;
/** Preparation parameters. */
struct SBEAML_PREPARE_PARAMS {
    const SBEAML_EVENT_HANDLER *root_handler;
};

/* ---------------------------------------------------------------------- */
/* Public API functions */
/* ---------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif /* def __cplusplus */

/* ********************************************************************** */
/**
 * @brief  Initialize the library.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_RES     No system resources.
 * @retval SBEAML_E_STATUS  Internal status error.
 * @retval SBEAML_E_SYS     Error caused by underlying library routines.
 */
/* ********************************************************************** */
extern SBEAML_ERR
sbeaml_Initialize(void);

/* ********************************************************************** */
/**
 * @brief  Finalize the library.
 */
/* ********************************************************************** */
extern void
sbeaml_Finalize(void);

/* ********************************************************************** */
/**
 * @brief  Prepare the library before main loop.
 *
 * @param[in] params  Preparation parameters.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_PRM     Parameter error (perhaps arguments error).
 * @retval SBEAML_E_RES     No system resources.
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
extern SBEAML_ERR
sbeaml_PrepareBeforeMainLoop(const SBEAML_PREPARE_PARAMS * const params);

/* ********************************************************************** */
/**
 * @brief  A resume-yield function for the main loop.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
extern SBEAML_ERR
sbeaml_ResumeAndYield(void);

/* ********************************************************************** */
/**
 * @brief  Cleanup the library after main loop.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
extern SBEAML_ERR
sbeaml_CleanupAfterMainLoop(void);

/* ********************************************************************** */
/**
 * @brief  Push the event handler to the stack.
 *
 * @param[in] handler  Event handler.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_PRM     Parameter error (perhaps arguments error).
 * @retval SBEAML_E_RES     No system resources.
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
extern SBEAML_ERR
sbeaml_PushEventHandler(const SBEAML_EVENT_HANDLER * const handler);

/* ********************************************************************** */
/**
 * @brief  Remove one event handler from the stack (except root handler).
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_NG      Exit failure (perhaps stack is empty).
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
extern SBEAML_ERR
sbeaml_PopEventHandler(void);

/* ********************************************************************** */
/**
 * @brief  Remove event handlers from the stack (except root handler).
 *
 * @param[in] tag  The next top event handler's tag.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_NG      Exit failure (perhaps stack is empty).
 * @retval SBEAML_E_PRM     Parameter error (perhaps arguments error).
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
extern SBEAML_ERR
sbeaml_PopEventHandlerByTag(const SBEAML_EVENT_HANDLER_TAG tag);

/* ********************************************************************** */
/**
 * @brief  Remove all event handlers from the stack (except root handler).
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_NG      Exit failure (perhaps stack is empty).
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
extern SBEAML_ERR
sbeaml_PopEventHandlerAll(void);

/* ********************************************************************** */
/**
 * @brief  Create and start the software timer.
 *
 * @param[in] id            Timer ID.
 * @param[in] timeout_msec  Timeout value (in milliseconds).
 * @param[in] repeat        Repeatedly reschedule or not.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_PRM     Parameter error (perhaps arguments error).
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
extern SBEAML_ERR
sbeaml_SetTimer(const SBEAML_TIMER_ID id,
                const SBEAML_SYS_TICK_MSEC timeout_msec,
                const bool repeat);

/* ********************************************************************** */
/**
 * @brief  Stop and delete the software timer.
 *
 * @param[in] id  Timer ID.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_PRM     Parameter error (perhaps arguments error).
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
extern SBEAML_ERR
sbeaml_KillTimer(const SBEAML_TIMER_ID id);

/* ********************************************************************** */
/**
 * @brief  Create and start the global software timer.
 *
 * @param[in] id            Timer ID.
 * @param[in] timeout_msec  Timeout value (in milliseconds).
 * @param[in] repeat        Repeatedly reschedule or not.
 * @param[in] handler       Timer handler.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_PRM     Parameter error (perhaps arguments error).
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
extern SBEAML_ERR
sbeaml_SetGlobalTimer(const SBEAML_TIMER_ID id,
                      const SBEAML_SYS_TICK_MSEC timeout_msec,
                      const bool repeat,
                      const SBEAML_TIMER_HANDLER * const handler);

/* ********************************************************************** */
/**
 * @brief  Stop and delete the global software timer.
 *
 * @param[in] id  Timer ID.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_PRM     Parameter error (perhaps arguments error).
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
extern SBEAML_ERR
sbeaml_KillGlobalTimer(const SBEAML_TIMER_ID id);

/* ********************************************************************** */
/**
 * @brief  Post the message to the mein loop.
 *
 * @param[in] msg  Message.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_PRM     Parameter error (perhaps arguments error).
 * @retval SBEAML_E_RES     No system resources.
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
extern SBEAML_ERR
sbeaml_PostMessage(const SBEAML_MESSAGE * const msg);

#ifdef __cplusplus
} /* extern "C" */
#endif /* def __cplusplus */

#endif /* ndef SBEAML_H_INCLUDED */
