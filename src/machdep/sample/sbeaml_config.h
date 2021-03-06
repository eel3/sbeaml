/* ********************************************************************** */
/**
 * @brief   SBEAML: configurations (sample code).
 * @author  eel3
 * @date    2017-09-07
 */
/* ********************************************************************** */

#ifndef SBEAML_CONFIG_H_INCLUDED
#define SBEAML_CONFIG_H_INCLUDED

/* ---------------------------------------------------------------------- */
/* Configurations for the library */
/* ---------------------------------------------------------------------- */

/** Maximum number of timers. */
#define SBEAML_CFG_MAX_TIMER 8

/** Maximum number of global timers. */
#define SBEAML_CFG_MAX_GLOBAL_TIMER 8

#if 0
/** Use C standard library's assert.h (for debug on hosted environment). */
#define SBEAML_CFG_USE_ASSERT_H
#endif

/* ---------------------------------------------------------------------- */
/* Configurations for the machdep library (for sample code only) */
/* ---------------------------------------------------------------------- */

/** Maximum number of event handlers. */
#define SBEAML_CFG_MAX_EVENT_HANDLER 16

/** Maximum number of messages. */
#define SBEAML_CFG_MAX_MESSAGE 16

/** Maximum size of event queue. */
#define SBEAML_CFG_EVENT_QUEUE_SIZE 32

#endif /* ndef SBEAML_CONFIG_H_INCLUDED */
