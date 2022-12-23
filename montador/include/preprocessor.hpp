#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <string>
#include <map>
#include <vector>

void PreProcessor(std::string, std::string);
bool FindEQU(std::vector<std::string>*, std::map <std::string, std::string>*);
void ProcessEquIf(std::vector<std::string>*, std::map <std::string, std::string>*, std::string);
// void ProcessMacro(std::vector<std::string>*, std::map <std::string, std::vector<std::string>>*, std::string);
bool FindMacro(std::vector<std::string>*, std::map <std::string, std::vector<std::string>>*);

#endif