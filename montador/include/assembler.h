#ifndef ASSEMBLER_H__
#define ASSEMBLER_H__

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <vector>

#include "opcodes.h"

void assemble(char* file_in, char* file_out);
std::string htod(const std::string& s);
bool lexic_err(const std::string& label, int line);
int opcode(const std::string& str);
char* string_maiusc(const char* s);
int tamanho_instr(const char* token);
std::string upper(std::string s);
std::string processa_primeiro_arg(std::string const& s, char t);
std::string processa_segundo_arg(std::string const& s, char t);
bool is_number(std::string const& s);

#endif // ASSEMBLER_H__
