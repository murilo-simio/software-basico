#include "../include/preprocessor.hpp"
#include <iostream>
#include <fstream>
#include <cstring>
#include <map>
#include <vector>
#include <algorithm>

// 3 formatos reconhecidos:
// 1 - Op
// 2 - Op End
// 3 - Op End1 End2
// (lembrar dos comentarios)


// Diretiva EQU sempre vai estar antes de SECTION TEXT - aceita ate 2 entradas
// Diretiva IF eh acompanhada de rotulo EQU
// EQU pode ser argumento de SPACE e CONST

std::map <std::string, std::string> ext_map = {{".pre", ".asm"}, {".mcr", ".pre"}, {".obj", ".mcr"}};

void PreProcessor(std::string in, std::string extension) {
    std::map <std::string, std::string>* equ_table = new std::map<std::string, std::string>();
    std::map <std::string, std::vector<std::string>>* macro_table = new std::map <std::string, std::vector<std::string>>();
    std::vector<std::string>* file_content = new std::vector<std::string>();
    std::ifstream input_file;
    std::string line;
    std::string file_name;
    std::string str;
    bool line_label = false;

    // Concatena a extensao do arquivo de entrada
    file_name = in + ext_map.find(extension)->second;
    
    input_file.open(file_name);

    if(!input_file) {
        std::cout << "Nao foi possivel abrir o arquivo!" << std::endl;
        return ;
    }

    while(!input_file.eof()) {
        getline(input_file, line);

        if(line[line.length() - 1] == ':') {
            line_label = true;
            str = line;
            continue;
        }
        // Desconsidera comentario e linha em branco
        if(line[0] != ';' && line.length() > 0) {
            if(line_label) {    // para o caso da linha conter apenas rotulo
                line = str + " " + line;
                line_label = false;
            }
            std::transform(line.begin(), line.end(), line.begin(), ::toupper);
            file_content->push_back(line);
        }

    }

    input_file.close();

    if(extension == ".pre") {
        if(FindEQU(file_content, equ_table)) {
            ProcessEquIf(file_content, equ_table, in+extension);
        } else {
            std::cout << "tt1\n";
        }
        return;
    }

    if(extension == ".mcr") {
        if(FindMacro(file_content, macro_table)) {
            std::cout << "Prox etapa: Processar macros" << std::endl;
            // ProcessMacro(file_content, macro_table, in+extension);
            return;
        } else {
            std::cout << "tt2" << std::endl;
        }

    }

}



bool FindMacro(std::vector<std::string>* file_content, std::map <std::string, std::vector<std::string>>* macro_table) {
    std::vector<std::string> body;
    std::string name;
    std::string line;
    char* line_token;
    char str[100];
    char* token[6];
    bool is_macro = false;
    bool macro_exists = false;
    int pos;
    int begin_macro;

    for(long unsigned int i = 0; i < file_content->size(); i++) {
        strcpy(str, (*file_content)[i].c_str());
        line = str;
        line_token = strtok(str, " ");

        int j = 0;
        
        while(line_token) {
            token[j] = line_token;
            j++;
            // aponta para proximo token da linha
            line_token = strtok(NULL, " ");
        }

        if(strcmp(token[1], "MACRO") == 0) {
            is_macro = true;
            macro_exists = true;
            name = token[0];
            begin_macro = i;
            pos = line.find(token[2]); // Busca posicao inicial dos args da macro
            line = line.substr(pos, line.length()); // Salva so os args
            body.push_back(line);
            continue;
        }

        if(strcmp(token[0], "ENDMACRO") == 0) {
            (*macro_table)[name.substr(0, name.length() - 1)] = body;
            (*file_content).erase((*file_content).begin() + begin_macro, (*file_content).begin() + i + 1);
            i = begin_macro - 1;    // retorna 1 posicao antes de iniciar a macro
            body.clear();
            is_macro = false;
        }

        if(is_macro) {
            body.push_back(line);
            continue;
        }

    }

    // std::map <std::string, std::vector<std::string>>::iterator it;
    // for(it = (*macro_table).begin(); it != (*macro_table).end(); ++it){
    //     std::cout << it->first  << std::endl;
    //     std::cout << "-----------" << std::endl;
    //     for (auto pos = it->second.begin(); pos != it->second.end(); ++pos)
    //         std::cout << *pos << std::endl;
    //     std::cout << "-----------" << std::endl;
    // }

    return macro_exists;

}





