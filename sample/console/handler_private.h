/* ********************************************************************** */
/**
 * @brief   SBEAML: handler - private interfaces (sample && test application).
 * @author  eel3
 * @date    2017-09-03
 */
/* ********************************************************************** */

#ifndef HANDLER_PRIVATE_H_INCLUDED
#define HANDLER_PRIVATE_H_INCLUDED

#include "sbeaml.h"

/* ---------------------------------------------------------------------- */
/* Global variables */
/* ---------------------------------------------------------------------- */

extern const SBEAML_EVENT_HANDLER inner_event_handler;
extern const SBEAML_EVENT_HANDLER leaf_event_handler;
extern const SBEAML_TIMER_HANDLER global_timer_handler;

/* ---------------------------------------------------------------------- */
/* Functions */
/* ---------------------------------------------------------------------- */

extern void
event_handler_on_init(void * const user_data);
extern void
event_handler_on_appear(void * const user_data);
extern void
event_handler_on_timer(void * const user_data, const SBEAML_TIMER_ID id);
extern void
event_handler_on_disappear(void * const user_data);
extern void
event_handler_on_destroy(void * const user_data);
extern void
event_handler_release_user_data(void * const user_data);

extern void
timer_handler_func(void * const user_data);
extern void
timer_handler_release_user_data(void * const user_data);

extern void
message_func(void * const user_data);
extern void
message_release_user_data(void * const user_data);

#endif /* ndef HANDLER_PRIVATE_H_INCLUDED */
