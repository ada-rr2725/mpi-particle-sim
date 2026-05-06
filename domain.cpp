#include "domain.h"

using namespace std;

int owner_of(double x_pos, int p)
{
    int owner = static_cast<int>(x_pos * p / x_max);
    if (owner >= p) owner = p - 1;
    if (owner < 0)  owner = 0;
    return owner;
}
