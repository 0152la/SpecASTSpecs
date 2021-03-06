#define BV_SIZE 20

#ifdef EXECUTE
#include "yices.h"
#include <cassert>
#else
#include "yices_spec_defs.hpp"
#endif

namespace fuzz {
namespace lib_helper_funcs {

term_t
ctor_expr(int n)
{
    return yices_bvconst_uint32(BV_SIZE, n);
}

term_t
ite_lt_wrapper(term_t cond1, term_t cond2, term_t then_e, term_t else_e)
{
    return yices_ite(yices_bvlt_atom(cond1, cond2), then_e, else_e);
}

term_t
ite_gte_wrapper(term_t cond1, term_t cond2, term_t then_e, term_t else_e)
{
    return yices_ite(yices_bvge_atom(cond1, cond2), then_e, else_e);
}

term_t
rotate_left_wrapper(term_t t, unsigned int n)
{
    return yices_rotate_left(t, n % (BV_SIZE + 1));
}

term_t
rotate_right_wrapper(term_t t, unsigned int n)
{
    return yices_rotate_right(t, n % (BV_SIZE + 1));
}

} // namespace lib_helper_funcs
} // namespace fuzz

int
main(int argc, char** argv)
{
    yices_init();

    ctx_config_t* config = yices_new_config();
    yices_default_config_for_logic(config, "QF_BV");
    yices_set_config(config, "mode", "one-shot");
    context_t* ctx = yices_new_context(config);

    term_t x = yices_new_uninterpreted_term(yices_bv_type(BV_SIZE));
    term_t y = yices_new_uninterpreted_term(yices_bv_type(BV_SIZE));
    term_t z = yices_new_uninterpreted_term(yices_bv_type(BV_SIZE));

    fuzz::start();
    term_t lhs = fuzz::fuzz_new<term_t>();
    term_t rhs = fuzz::fuzz_new<term_t>();
    fuzz::output_var = yices_bvadd(lhs, rhs);
    fuzz::end();

    assert(!yices_error_code());

    fuzz::meta_test();

    yices_exit();
}

