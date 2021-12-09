#include <string>
#include <vector>

#include "../lib/exprtk.hpp"

class varpair{
    public:
        std::string var;
        double val;

        varpair(std::string vn, double vl);
};

double getexpval(std::string equ, std::vector<varpair> v);