#ifndef ASSEMBLER_H__
#define ASSEMBLER_H__

#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <set>

#include "opcodes.h"

class Linha{
    std::string label, op, arg1, arg2;

    Linha getToken(std::string s, int line);
};

#endif // ASSEMBLER_H__