/* ********************************************************************** */
/**
 * @brief   SBEAML: public API implementation.
 * @author  eel3
 * @date    2017-09-23
 */
/* ********************************************************************** */

#include "sbeaml.h"
#include "sbeaml_md.h"

#include <stddef.h>

#ifdef SBEAML_CFG_USE_ASSERT_H
#include <assert.h>
#else
#define assert(cond)
#endif

/* ---------------------------------------------------------------------- */
/* Constants */
/* ---------------------------------------------------------------------- */

/** Event handler tag: top event handler. */
#define SBEAML_EVENT_HANDLER_TAG_TOP (-1)
/** Event handler tag: all event handlers (except root event handler). */
#define SBEAML_EVENT_HANDLER_TAG_ALL (-2)

/* ---------------------------------------------------------------------- */
/* Data structures */
/* ---------------------------------------------------------------------- */

/** Timer handler cell type. */
typedef struct {
    SBEAML_SYS_TICK_MSEC timeout_msec;
    SBEAML_SYS_TICK_MSEC expire_time_msec;
    bool expired;
    bool repeat;
    SBEAML_TIMER_HANDLER handler;
} SBEAML_TIMER_HANDLER_CELL;

/** Module context type. */
typedef struct {
    bool initialized;
    bool prepared;

    /* Event handler stack. */
    SBEAML_EVENT_HANDLER_CELL *top_handler_cell;
    SBEAML_EVENT_HANDLER_CELL *next_top_handler_cell;

    /* Message queue. */
    SBEAML_MESSAGE_CELL *first_message_cell;
    SBEAML_MESSAGE_CELL *last_message_cell;

    /* Global timer's handlers */
    SBEAML_TIMER_HANDLER_CELL timers[SBEAML_CFG_MAX_GLOBAL_TIMER];
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
/* Private functions: dummy callback functions */
/* ---------------------------------------------------------------------- */

static void
dummy_on_init(void * const user_data)
{
    (void) user_data;
}

static void
dummy_on_appear(void * const user_data)
{
    (void) user_data;
}

static void
dummy_on_event(void * const user_data, const SBEAML_EVENT_ID id)
{
    (void) user_data, (void) id;
}

static void
dummy_on_timer(void * const user_data, const SBEAML_TIMER_ID id)
{
    (void) user_data, (void) id;
}

static void
dummy_on_disappear(void * const user_data)
{
    (void) user_data;
}

static void
dummy_on_destroy(void * const user_data)
{
    (void) user_data;
}

static void
dummy_release_user_data(void * const user_data)
{
    (void) user_data;
}

/* ---------------------------------------------------------------------- */
/* Private functions: for data structures */
/* ---------------------------------------------------------------------- */

/* ====================================================================== */
/**
 * @brief  Sanitize SBEAML_EVENT_HANDLER members.
 *
 * @param[in,out] handler  Event handler.
 */
/* ====================================================================== */
static void
seh_Sanitize(SBEAML_EVENT_HANDLER * const handler)
{
#define SANITIZE_FUNC(func) \
    if (handler->func == NULL) handler->func = dummy_##func

    assert(handler != NULL);

    SANITIZE_FUNC(on_init);
    SANITIZE_FUNC(on_appear);
    SANITIZE_FUNC(on_event);
    SANITIZE_FUNC(on_timer);
    SANITIZE_FUNC(on_disappear);
    SANITIZE_FUNC(on_destroy);
    SANITIZE_FUNC(release_user_data);

#undef SANITIZE_FUNC
}

/* ====================================================================== */
/**
 * @brief  Cleanup SBEAML_EVENT_HANDLER members.
 *
 * @param[out] handler  Event handler.
 */
/* ====================================================================== */
static void
seh_Cleanup(SBEAML_EVENT_HANDLER * const handler)
{
    assert(handler != NULL);

    handler->on_init = NULL;
    handler->on_appear = NULL;
    handler->on_event = NULL;
    handler->on_timer = NULL;
    handler->on_disappear = NULL;
    handler->on_destroy = NULL;
    handler->release_user_data = NULL;
    handler->user_data = NULL;
    handler->tag = SBEAML_EVENT_HANDLER_TAG_INVALID;
}

/* ====================================================================== */
/**
 * @brief  Sanitize SBEAML_GENERIC_HANDLER members.
 *
 * @param[in,out] handler  Generic handler.
 */
/* ====================================================================== */
static void
sgh_Sanitize(SBEAML_GENERIC_HANDLER * const handler)
{
#define SANITIZE_FUNC(func) \
    if (handler->func == NULL) handler->func = dummy_##func

    assert(handler != NULL);

    SANITIZE_FUNC(release_user_data);

#undef SANITIZE_FUNC
}

/* ====================================================================== */
/**
 * @brief  Sanitize SBEAML_TIMER_HANDLER members.
 *
 * @param[in,out] handler  Timer handler.
 */
/* ====================================================================== */
#define sth_Sanitize(handler) sgh_Sanitize((SBEAML_GENERIC_HANDLER *) (handler))

/* ====================================================================== */
/**
 * @brief  Sanitize SBEAML_MESSAGE members.
 *
 * @param[in,out] msg  Message.
 */
/* ====================================================================== */
#define sm_Sanitize(msg) sgh_Sanitize((SBEAML_GENERIC_HANDLER *) (msg))

/* ====================================================================== */
/**
 * @brief  Cleanup SBEAML_GENERIC_HANDLER members.
 *
 * @param[out] handler  Generic handler.
 */
/* ====================================================================== */
static void
sgh_Cleanup(SBEAML_GENERIC_HANDLER * const handler)
{
    assert(handler != NULL);

    handler->func = NULL;
    handler->release_user_data = NULL;
    handler->user_data = NULL;
}

/* ====================================================================== */
/**
 * @brief  Cleanup SBEAML_TIMER_HANDLER members.
 *
 * @param[out] handler  Timer handler.
 */
/* ====================================================================== */
#define sth_Cleanup(handler) sgh_Cleanup((SBEAML_GENERIC_HANDLER *) (handler))

/* ====================================================================== */
/**
 * @brief  Cleanup SBEAML_MESSAGE members.
 *
 * @param[out] msg  Message.
 */
/* ====================================================================== */
#define sm_Cleanup(msg) sgh_Cleanup((SBEAML_GENERIC_HANDLER *) (msg))

/* ====================================================================== */
/**
 * @brief  Initialize SBEAML_TIMER_CELL members.
 *
 * @param[out] cell  Timer cell.
 */
/* ====================================================================== */
static void
stc_Initialize(SBEAML_TIMER_CELL * const cell)
{
    assert(cell != NULL);

    cell->timeout_msec = 0;
    cell->expire_time_msec = 0;
    cell->expired = true;
    cell->repeat = false;
}

/* ====================================================================== */
/**
 * @brief  Initialize SBEAML_TIMER_HANDLER_CELL members.
 *
 * @param[out] cell  Timer handler cell.
 */
/* ====================================================================== */
static void
sthc_Initialize(SBEAML_TIMER_HANDLER_CELL * const cell)
{
    assert(cell != NULL);

    cell->timeout_msec = 0;
    cell->expire_time_msec = 0;
    cell->expired = true;
    cell->repeat = false;
    sth_Cleanup(&cell->handler);
}

/* ====================================================================== */
/**
 * @brief  Create a SBEAML_EVENT_HANDLER_CELL object.
 *
 * @param[in] handler  Event handler.
 *
 * @retval !=NULL  Exit success.
 * @retval   NULL  Exit failure.
 */
/* ====================================================================== */
static SBEAML_EVENT_HANDLER_CELL *
sehc_Create(const SBEAML_EVENT_HANDLER * const handler)
{
    SBEAML_EVENT_HANDLER_CELL *cell;
    size_t i;

    assert(handler != NULL);

    cell = sbeaml_md_AllocEventHandlerCell();
    if (cell == NULL) {
        return NULL;
    }

    cell->prev = NULL;
    cell->handler = *handler;
    seh_Sanitize(&cell->handler);
    for (i = 0; i < NELEMS(cell->timers); i++) {
        stc_Initialize(&cell->timers[i]);
    }

    return cell;
}

/* ====================================================================== */
/**
 * @brief  Delete the SBEAML_EVENT_HANDLER_CELL object.
 *
 * @param[in,out] cell  Event handler cell.
 */
/* ====================================================================== */
static void
sehc_Delete(SBEAML_EVENT_HANDLER_CELL * const cell)
{
    size_t i;

    assert(cell != NULL);

    cell->prev = NULL;
    seh_Cleanup(&cell->handler);
    for (i = 0; i < NELEMS(cell->timers); i++) {
        stc_Initialize(&cell->timers[i]);
    }

    sbeaml_md_DeallocEventHandlerCell(cell);
}

/* ====================================================================== */
/**
 * @brief  Create a SBEAML_MESSAGE_CELL object.
 *
 * @param[in] msg  Message.
 *
 * @retval !=NULL  Exit success.
 * @retval   NULL  Exit failure.
 */
/* ====================================================================== */
static SBEAML_MESSAGE_CELL *
smc_Create(const SBEAML_MESSAGE * const msg)
{
    SBEAML_MESSAGE_CELL *cell;

    assert(msg != NULL);

    cell = sbeaml_md_AllocMessageCell();
    if (cell == NULL) {
        return NULL;
    }

    cell->next = NULL;
    cell->message = *msg;
    sm_Sanitize(&cell->message);

    return cell;
}

/* ====================================================================== */
/**
 * @brief  Delete the SBEAML_MESSAGE_CELL object.
 *
 * @param[in,out] cell  Message cell.
 */
/* ====================================================================== */
static void
smc_Delete(SBEAML_MESSAGE_CELL * const cell)
{
    assert(cell != NULL);

    cell->next = NULL;
    sm_Cleanup(&cell->message);

    sbeaml_md_DeallocMessageCell(cell);
}

/* ---------------------------------------------------------------------- */
/* Private functions: process event handler stack */
/* ---------------------------------------------------------------------- */

static void
force_stop_timers(SBEAML_EVENT_HANDLER_CELL * const cell);

/* ====================================================================== */
/**
 * @brief  Validate event handler tag (on "pop handler" phase).
 *
 * @param[in] tag  Event handler tag.
 *
 * @retval true   Valid.
 * @retval false  Invalid.
 */
/* ====================================================================== */
static bool
valid_event_handler_tag(const SBEAML_EVENT_HANDLER_TAG tag)
{
    return tag > 0;
}

/* ====================================================================== */
/**
 * @brief  Validate event handler tag (on "push handler" phase).
 *
 * @param[in] tag  Event handler tag.
 *
 * @retval true   Valid.
 * @retval false  Invalid.
 */
/* ====================================================================== */
static bool
valid_event_handler_tag_on_push(const SBEAML_EVENT_HANDLER_TAG tag)
{
    return valid_event_handler_tag(tag) || (tag == SBEAML_EVENT_HANDLER_TAG_INVALID);
}

/* ====================================================================== */
/**
 * @brief  Return true if event handler stack is modified.
 *
 * @param[in,out] mc  Module context.
 *
 * @retval true   Modified.
 * @retval false  Not modified.
 */
/* ====================================================================== */
static bool
event_handler_stack_modified(MODULE_CTX * const mc)
{
    assert(mc != NULL);

    return mc->next_top_handler_cell != mc->top_handler_cell;
}

/* ====================================================================== */
/**
 * @brief  Push the root event handler to the stack.
 *
 * @param[in,out] mc            Module context.
 * @param[in]     root_handler  Event handler.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_PRM     Parameter error (perhaps arguments error).
 * @retval SBEAML_E_RES     No system resources.
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ====================================================================== */
static SBEAML_ERR
push_root_event_handler(MODULE_CTX * const mc,
                        const SBEAML_EVENT_HANDLER * const root_handler)
{
    SBEAML_EVENT_HANDLER_CELL *cell;

    assert((mc != NULL) && (root_handler != NULL));

    if (!valid_event_handler_tag_on_push(root_handler->tag)) {
        return SBEAML_E_PRM;
    }

    cell = sehc_Create(root_handler);
    if (cell == NULL) {
        return SBEAML_E_RES;
    }

    mc->top_handler_cell = cell;
    mc->next_top_handler_cell = cell;

    return SBEAML_E_OK;
}

/* ====================================================================== */
/**
 * @brief  Book to push the event handler to the stack.
 *
 * @param[in,out] mc       Module context.
 * @param[in]     handler  Event handler.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_PRM     Parameter error (perhaps arguments error).
 * @retval SBEAML_E_RES     No system resources.
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ====================================================================== */
static SBEAML_ERR
book_to_push_event_handler(MODULE_CTX * const mc,
                           const SBEAML_EVENT_HANDLER * const handler)
{
    SBEAML_EVENT_HANDLER_CELL *cell;

    assert((mc != NULL) && (handler != NULL));

    if (event_handler_stack_modified(mc)) {
        return SBEAML_E_STATUS;
    }

    if (!valid_event_handler_tag_on_push(handler->tag)) {
        return SBEAML_E_PRM;
    }

    cell = sehc_Create(handler);
    if (cell == NULL) {
        return SBEAML_E_RES;
    }

    cell->prev = mc->next_top_handler_cell;
    mc->next_top_handler_cell = cell;

    return SBEAML_E_OK;
}

/* ====================================================================== */
/**
 * @brief  Book to remove event handlers from the stack (except root handler).
 *
 * @param[in,out] mc   Module context.
 * @param[in]     tag  The next top event handler's tag.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_NG      Exit failure (perhaps stack is empty).
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ====================================================================== */
static SBEAML_ERR
book_to_pop_event_handler(MODULE_CTX * const mc,
                          const SBEAML_EVENT_HANDLER_TAG tag)
{
    SBEAML_EVENT_HANDLER_CELL *cell;

    assert(mc != NULL);

    if (event_handler_stack_modified(mc)) {
        return SBEAML_E_STATUS;
    }

    if (mc->next_top_handler_cell->prev == NULL) {
        /* Don't remove root event handler. */
        return SBEAML_E_NG;
    }

    switch (tag) {
    case SBEAML_EVENT_HANDLER_TAG_INVALID:
        assert(0);      /* Must not happen */
        return SBEAML_E_PRM;
    case SBEAML_EVENT_HANDLER_TAG_TOP:
        cell = mc->next_top_handler_cell;
        mc->next_top_handler_cell = cell->prev;
        cell->prev = NULL;
        break;
    case SBEAML_EVENT_HANDLER_TAG_ALL:
        do {
            cell = mc->next_top_handler_cell;
            mc->next_top_handler_cell = cell->prev;
        } while (mc->next_top_handler_cell->prev != NULL);
        cell->prev = NULL;
        break;
    default:
        if (mc->next_top_handler_cell->handler.tag == tag) {
            break;
        }
        do {
            cell = mc->next_top_handler_cell;
            mc->next_top_handler_cell = cell->prev;
            if (mc->next_top_handler_cell->handler.tag == tag) {
                break;
            }
        } while (mc->next_top_handler_cell->prev != NULL);
        cell->prev = NULL;
        break;
    }

    return SBEAML_E_OK;
}

/* ====================================================================== */
/**
 * @brief  Update event handler stack (apply push/pop operation).
 *
 * @param[in,out] mc  Module context.
 */
/* ====================================================================== */
static void
update_event_handler_stack(MODULE_CTX * const mc)
{
    SBEAML_EVENT_HANDLER *handler;
    SBEAML_EVENT_HANDLER_CELL *cell, *prev_cell;

    assert(mc != NULL);

    if (!event_handler_stack_modified(mc)) {
        /* No need to update. */
        return;
    }

    if (mc->top_handler_cell == mc->next_top_handler_cell->prev) {
        /* Booked to push. */
        handler = &mc->top_handler_cell->handler;
        handler->on_disappear(handler->user_data);

        mc->top_handler_cell = mc->next_top_handler_cell;

        handler = &mc->top_handler_cell->handler;
        handler->on_init(handler->user_data);
        handler->on_appear(handler->user_data);

        return;
    }

    /* Booked to pop. */
    for (cell = mc->top_handler_cell; cell != NULL; cell = prev_cell) {
        prev_cell = cell->prev;
        handler = &cell->handler;
        if (cell == mc->top_handler_cell) {
            /* Current top handler only. */
            handler->on_disappear(handler->user_data);
        }
        handler->on_destroy(handler->user_data);
        handler->release_user_data(handler->user_data);
        sehc_Delete(cell);
    }

    cell = mc->top_handler_cell = mc->next_top_handler_cell;

    force_stop_timers(cell);
    handler = &cell->handler;
    handler->on_appear(handler->user_data);
}

/* ====================================================================== */
/**
 * @brief  Remove all event handlers from the stack (including root handler).
 *
 * @param[in,out] mc  Module context.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_NG      Exit failure (perhaps stack is empty).
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ====================================================================== */
static void
pop_all_event_handlers(MODULE_CTX * const mc)
{
    bool booked_to_pop;
    SBEAML_EVENT_HANDLER_CELL *cell, *prev_cell;
    SBEAML_EVENT_HANDLER *handler;

    assert(mc != NULL);

    booked_to_pop = event_handler_stack_modified(mc);

    if (mc->top_handler_cell == mc->next_top_handler_cell->prev) {
        /* Booked to push. */
        cell = mc->next_top_handler_cell;
        handler = &cell->handler;
        handler->release_user_data(handler->user_data);
        sehc_Delete(cell);

        booked_to_pop = false;
    }

    for (cell = mc->top_handler_cell; cell != NULL; cell = prev_cell) {
        prev_cell = cell->prev;
        handler = &cell->handler;
        handler->on_destroy(handler->user_data);
        handler->release_user_data(handler->user_data);
        sehc_Delete(cell);
    }

    if (booked_to_pop) {
        for (cell = mc->next_top_handler_cell; cell != NULL; cell = prev_cell) {
            prev_cell = cell->prev;
            handler = &cell->handler;
            handler->release_user_data(handler->user_data);
            sehc_Delete(cell);
        }
    }

    mc->top_handler_cell = NULL;
    mc->next_top_handler_cell = NULL;
}

/* ---------------------------------------------------------------------- */
/* Private functions: process event */
/* ---------------------------------------------------------------------- */

/* ====================================================================== */
/**
 * @brief  Process a event.
 *
 * @param[in,out] mc  Module context.
 */
/* ====================================================================== */
static void
process_event(MODULE_CTX * const mc)
{
    SBEAML_EVENT_ID id;
    SBEAML_EVENT_HANDLER *handler;

    assert(mc != NULL);

    id = sbeaml_md_PeekEvent();
    if (id == SBEAML_EVENT_ID_NONE) {
        return;
    }

    handler = &mc->top_handler_cell->handler;
    handler->on_event(handler->user_data, id);

    update_event_handler_stack(mc);
}

/* ---------------------------------------------------------------------- */
/* Private functions: process timer */
/* ---------------------------------------------------------------------- */

/* ====================================================================== */
/**
 * @brief  Validate timer id.
 *
 * @param[in] mc  Module context.
 * @param[in] id  Timer ID.
 *
 * @retval true   Valid.
 * @retval false  Invalid.
 */
/* ====================================================================== */
static bool
valid_timer_id(const MODULE_CTX * const mc, const SBEAML_TIMER_ID id)
{
    assert(mc != NULL);

    return (size_t) id < NELEMS(mc->top_handler_cell->timers);
}

/* ====================================================================== */
/**
 * @brief  Create and start the software timer.
 *
 * @param[in,out] mc            Module context.
 * @param[in]     id            Timer ID.
 * @param[in]     timeout_msec  Timeout value (in milliseconds).
 * @param[in]     repeat        Repeatedly reschedule or not.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ====================================================================== */
static SBEAML_ERR
set_timer(MODULE_CTX * const mc,
          const SBEAML_TIMER_ID id,
          const SBEAML_SYS_TICK_MSEC timeout_msec,
          const bool repeat)
{
    SBEAML_TIMER_CELL *cell;

    assert((mc != NULL) && valid_timer_id(mc, id));

    cell = &mc->top_handler_cell->timers[id];
    if (!cell->expired) {
        return SBEAML_E_STATUS;
    }

    cell->timeout_msec = timeout_msec;
    cell->expire_time_msec = sbeaml_md_GetTick() + timeout_msec;
    cell->expired = false;
    cell->repeat = repeat;

    return SBEAML_E_OK;
}

/* ====================================================================== */
/**
 * @brief  Stop and delete the software timer.
 *
 * @param[in,out] mc  Module context.
 * @param[in]     id  Timer ID.
 */
/* ====================================================================== */
static void
kill_timer(MODULE_CTX * const mc, const SBEAML_TIMER_ID id)
{
    SBEAML_TIMER_CELL *cell;

    assert((mc != NULL) && valid_timer_id(mc, id));

    cell = &mc->top_handler_cell->timers[id];
    cell->expired = true;
}

/* ====================================================================== */
/**
 * @brief  Process current software timers.
 *
 * @param[in,out] mc  Module context.
 */
/* ====================================================================== */
static void
process_timers(MODULE_CTX * const mc)
{
    SBEAML_SYS_TICK_MSEC current_time;
    SBEAML_EVENT_HANDLER *handler;
    size_t i;

    assert(mc != NULL);

    current_time = sbeaml_md_GetTick();
    handler = &mc->top_handler_cell->handler;

    for (i = 0; i < NELEMS(mc->top_handler_cell->timers); i++) {
        SBEAML_TIMER_CELL *cell;

        cell = &mc->top_handler_cell->timers[i];
        if (cell->expired) {
            continue;
        }
        if ((current_time - cell->expire_time_msec) < 0) {
            continue;
        }
        if (cell->repeat) {
            cell->expire_time_msec += cell->timeout_msec;
        } else {
            cell->expired = true;
        }

        handler->on_timer(handler->user_data, (SBEAML_TIMER_ID) i);

        update_event_handler_stack(mc);
    }
}

/* ====================================================================== */
/**
 * @brief  Force stop current software timers.
 *
 * @param[in,out] cell  Event handler cell.
 */
/* ====================================================================== */
static void
force_stop_timers(SBEAML_EVENT_HANDLER_CELL * const cell)
{
    size_t i;

    assert(cell != NULL);

    for (i = 0; i < NELEMS(cell->timers); i++) {
        cell->timers[i].expired = true;
    }
}

/* ---------------------------------------------------------------------- */
/* Private functions: process global timer */
/* ---------------------------------------------------------------------- */

/* ====================================================================== */
/**
 * @brief  Validate global timer id.
 *
 * @param[in] mc  Module context.
 * @param[in] id  Timer ID.
 *
 * @retval true   Valid.
 * @retval false  Invalid.
 */
/* ====================================================================== */
static bool
valid_global_timer_id(const MODULE_CTX * const mc, const SBEAML_TIMER_ID id)
{
    assert(mc != NULL);

    return (size_t) id < NELEMS(mc->timers);
}

/* ====================================================================== */
/**
 * @brief  Initialize global software timers.
 *
 * @param[in,out] mc  Module context.
 */
/* ====================================================================== */
static void
initialize_global_timers(MODULE_CTX * const mc)
{
    size_t i;

    assert(mc != NULL);

    for (i = 0; i < NELEMS(mc->timers); i++) {
        sthc_Initialize(&mc->timers[i]);
    }
}

/* ====================================================================== */
/**
 * @brief  Create and start the global software timer.
 *
 * @param[in,out] mc            Module context.
 * @param[in]     id            Timer ID.
 * @param[in]     timeout_msec  Timeout value (in milliseconds).
 * @param[in]     repeat        Repeatedly reschedule or not.
 * @param[in]     handler       Timer handler.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_PRM     Parameter error (perhaps arguments error).
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ====================================================================== */
static SBEAML_ERR
set_global_timer(MODULE_CTX * const mc,
                 const SBEAML_TIMER_ID id,
                 const SBEAML_SYS_TICK_MSEC timeout_msec,
                 const bool repeat,
                 const SBEAML_TIMER_HANDLER * const handler)
{
    SBEAML_TIMER_HANDLER_CELL *cell;

    assert((mc != NULL) && valid_global_timer_id(mc, id) && (handler != NULL));

    if (handler->func == NULL) {
        return SBEAML_E_PRM;
    }

    cell = &mc->timers[id];
    if (!cell->expired) {
        return SBEAML_E_STATUS;
    }

    cell->timeout_msec = timeout_msec;
    cell->expire_time_msec = sbeaml_md_GetTick() + timeout_msec;
    cell->expired = false;
    cell->repeat = repeat;
    cell->handler = *handler;
    sth_Sanitize(&cell->handler);

    return SBEAML_E_OK;
}

/* ====================================================================== */
/**
 * @brief  Stop and delete the global software timer.
 *
 * @param[in,out] mc  Module context.
 * @param[in]     id  Timer ID.
 */
/* ====================================================================== */
static void
kill_global_timer(MODULE_CTX * const mc, const SBEAML_TIMER_ID id)
{
    SBEAML_TIMER_HANDLER_CELL *cell;
    SBEAML_TIMER_HANDLER *handler;

    assert((mc != NULL) && valid_global_timer_id(mc, id));

    cell = &mc->timers[id];
    if (cell->expired) {
        return;
    }
    cell->expired = true;
    handler = &cell->handler;
    handler->release_user_data(handler->user_data);
}

/* ====================================================================== */
/**
 * @brief  Process global software timers.
 *
 * @param[in,out] mc  Module context.
 */
/* ====================================================================== */
static void
process_global_timers(MODULE_CTX * const mc)
{
    SBEAML_SYS_TICK_MSEC current_time;
    size_t i;

    assert(mc != NULL);

    current_time = sbeaml_md_GetTick();

    for (i = 0; i < NELEMS(mc->timers); i++) {
        SBEAML_TIMER_HANDLER_CELL *cell;
        SBEAML_TIMER_HANDLER *handler;

        cell = &mc->timers[i];
        if (cell->expired) {
            continue;
        }
        if ((current_time - cell->expire_time_msec) < 0) {
            continue;
        }

        handler = &cell->handler;
        handler->func(handler->user_data);

        if (cell->repeat) {
            cell->expire_time_msec += cell->timeout_msec;
        } else {
            cell->expired = true;
            handler->release_user_data(handler->user_data);
        }

        update_event_handler_stack(mc);
    }
}

/* ====================================================================== */
/**
 * @brief  Force stop global software timers.
 *
 * @param[in,out] mc  Module context.
 */
/* ====================================================================== */
static void
force_stop_global_timers(MODULE_CTX * const mc)
{
    size_t i;

    assert(mc != NULL);

    for (i = 0; i < NELEMS(mc->timers); i++) {
        SBEAML_TIMER_HANDLER_CELL *cell;

        cell = &mc->timers[i];
        if (!cell->expired) {
            SBEAML_TIMER_HANDLER *handler;

            handler = &cell->handler;
            handler->release_user_data(handler->user_data);
        }
        sthc_Initialize(cell);
    }
}

/* ---------------------------------------------------------------------- */
/* Private functions: process message */
/* ---------------------------------------------------------------------- */

/* ====================================================================== */
/**
 * @brief  Post the message to the mein loop.
 *
 * @param[in,out] mc   Module context.
 * @param[in]     msg  Message.
 *
 * @retval SBEAML_E_OK   Exit success.
 * @retval SBEAML_E_PRM  Parameter error (perhaps arguments error).
 * @retval SBEAML_E_RES  No system resources.
 */
/* ====================================================================== */
static SBEAML_ERR
post_message(MODULE_CTX * const mc, const SBEAML_MESSAGE * const msg)
{
    SBEAML_MESSAGE_CELL *cell;

    assert((mc != NULL) && (msg != NULL));

    if (msg->func == NULL) {
        return SBEAML_E_PRM;
    }

    cell = smc_Create(msg);
    if (cell == NULL) {
        return SBEAML_E_RES;
    }

    if (mc->first_message_cell == NULL) {
        mc->first_message_cell = cell;
        mc->last_message_cell = cell;
    } else {
        mc->last_message_cell->next = cell;
        mc->last_message_cell = cell;
    }

    return SBEAML_E_OK;
}

/* ====================================================================== */
/**
 * @brief  Process all messages.
 *
 * @param[in,out] mc  Module context.
 */
/* ====================================================================== */
static void
process_messages(MODULE_CTX * const mc)
{
    SBEAML_MESSAGE_CELL *cell, *next_cell;
    SBEAML_MESSAGE *msg;

    assert(mc != NULL);

    sbeaml_md_LockForAPI();
    cell = mc->first_message_cell;
    mc->first_message_cell = NULL;
    mc->last_message_cell = NULL;
    sbeaml_md_UnlockForAPI();

    for (; cell != NULL; cell = next_cell) {
        next_cell = cell->next;
        msg = &cell->message;
        msg->func(msg->user_data);
        msg->release_user_data(msg->user_data);

        sbeaml_md_LockForAPI();
        smc_Delete(cell);
        sbeaml_md_UnlockForAPI();

        update_event_handler_stack(mc);
    }
}

/* ---------------------------------------------------------------------- */
/* Public API functions */
/* ---------------------------------------------------------------------- */

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
SBEAML_ERR
sbeaml_Initialize(void)
{
    MODULE_CTX * const mc = &module_ctx;
    SBEAML_ERR err;

    if (mc->initialized) {
        return SBEAML_E_STATUS;
    }

    err = sbeaml_md_Initialize();
    if (err != SBEAML_E_OK) {
        return err;
    }

    mc->prepared = false;
    mc->top_handler_cell = NULL;
    mc->next_top_handler_cell = NULL;
    mc->first_message_cell = NULL;
    mc->last_message_cell = NULL;

    mc->initialized = true;

    return SBEAML_E_OK;
}

/* ********************************************************************** */
/**
 * @brief  Finalize the library.
 */
/* ********************************************************************** */
void
sbeaml_Finalize(void)
{
    MODULE_CTX * const mc = &module_ctx;

    if (!mc->initialized) {
        return;
    }

    (void) sbeaml_CleanupAfterMainLoop();
    sbeaml_md_Finalize();

    mc->initialized = false;
}

/* ********************************************************************** */
/**
 * @brief  Prepare the library before main loop.
 *
 * @param[in] params  Preparation parameters.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_PRM     Parameter error (perhaps arguments error).
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
SBEAML_ERR
sbeaml_PrepareBeforeMainLoop(const SBEAML_PREPARE_PARAMS * const params)
{
    MODULE_CTX * const mc = &module_ctx;
    SBEAML_ERR err;
    SBEAML_EVENT_HANDLER *handler;

    if (params == NULL) {
        return SBEAML_E_PRM;
    }
    if (params->root_handler == NULL) {
        return SBEAML_E_PRM;
    }

    if (!mc->initialized) {
        return SBEAML_E_STATUS;
    }
    if (mc->prepared) {
        return SBEAML_E_STATUS;
    }

    err = sbeaml_md_PrepareBeforeMainLoop();
    if (err != SBEAML_E_OK) {
        return err;
    }

    err = push_root_event_handler(mc, params->root_handler);
    if (err != SBEAML_E_OK) {
        (void) sbeaml_md_CleanupAfterMainLoop();
        return err;
    }

    initialize_global_timers(mc);

    mc->prepared = true;

    handler = &mc->top_handler_cell->handler;
    handler->on_init(handler->user_data);
    handler->on_appear(handler->user_data);

    return SBEAML_E_OK;
}

/* ********************************************************************** */
/**
 * @brief  A resume-yield function for the main loop.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
SBEAML_ERR
sbeaml_ResumeAndYield(void)
{
    MODULE_CTX * const mc = &module_ctx;

    if (!mc->initialized) {
        return SBEAML_E_STATUS;
    }
    if (!mc->prepared) {
        return SBEAML_E_STATUS;
    }

    process_event(mc);
    process_timers(mc);
    process_global_timers(mc);
    process_messages(mc);

    return SBEAML_E_OK;
}

/* ********************************************************************** */
/**
 * @brief  Cleanup the library after main loop.
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
SBEAML_ERR
sbeaml_CleanupAfterMainLoop(void)
{
    MODULE_CTX * const mc = &module_ctx;

    if (!mc->initialized) {
        return SBEAML_E_STATUS;
    }
    if (!mc->prepared) {
        return SBEAML_E_STATUS;
    }

    process_messages(mc);
    force_stop_global_timers(mc);
    pop_all_event_handlers(mc);

    (void) sbeaml_md_CleanupAfterMainLoop();

    mc->prepared = false;

    return SBEAML_E_OK;
}

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
SBEAML_ERR
sbeaml_PushEventHandler(const SBEAML_EVENT_HANDLER * const handler)
{
    MODULE_CTX * const mc = &module_ctx;
    SBEAML_ERR err;

    if (handler == NULL) {
        return SBEAML_E_PRM;
    }

    if (!mc->initialized) {
        return SBEAML_E_STATUS;
    }
    if (!mc->prepared) {
        return SBEAML_E_STATUS;
    }

    err = book_to_push_event_handler(mc, handler);

    return err;
}

/* ********************************************************************** */
/**
 * @brief  Remove one event handler from the stack (except root handler).
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_NG      Exit failure (perhaps stack is empty).
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
SBEAML_ERR
sbeaml_PopEventHandler(void)
{
    MODULE_CTX * const mc = &module_ctx;
    SBEAML_ERR err;

    if (!mc->initialized) {
        return SBEAML_E_STATUS;
    }
    if (!mc->prepared) {
        return SBEAML_E_STATUS;
    }

    err = book_to_pop_event_handler(mc, SBEAML_EVENT_HANDLER_TAG_TOP);

    return err;
}

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
SBEAML_ERR
sbeaml_PopEventHandlerByTag(const SBEAML_EVENT_HANDLER_TAG tag)
{
    MODULE_CTX * const mc = &module_ctx;
    SBEAML_ERR err;

    if (!valid_event_handler_tag(tag)) {
        return SBEAML_E_PRM;
    }

    if (!mc->initialized) {
        return SBEAML_E_STATUS;
    }
    if (!mc->prepared) {
        return SBEAML_E_STATUS;
    }

    err = book_to_pop_event_handler(mc, tag);

    return err;
}

/* ********************************************************************** */
/**
 * @brief  Remove all event handlers from the stack (except root handler).
 *
 * @retval SBEAML_E_OK      Exit success.
 * @retval SBEAML_E_NG      Exit failure (perhaps stack is empty).
 * @retval SBEAML_E_STATUS  Internal status error.
 */
/* ********************************************************************** */
SBEAML_ERR
sbeaml_PopEventHandlerAll(void)
{
    MODULE_CTX * const mc = &module_ctx;
    SBEAML_ERR err;

    if (!mc->initialized) {
        return SBEAML_E_STATUS;
    }
    if (!mc->prepared) {
        return SBEAML_E_STATUS;
    }

    err = book_to_pop_event_handler(mc, SBEAML_EVENT_HANDLER_TAG_ALL);

    return err;
}

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
SBEAML_ERR
sbeaml_SetTimer(const SBEAML_TIMER_ID id,
                const SBEAML_SYS_TICK_MSEC timeout_msec,
                const bool repeat)
{
    MODULE_CTX * const mc = &module_ctx;
    SBEAML_ERR err;

    if (!mc->initialized) {
        return SBEAML_E_STATUS;
    }
    if (!mc->prepared) {
        return SBEAML_E_STATUS;
    }

    if (!valid_timer_id(mc, id)) {
        return SBEAML_E_PRM;
    }

    err = set_timer(mc, id, timeout_msec, repeat);

    return err;
}

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
SBEAML_ERR
sbeaml_KillTimer(const SBEAML_TIMER_ID id)
{
    MODULE_CTX * const mc = &module_ctx;

    if (!mc->initialized) {
        return SBEAML_E_STATUS;
    }
    if (!mc->prepared) {
        return SBEAML_E_STATUS;
    }

    if (!valid_timer_id(mc, id)) {
        return SBEAML_E_PRM;
    }

    kill_timer(mc, id);

    return SBEAML_E_OK;
}

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
SBEAML_ERR
sbeaml_SetGlobalTimer(const SBEAML_TIMER_ID id,
                      const SBEAML_SYS_TICK_MSEC timeout_msec,
                      const bool repeat,
                      const SBEAML_TIMER_HANDLER * const handler)
{
    MODULE_CTX * const mc = &module_ctx;
    SBEAML_ERR err;

    if (handler == NULL) {
        return SBEAML_E_PRM;
    }

    if (!mc->initialized) {
        return SBEAML_E_STATUS;
    }
    if (!mc->prepared) {
        return SBEAML_E_STATUS;
    }

    if (!valid_global_timer_id(mc, id)) {
        return SBEAML_E_PRM;
    }

    err = set_global_timer(mc, id, timeout_msec, repeat, handler);

    return err;
}

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
SBEAML_ERR
sbeaml_KillGlobalTimer(const SBEAML_TIMER_ID id)
{
    MODULE_CTX * const mc = &module_ctx;

    if (!mc->initialized) {
        return SBEAML_E_STATUS;
    }
    if (!mc->prepared) {
        return SBEAML_E_STATUS;
    }

    if (!valid_global_timer_id(mc, id)) {
        return SBEAML_E_PRM;
    }

    kill_global_timer(mc, id);

    return SBEAML_E_OK;
}

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
SBEAML_ERR
sbeaml_PostMessage(const SBEAML_MESSAGE * const msg)
{
    MODULE_CTX * const mc = &module_ctx;
    SBEAML_ERR err;

    if (msg == NULL) {
        return SBEAML_E_PRM;
    }

    sbeaml_md_LockForAPI();

    err = SBEAML_E_STATUS;

    if (!mc->initialized) {
        goto DONE;
    }
    if (!mc->prepared) {
        goto DONE;
    }

    err = post_message(mc, msg);

DONE:
    sbeaml_md_UnlockForAPI();

    return err;
}
