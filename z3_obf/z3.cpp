#include "/home/sentenced/Documents/Internships/2018_ETH/work/spec_ast/input/spec_repo/z3_obf/spec_fuzz.hpp"

#define OBFUSCATE

namespace fuzz {
namespace lib_helper_funcs {

z3::expr
ctor_expr(int n, z3::context& ctx)
{
    return ctx.int_val(n);
}

z3::expr
ctor_expr(std::string s, z3::context& ctx)
{
    return ctx.int_const(std::string(s).c_str());
}

z3::expr
div_wrapper(z3::expr const& e1, z3::expr const& e2)
{
    return z3::ite(e2 != 0, e1 / e2, e1);
}

z3::expr
mod_wrapper(z3::expr const& e1, z3::expr const& e2)
{
    return z3::ite(e2 != 0, z3::mod(e1, e2), e1);
}

z3::expr
rem_wrapper(z3::expr const& e1, z3::expr const& e2)
{
    return z3::ite(e2 != 0, z3::rem(e1, e2), e1);
}

z3::expr
pw_wrapper(z3::expr const& e1, z3::expr const& e2)
{
    return z3::ite(e1 != 0 && e2 != 0, z3::pw(e1, e2), e1);
}

} // namespace lib_helper_funcs
} // namespace fuzz

int
main(int argc, char** argv)
{
    z3::context ctx;
    fuzz::start();
    //std::string var_name = "var" + std::to_string(fuzz::fuzz_rand(0, 2000));
    //z3::expr e1 = ctx.int_const(var_name.c_str());
    //z3::expr e2 = ctx.int_val(fuzz::fuzz_rand(20, 30));
    z3::expr tmp_v = ctx.int_const("tmp_v");
    fuzz::output_var = ctx.int_const(std::string(fuzz::fuzz_rand<std::string, uint8_t>(3, 3)).c_str());
    fuzz::output_var = z3::operator+(fuzz::fuzz_new<z3::expr>(), fuzz::output_var);
    fuzz::end();
    fuzz::meta_test();
}
