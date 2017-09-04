/* ********************************************************************** */
/**
 * @brief   SBEAML: inner event handler implementation (sample && test application).
 * @author  eel3
 * @date    2017-09-03
 */
/* ********************************************************************** */

#include "handler_public.h"
#include "handler_private.h"

#include "event_id.h"

#include <stdio.h>

/* ---------------------------------------------------------------------- */
/* File scope variables */
/* ---------------------------------------------------------------------- */

static char handler_name_prefix[] = "inner";

static const SBEAML_MESSAGE message_handler = {
    message_func,
    message_release_user_data,
    (void *) handler_name_prefix,
};

/* ---------------------------------------------------------------------- */
/* Private functions: for event handler */
/* ---------------------------------------------------------------------- */

static void
event_handler_on_event(void * const user_data, const SBEAML_EVENT_ID id)
{
    const char * const s = (const char *) user_data;
    EVENT_BIT bits = (EVENT_BIT) id;

    (void) printf("%s->event_handler->on_event(0x%08lX);\n", s, (unsigned long) id);

    switch (bits & EVENT_BITMASK_CMD) {
    case EVENT_BIT_CMD_PUSH_HANDLER:
        (void) sbeaml_PushEventHandler(&leaf_event_handler);
        break;
    case EVENT_BIT_CMD_POP_HANDLER:
        switch (bits & EVENT_BITMASK_POP_TYPE) {
        case EVENT_BIT_POP_TYPE_ONE:
            (void) sbeaml_PopEventHandler();
            break;
        case EVENT_BIT_POP_TYPE_TAG:
            (void) sbeaml_PopEventHandlerByTag((SBEAML_EVENT_HANDLER_TAG) (bits & EVENT_BITMASK_POP_TAG));
            break;
        case EVENT_BIT_POP_TYPE_ALL:
            (void) sbeaml_PopEventHandlerAll();
            break;
        default:
            break;
        }
        break;
    case EVENT_BIT_CMD_SET_TIMER:
        (void) sbeaml_SetTimer((SBEAML_TIMER_ID) ((bits & EVENT_BITMASK_TIMER_ID) >> 16),
                               (SBEAML_SYS_TICK_MSEC) (bits & EVENT_BITMASK_TIMER_TIMEOUT),
                               (bits & EVENT_BITMASK_TIMER_REPEAT) != 0 ? true : false);
        break;
    case EVENT_BIT_CMD_KILL_TIMER:
        (void) sbeaml_KillTimer((SBEAML_TIMER_ID) ((bits & EVENT_BITMASK_TIMER_ID) >> 16));
        break;
    case EVENT_BIT_CMD_SET_GTIMER:
        (void) sbeaml_SetGlobalTimer((SBEAML_TIMER_ID) ((bits & EVENT_BITMASK_TIMER_ID) >> 16),
                                     (SBEAML_SYS_TICK_MSEC) (bits & EVENT_BITMASK_TIMER_TIMEOUT),
                                     (bits & EVENT_BITMASK_TIMER_REPEAT) != 0 ? true : false,
                                     &global_timer_handler);
        break;
    case EVENT_BIT_CMD_KILL_GTIMER:
        (void) sbeaml_KillGlobalTimer((SBEAML_TIMER_ID) ((bits & EVENT_BITMASK_TIMER_ID) >> 16));
        break;
    case EVENT_BIT_CMD_POST_MESSAGE:
        (void) sbeaml_PostMessage(&message_handler);
        break;
    default:
        break;
    }
}

/* ---------------------------------------------------------------------- */
/* Global variables */
/* ---------------------------------------------------------------------- */

const SBEAML_EVENT_HANDLER inner_event_handler = {
    event_handler_on_init,
    event_handler_on_appear,
    event_handler_on_event,
    event_handler_on_timer,
    event_handler_on_disappear,
    event_handler_on_destroy,
    event_handler_release_user_data,
    (void *) handler_name_prefix,
    SBEAML_EVENT_HANDLER_TAG_INVALID,
};
