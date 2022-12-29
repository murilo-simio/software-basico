#ifndef ASSEMBLER_H__
#define ASSEMBLER_H__

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <map>
#include <sstream>
#include <set>
#include <vector>

#include "opcodes.h"

// class Linha{
//     std::string label, op, arg1, arg2;

//     Linha getToken(std::string s, int line);
// };

int tamanho_instr(const char* token);
char* string_maiusc(const char* s);
bool lexic_err(const std::string& label, int line);
void assemble(char* file_in, char* file_out);
int opcode(const std::string& str);


#endif // ASSEMBLER_H__