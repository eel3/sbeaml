/* ********************************************************************** */
/**
 * @brief   SBEAML: handler - common implementation (sample && test application).
 * @author  eel3
 * @date    2017-09-03
 */
/* ********************************************************************** */

#include "handler_public.h"
#include "handler_private.h"

#include <stdio.h>

/* ---------------------------------------------------------------------- */
/* File scope variables */
/* ---------------------------------------------------------------------- */

static char global_timer_prefix[] = "global";

/* ---------------------------------------------------------------------- */
/* Functions: for event handler */
/* ---------------------------------------------------------------------- */

void
event_handler_on_init(void * const user_data)
{
    const char * const s = (const char *) user_data;

    (void) printf("%s->event_handler->on_init();\n", s);
}

void
event_handler_on_appear(void * const user_data)
{
    const char * const s = (const char *) user_data;

    (void) printf("%s->event_handler->on_appear();\n", s);
}

void
event_handler_on_timer(void * const user_data, const SBEAML_TIMER_ID id)
{
    const char * const s = (const char *) user_data;

    (void) printf("%s->event_handler->on_timer(%lu);\n", s, (unsigned long) id);
}

void
event_handler_on_disappear(void * const user_data)
{
    const char * const s = (const char *) user_data;

    (void) printf("%s->event_handler->on_disappear();\n", s);
}

void
event_handler_on_destroy(void * const user_data)
{
    const char * const s = (const char *) user_data;

    (void) printf("%s->event_handler->on_destroy();\n", s);
}

void
event_handler_release_user_data(void * const user_data)
{
    const char * const s = (const char *) user_data;

    (void) printf("%s->event_handler->release_user_data();\n", s);
}

/* ---------------------------------------------------------------------- */
/* Private functions: for timer handler */
/* ---------------------------------------------------------------------- */

void
timer_handler_func(void * const user_data)
{
    const char * const s = (const char *) user_data;

    (void) printf("%s->timer_handler->func();\n", s);
}

void
timer_handler_release_user_data(void * const user_data)
{
    const char * const s = (const char *) user_data;

    (void) printf("%s->timer_handler->release_user_data();\n", s);
}

/* ---------------------------------------------------------------------- */
/* Private functions: for message */
/* ---------------------------------------------------------------------- */

void
message_func(void * const user_data)
{
    const char * const s = (const char *) user_data;

    (void) printf("%s->message->func();\n", s);
}

void
message_release_user_data(void * const user_data)
{
    const char * const s = (const char *) user_data;

    (void) printf("%s->message->release_user_data();\n", s);
}

/* ---------------------------------------------------------------------- */
/* Global variables */
/* ---------------------------------------------------------------------- */

const SBEAML_TIMER_HANDLER global_timer_handler = {
    timer_handler_func,
    timer_handler_release_user_data,
    (void *) global_timer_prefix,
};

const SBEAML_MESSAGE default_message = {
    message_func,
    message_release_user_data,
    NULL,
};
