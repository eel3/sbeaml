/* ********************************************************************** */
/**
 * @brief   SBEAML: machdep interfaces for submodules (sample code).
 * @author  eel3
 * @date    2017-09-07
 */
/* ********************************************************************** */

#ifndef SBEAML_MD_EQ_H_INCLUDED
#define SBEAML_MD_EQ_H_INCLUDED

#include "sbeaml.h"

/* ---------------------------------------------------------------------- */
/* Public API Functions */
/* ---------------------------------------------------------------------- */

/* ********************************************************************** */
/**
 * @brief  Post event ID to the event queue.
 *
 * @param[in] id  Event ID.
 *
 * @retval true  Exit success.
 * @retval false Exit failure.
 */
/* ********************************************************************** */
extern bool
sbeaml_md_PostEvent(const SBEAML_EVENT_ID id);

#endif /* ndef SBEAML_MD_EQ_H_INCLUDED */
