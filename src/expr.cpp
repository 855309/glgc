#include "expr.hpp"

varpair::varpair(std::string vn, double vl){
    this->var = vn;
    this->val = vl;
}

double getexpval(std::string equ, std::vector<varpair> v){
    exprtk::symbol_table<double> symbol_table;
    symbol_table.add_constants();

    for(varpair p : v){
        symbol_table.add_constant(p.var, p.val);
    }

    exprtk::expression<double> expression;
    expression.register_symbol_table(symbol_table);

    exprtk::parser<double> parser;
    parser.compile(equ.c_str(), expression);

    return expression.value();
}