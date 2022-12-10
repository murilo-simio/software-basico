#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  
  FILE* input_file;
  std::string op_type;

  if(argc != 3) {
    std::cout << "Quantidade invalida de argumentos!" << std::endl;
    std::cout << "Eh necessario inserir: -<op> <arquivo>" << std::endl;
    return 0;
  }

  input_file = fopen(argv[2], "r");

  if(input_file == NULL) {
    std:: cout << "Arquivo nao existe!" << std::endl;
    return 0;
  }

  // char *buff;
  // char line[40];
  // while (!feof(input_file)) {
  //   buff = fgets(line, 40, input_file);
  //   if (buff) {
  //     std::cout << buff;
  //   }
  // }

  op_type = argv[1];

  if(op_type == "-p") {
    std::cout << "processa EQU e IF" << std::endl;
  } else if(op_type == "-m") {
    std::cout << "processa MACROS" << std::endl;
  } else if(op_type == "-o") {
    std::cout << "processo de traducao" << std::endl;
  } else {
    std::cout << "Operacao invalida!" << std::endl;
  }

  fclose(input_file);

  return 0;
}