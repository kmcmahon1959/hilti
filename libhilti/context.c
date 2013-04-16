
#include <stdio.h>

#include "memory_.h"
#include "context.h"
#include "rtti.h"
#include "globals.h"
#include "exceptions.h"
#include "config.h"
#include "profiler.h"

// Generated by the HILTI linker. It tells us how many bytes we need to store
// all thread-global variables.  We also declare it here weak so that we can
// link with the library even if the function is not present.
__attribute__ ((weak)) int64_t __hlt_globals_size = 0;

// These functions initialize/destroy all the globals in the passed context
// to their default values. They are normally generated by the HILTI linker,
// but we provide a "weak" dummy implementation in case no module needs any of this.
__attribute__ ((weak)) void __hlt_globals_init(hlt_execution_context* ctx) {}
__attribute__ ((weak)) void __hlt_globals_dtor(hlt_execution_context* ctx) {}

void hlt_execution_context_dtor(hlt_type_info* ti, hlt_execution_context* ctx)
{
    __hlt_globals_dtor(ctx);

    hlt_exception* excpt = 0;
    GC_DTOR(ctx->excpt, hlt_exception);

    if ( ctx->fiber )
        hlt_fiber_delete(ctx->fiber);

    if ( ctx->pstate )
        __hlt_profiler_state_delete(ctx->pstate);

    if ( ctx->tcontext ) {
        GC_DTOR_GENERIC(&ctx->tcontext, ctx->tcontext_type);
    }

    hlt_free_list_delete(ctx->fiber_pool);
}

__HLT_RTTI_GC_TYPE(hlt_execution_context, HLT_TYPE_CONTEXT)

hlt_execution_context* __hlt_execution_context_new(hlt_vthread_id vid)
{
    hlt_execution_context* ctx = (hlt_execution_context*)
        GC_NEW_CUSTOM_SIZE(hlt_execution_context, sizeof(hlt_execution_context) + __hlt_globals_size);

    ctx->vid = vid;

    __hlt_globals_init(ctx);

    ctx->excpt = 0;
    ctx->fiber = 0;
    ctx->fiber_pool = hlt_free_list_new(hlt_config_get()->fiber_stack_size);
    ctx->worker = 0;
    ctx->tcontext = 0;
    ctx->tcontext_type = 0;
    ctx->pstate = 0;
    ctx->blockable = 0;

    return ctx;
}

void __hlt_context_set_exception(hlt_execution_context* ctx, hlt_exception* excpt)
{
    GC_ASSIGN(ctx->excpt, excpt, hlt_exception);
}

hlt_exception* __hlt_context_get_exception(hlt_execution_context* ctx)
{
    return ctx->excpt;
}

void __hlt_context_clear_exception(hlt_execution_context* ctx)
{
    GC_CLEAR(ctx->excpt, hlt_exception);
}

hlt_fiber* __hlt_context_get_fiber(hlt_execution_context* ctx)
{
    return ctx->fiber;
}

void __hlt_context_set_fiber(hlt_execution_context* ctx, hlt_fiber* fiber)
{
    ctx->fiber = fiber;
}

void* __hlt_context_get_thread_context(hlt_execution_context* ctx)
{
    return ctx->tcontext;
}

void __hlt_context_set_thread_context(hlt_execution_context* ctx, hlt_type_info* type, void* tctx)
{
    GC_DTOR_GENERIC(&ctx->tcontext, ctx->tcontext_type);
    ctx->tcontext = tctx;
    ctx->tcontext_type = type;
    GC_CCTOR_GENERIC(&ctx->tcontext, ctx->tcontext_type);
}

hlt_vthread_id __hlt_context_get_vid(hlt_execution_context* ctx)
{
    return ctx->vid;
}

void __hlt_context_set_blockable(hlt_execution_context* ctx, __hlt_thread_mgr_blockable* b)
{
    ctx->blockable = b;
}
