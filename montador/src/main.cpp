#include <iostream>
#include <cstring>
#include <string>

#include "../include/preprocessor.hpp"
#include "assembler.h"
// #include "assemble2.h"

int main(int argc, char *argv[]) {

  std::string op_type, input_file;

  if(argc != 3) {
    std::cout << "Quantidade invalida de argumentos!" << std::endl;
    std::cout << "Eh necessario inserir o seguinte formato: -<op> <arquivo>" << std::endl;
    return 0;
  }

  op_type = argv[1];
  input_file = argv[2]; // recebe arquivo sem extensao

  if(op_type == "-p") {
    PreProcessor(input_file, ".pre");

  } else if(op_type == "-m") {
    PreProcessor(input_file, ".mcr");

  } else if(op_type == "-o") {
    char file_in[255], file_out[255];
    strcpy(file_in, argv[2]);
    strcat(file_in, ".mcr");
    strcpy(file_out, argv[2]);
    strcat(file_out, ".obj");
    
    assemble(file_in, file_out);

  } else {
    std::cout << "Operacao invalida!" << std::endl;

  }

  return 0;
}