void ProcessEquIf(std::vector<std::string>* file_content, std::map <std::string, std::string>* equ_table, std::string file_name) {
    std::fstream output_file;
    char* line_token;
    char str[100];
    char* token[4];

    output_file.open(file_name, std::ios::out);

    if(!output_file) {
        std::cout << "Nao foi possivel criar o arquivo!" << std::endl;
        return ;
    }

    for(long unsigned int i = 0; i < file_content->size(); i++) {
        strcpy(str, (*file_content)[i].c_str());

        line_token = strtok(str, " ");

        int j = 0;
        while(line_token) {
            token[j] = line_token;
            j++; // armazena qtd de tokens criados
            line_token = strtok(NULL, " "); // aponta para proximo token da linha
        }

        if(strcmp(token[0], "IF") == 0) {
            // Desconsidera prox linha se a flag for 0
            if((*equ_table).find(token[1])->second == "0") {
                i++;
            }
            continue;
        }
        
        // Instrucoes do tipo "Op"
        if(j == 1) {
            output_file << token[0];
            output_file << "\n";
            continue;
        }
        
        // Instrucoes do tipo "Op End" ou "ROTULO: SPACE"
        if(j == 2) {
            output_file << token[0];
            output_file << " ";
            output_file << token[1];
            output_file << "\n";
            continue;
        }

        // Instrucoes do tipo "Op End1 End2" ou "ROTULO: SPACE/CONST VALOR"
        if(j == 3) {
            output_file << token[0];
            output_file << " ";
            output_file << token[1];
            output_file << " ";

            if (strcmp(token[1], "SPACE") == 0 || strcmp(token[1], "CONST") == 0) {
                if((*equ_table).find(token[2]) != (*equ_table).end()) {
                    output_file << (*equ_table).find(token[2])->second;
                    output_file << "\n";
                    continue;
                }

            }

            output_file << token[2];
            output_file << "\n";
            continue;
        }

        // Instrucao do tipo "ROTULO: COPY END1,END2"
        if(j == 4) {
            output_file << token[0];
            output_file << " ";
            output_file << token[1];
            output_file << " ";
            output_file << token[2];
            output_file << token[3];
            output_file << "\n";
        }

    }

    output_file.close();
    std::cout << "O arquivo foi criado com sucesso!" << std::endl;

}

bool FindEQU(std::vector<std::string>* file_content, std::map <std::string, std::string>* equ_table) {
    char* line_token;
    char* label;
    char* value;
    char str[100];
    char* token[4];
    bool equ_exists = false;
    std::vector<std::string>::iterator pos;

    for(long unsigned int i = 0; i < file_content->size(); i++) {

        strcpy(str, (*file_content)[i].c_str());
        line_token = strtok(str, " ");

        int j = 0;
        while(line_token) {
            token[j] = line_token;
            j++; // armazena qtd de tokens criados
            line_token = strtok(NULL, " "); // aponta para proximo token da linha
        }

        if(strcmp(token[0], "SECTION") == 0) {
            // Remove as instrucoes de EQU do vetor
            (*file_content).erase((*file_content).begin(), (*file_content).begin() + i);

            // std::map<std::string, std::string>::iterator it;
            // for(it = (*equ_table).begin(); it != (*equ_table).end(); ++it){
            //     std::cout << it->first << " => " << it->second << std::endl;
            // }

            if(equ_table->size() > 0) {
                equ_exists = true;
            }
            return equ_exists;
        }

        token[0][strlen(token[0]) - 1] = '\0';  // desconsidera ":" no final do rotulo
        label = token[0];
        value = token[2];
        (*equ_table)[label] = value;

    }
    return equ_exists;

}



