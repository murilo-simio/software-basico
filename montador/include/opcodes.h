#include <map>
#include <string>

#define ADD 1
#define SUB 2
#define MULT 3
#define DIV 4
#define JMP 5
#define JMPN 6
#define JMPP 7
#define JMPZ 8
#define COPY 9
#define LOAD 10
#define STORE 11
#define INPUT 12
#define OUTPUT 13
#define STOP 14

#define SPACE_BAR ' '
#define DOIS_PTS ':'
#define PONTO_VIR ';'
#define VIRGULA ','

std::map<std::string, int> operacoes = 
{
    {"ADD",    1},   
    {"SUB",    2},
    {"MUL",    3},
    {"DIV",    4},
    {"JMP",    5},
    {"JMPN",   6},
    {"JMPP",   7},
    {"JMPZ",   8},
    {"COPY",   9},
    {"LOAD",   10},
    {"STORE",  11},
    {"INPUT",  12},
    {"OUTPUT", 13},
    {"STOP",   14}
};
