/* ********************************************************************** */
/**
 * @brief   SBEAML: private interfaces.
 * @author  eel3
 * @date    2017-09-01
 */
/* ********************************************************************** */

#ifndef SBEAML_PRIVATE_H_INCLUDED
#define SBEAML_PRIVATE_H_INCLUDED

#include "sbeaml.h"
#include "sbeaml_config.h"

/* ---------------------------------------------------------------------- */
/* Data structures */
/* ---------------------------------------------------------------------- */

/** Timer cell type. */
typedef struct SBEAML_TIMER_CELL SBEAML_TIMER_CELL;
/** Timer cell type. */
struct  SBEAML_TIMER_CELL {
    SBEAML_SYS_TICK_MSEC timeout_msec;
    SBEAML_SYS_TICK_MSEC expire_time_msec;
    bool expired;
    bool repeat;
};

/** Event handler cell type. */
typedef struct SBEAML_EVENT_HANDLER_CELL SBEAML_EVENT_HANDLER_CELL;
/** Event handler cell type. */
struct  SBEAML_EVENT_HANDLER_CELL {
    bool empty;     /* For machdep library only */

    SBEAML_EVENT_HANDLER_CELL *prev;
    SBEAML_EVENT_HANDLER handler;
    SBEAML_TIMER_CELL timers[SBEAML_CFG_MAX_TIMER];
};

/** Message cell type. */
typedef struct SBEAML_MESSAGE_CELL SBEAML_MESSAGE_CELL;
/** Message cell type. */
struct  SBEAML_MESSAGE_CELL {
    bool empty;     /* For machdep library only */

    SBEAML_MESSAGE_CELL *next;
    SBEAML_MESSAGE message;
};

#endif /* ndef SBEAML_PRIVATE_H_INCLUDED */
