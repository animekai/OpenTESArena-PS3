#pragma once
// Compatibilidad newlib/PSL1GHT: funciones C99 que faltan en std::
#include <stdio.h>
#include <cstdio>

namespace std
{
    using ::snprintf;
    using ::vsnprintf;
}
