#ifdef EXECUTE
#include "z3++.h"
#include <cassert>
typedef std::pair<z3::expr, z3::expr> mr_pair;
#else
#include "z3_spec_defs.hpp"
#endif

#define POW_LIM 32.0
#define FPA_PREC 32
#define FP_RM_COUNT_MAX 4

Z3_ast (*get_rm_expr)(Z3_context);

namespace fuzz {

namespace lib_helper_funcs {

z3::expr
ctor_expr(float n, z3::context& ctx)
{
    return ctx.fpa_val((float)n);
}

z3::expr
ite_lt_wrapper(z3::expr cond1, z3::expr cond2, z3::expr if_b, z3::expr then_b)
{
    return z3::ite(cond1 < cond2, if_b, then_b);
}

z3::expr
ite_gte_wrapper(z3::expr cond1, z3::expr cond2, z3::expr if_b, z3::expr then_b)
{
    return z3::ite(cond1 >= cond2, if_b, then_b);
}

z3::expr
div_wrapper(z3::expr const& e1, z3::expr const& e2)
{
    return z3::ite(e2 != e1.ctx().fpa_val((float)0.0), e1 / e2, e1);
}

z3::expr
fma_wrapper(z3::expr const& e1, z3::expr const& e2, z3::expr const& e3)
{
    Z3_ast rm_c;
    std::string rm_name = e1.ctx().fpa_rounding_mode().to_string();
    if (!rm_name.compare("roundNearestTiesToAway"))
    {
        rm_c = Z3_mk_fpa_rna(Z3_context(e1.ctx()));
    }
    else if (!rm_name.compare("roundNearestTiesToEven"))
    {
        rm_c = Z3_mk_fpa_rne(Z3_context(e1.ctx()));
    }
    else if (!rm_name.compare("roundTowardPositive"))
    {
        rm_c = Z3_mk_fpa_rtp(Z3_context(e1.ctx()));
    }
    else if (!rm_name.compare("roundTowardNegative"))
    {
        rm_c = Z3_mk_fpa_rtn(Z3_context(e1.ctx()));
    }
    else if (!rm_name.compare("roundTowardZero"))
    {
        rm_c = Z3_mk_fpa_rtz(Z3_context(e1.ctx()));
    }
    else
    {
        assert(false);
    }
    return z3::fma(e1, e2, e3, z3::expr(e1.ctx(), rm_c)); }

} // namespace lib_helper_funcs
} // namespace fuzz

int
main(int argc, char** argv)
{
    z3::context ctx;

    z3::expr cnst_var1 = ctx.fpa_const<FPA_PREC>("x");
    z3::expr cnst_var2 = ctx.fpa_const<FPA_PREC>("y");
    z3::expr cnst_var3 = ctx.fpa_const<FPA_PREC>("z");

    ctx.set_rounding_mode(static_cast<z3::rounding_mode>(fuzz::fuzz_rand<int, int>(0, FP_RM_COUNT_MAX)));

    fuzz::start();
    z3::expr lhs = fuzz::fuzz_new<z3::expr>();
    z3::expr rhs = fuzz::fuzz_new<z3::expr>();
    fuzz::output_var = std::make_pair(lhs, rhs);
    fuzz::end();
    assert(ctx.check_error() == Z3_OK);

    z3::solver s(ctx);
    z3::expr (*chk_op)(z3::expr const&, z3::expr const&){ &z3::operator< };
    s.add((*chk_op)(fuzz::output_var_get(0).first, fuzz::output_var_get(0).second));
    if (s.check() != z3::sat)
    {
        chk_op = &z3::operator>=;
        s.reset();
        s.add((*chk_op)(fuzz::output_var_get(0).first, fuzz::output_var_get(0).second));
        if (s.check() != z3::sat)
        {
            std::cout << "Non-SAT formula." << std::endl;
            exit(0);
        }
    }
    z3::model out_model = s.get_model();

    for (z3::expr cnst_expr : {cnst_var1, cnst_var2, cnst_var3})
    {
        z3::func_decl cnst_decl = cnst_expr.decl();
        if (!out_model.has_interp(cnst_decl))
        {
            z3::expr zero_val = ctx.fpa_val(0.0);
            out_model.add_const_interp(cnst_decl, zero_val);
        }
    }
    fuzz::meta_test();
}
