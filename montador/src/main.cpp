#include <iostream>
#include <cstring>

#include "../include/preprocessor.hpp"

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
    std::cout << "processo de traducao" << std::endl;

  } else {
    std::cout << "Operacao invalida!" << std::endl;

  }

  return 0;
}