/* ********************************************************************** */
/**
 * @brief   SBEAML: machdep data types (sample code).
 * @author  eel3
 * @date    2021-04-15
 */
/* ********************************************************************** */

#ifndef SBEAML_TYPES_H_INCLUDED
#define SBEAML_TYPES_H_INCLUDED

/* ---------------------------------------------------------------------- */
/* Data types */
/* ---------------------------------------------------------------------- */

/** Event ID type (must be greater than or equal to 0). */
typedef int32_t SBEAML_EVENT_ID;

/** "No event happen" event ID value. */
#define SBEAML_EVENT_ID_NONE (-1)

/**
 * System tick type (milliseconds).
 * You must select a signed integer types.
 */
typedef int32_t SBEAML_SYS_TICK_MSEC;

#endif /* ndef SBEAML_TYPES_H_INCLUDED */
