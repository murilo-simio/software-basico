#include "../include/preprocessor.hpp"
#include <iostream>
#include <fstream>
#include <cstring>
#include <map>
#include <vector>
#include <algorithm>

#define WHITESPACE " "
#define NEWLINE "\n"

std::map <std::string, std::string> ext_map = {{".pre", ".asm"}, {".mcr", ".pre"}, {".obj", ".mcr"}};

void PreProcessor(std::string in, std::string extension) {
    std::map <std::string, std::string>* equ_table = new std::map<std::string, std::string>();
    std::map <std::string, std::vector<std::string>>* macro_table = new std::map <std::string, std::vector<std::string>>();
    std::vector<std::string>* file_content = new std::vector<std::string>();
    std::ifstream input_file;
    std::string line;
    std::string file_name;
    std::string str;
    bool label_alone = false;

    // Concatena a extensao do arquivo de entrada
    file_name = in + ext_map.find(extension)->second;
    
    // Abre o arquivo de entrada
    input_file.open(file_name);
    if(!input_file) {
        std::cout << "Nao foi possivel abrir o arquivo!" << std::endl;
        return ;
    }

    while(!input_file.eof()) {
        getline(input_file, line);

        // caso a linha contenha apenas rotulo, salva o rotulo para concatenar com o resto da instr
        // que deve estar na prox linha
        if(line[line.length() - 1] == ':') {
            label_alone = true;
            str = line;
            continue;
        }

        line = line.substr(0, line.find(";"));  // retira comentario

        if(line.length() > 0) {
            if(label_alone) {    // para o caso da linha conter apenas rotulo
                line = str + " " + line;
                label_alone = false;
            }

            file_content->push_back(line);
        }

    }

    input_file.close();

    if(extension == ".pre") {
        FindEQU(file_content, equ_table);
        ProcessEquIf(file_content, equ_table, in+extension);
        return;
    }

    if(extension == ".mcr") {
        FindMacro(file_content, macro_table);
        ProcessMacro(file_content, macro_table, in+extension);
    }

}

std::string WriteInstruction(int j, std::string* token) {
    std::string inst;

    switch (j) {
        case 1:
            inst = token[0] + NEWLINE;
            break;
        case 2:
            inst = token[0] + WHITESPACE + token[1] + NEWLINE;
            break;
        case 3:
            inst = token[0] + WHITESPACE + token[1] + WHITESPACE + token[2] + NEWLINE;
            break;
        default:    // para instr de macro
            for (int i = 0; i < j; i++) {
                if(i == j - 1) {
                    inst = inst + token[i] + NEWLINE;
                    continue;
                }
                inst = inst + token[i] + WHITESPACE;
            }
            
            break;
    }

    return inst;

}

void ProcessMacro(std::vector<std::string>* file_content, std::map <std::string, std::vector<std::string>>* macro_table, std::string file_name) {
    std::fstream output_file;
    std::vector<std::string> body;
    std::map <std::string, std::string> params_table;
    std::string token[6];
    std::string instruction;
    char* line_token;
    char* macro_token;
    char str[100];
    bool is_header;
    bool call_macro = false;

    // criacao do arquivo de saida
    output_file.open(file_name, std::ios::out);
    if(!output_file) {
        std::cout << "Nao foi possivel criar o arquivo!" << std::endl;
        return ;
    }

    for(long unsigned int i = 0; i < file_content->size(); i++) {
        strcpy(str, (*file_content)[i].c_str());
        line_token = strtok(str, " ");
        
        // caso em que uma macro chama outra macro
        // se entrar na cond quer dizer que a primeira macro ja foi expandida
        if(call_macro) {
            strcpy(str, instruction.c_str());
            line_token = strtok(str, " ");
            call_macro = false;
        }

        int count_token = 0;
        while(line_token) {
            token[count_token] = line_token;
            count_token++;
            if((*macro_table).find(token[0]) != (*macro_table).end()) {
                line_token = strtok(NULL, ", ");    // desconsidera "," que separam os params da macro
                continue;
            }
            line_token = strtok(NULL, " ");
        }

        // faz a expansao da macro
        if((*macro_table).find(token[0]) != (*macro_table).end()) {
            body = (*macro_table).find(token[0])->second;
            is_header = true;

            for(std::string line : body) {
                strcpy(str, line.c_str());
                macro_token = strtok(str, ", ");
                
                int k = 0;
                if(is_header) { // tratamento especial caso seja o cabecalho (contem params que a macro utiliza)
                    while(macro_token) {
                        params_table[macro_token] = token[k + 1]; // associa o param ao arg passado na chamada
                        macro_token = strtok(NULL, ", ");
                        k++;
                    }

                    is_header = false;
                    continue;
                }

                while(macro_token) {
                    if(params_table.find(macro_token) != params_table.end()) {
                        token[k] = params_table.find(macro_token)->second;  // substitui o param pelo arg no conteudo da macro
                    } else {
                        token[k] = macro_token;
                    }
                    macro_token = strtok(NULL, ", ");
                    k++;
                }

                // trata o caso da macro ser chamada dentro de outra macro
                if(macro_table->find(token[0]) != macro_table->end()) {
                    instruction = WriteInstruction(k, token);
                    instruction = instruction.substr(0, instruction.length() - 1);  // retira "\n"
                    call_macro = true;
                    i--; // volta no tempo
                    break;
                }

                if(k == 3) {    // caso do "copy end1, end2"
                    output_file << token[0] + WHITESPACE + token[1] + "," + token[2] + NEWLINE;
                    continue;
                }

                instruction = WriteInstruction(k, token);
                output_file << instruction;

            }
            continue;

        }
    
        instruction = WriteInstruction(count_token, token);
        output_file << instruction;

    }

    output_file.close();
    std::cout << "O arquivo foi criado com sucesso!" << std::endl;

}

void FindMacro(std::vector<std::string>* file_content, std::map <std::string, std::vector<std::string>>* macro_table) {
    std::vector<std::string> body;
    std::string label;
    std::string params_macro;
    std::string token[10];
    std::string temp_str;
    char* line_token;
    char str[100];
    bool is_macro = false;
    int pos;

    for(long unsigned int i = 0; i < file_content->size(); i++) {
        strcpy(str, (*file_content)[i].c_str());
        line_token = strtok(str, " ");
        
        int count_token = 0;
        while(line_token) {
            token[count_token] = line_token;
            count_token++;
            line_token = strtok(NULL, " ");
        }

        temp_str = token[1];
        std::transform(temp_str.begin(), temp_str.end(), temp_str.begin(), ::toupper);
        if(temp_str == "MACRO") {
            is_macro = true;
            label = token[0].substr(0, token[0].length() - 1);  // retira ":" do rotulo
            pos = i;

            if(count_token > 2) // quer dizer que a macro possui params
                params_macro = (*file_content)[i].substr((*file_content)[i].find(token[2]), (*file_content)[i].length()); // Salva so os params
            else
                params_macro = "-"; // usado para garantir a existencia do cabecalho para macro sem param

            body.push_back(params_macro);  // inicia a construcao do corpo da macro com o cabecalho
            continue;
        }

        temp_str = token[0];
        std::transform(temp_str.begin(), temp_str.end(), temp_str.begin(), ::toupper);
        if(temp_str == "ENDMACRO"){
            (*macro_table)[label] = body;   // associa o rotulo com o corpo da macro
            (*file_content).erase((*file_content).begin() + pos, (*file_content).begin() + i + 1); // exclui o trecho da macro do cod original
            i = pos - 1;    // retorna 1 posicao antes de iniciar a macro
            body.clear();
            is_macro = false;
        }

        if(is_macro)
            body.push_back((*file_content)[i]);

    }

}

void ProcessEquIf(std::vector<std::string>* file_content, std::map <std::string, std::string>* equ_table, std::string file_name) {
    std::fstream output_file;
    std::string token[10];
    std::string instruction;
    char* line_token;
    char str[100];
    std::string temp_str;

    // criacao do arquivo de saida
    output_file.open(file_name, std::ios::out);
    if(!output_file) {
        std::cout << "Nao foi possivel criar o arquivo!" << std::endl;
        return ;
    }

    for(long unsigned int i = 0; i < file_content->size(); i++) {
        strcpy(str, (*file_content)[i].c_str());
        line_token = strtok(str, " ");

        // criacao dos tokens da linha
        int count_token = 0;
        while(line_token) {
            token[count_token] = line_token;
            count_token++; // armazena qtd de tokens criados
            line_token = strtok(NULL, " "); // aponta para proximo token da linha

        }

        // para qnd tiver espaco separando rotulo e ":", tipo "rot   :"
        if(token[1] == ":") {
            for(int j = 0; j < count_token; j++) {
                if(j == 0)
                    token[j] += token[j + 1];
                else
                    token[j] = token[j + 1];
            }
            count_token--;
        }

        temp_str = token[0];
        std::transform(temp_str.begin(), temp_str.end(), temp_str.begin(), ::toupper);
        if(temp_str == "IF") {
            // Desconsidera prox linha se a flag for 0
            if(equ_table->find(token[1])->second == "0")
                i++;
            continue;
        }

        std::string s;
        // garante que instr do tipo copy tenha 2 tokens => |copy| |end1,end2|
        if(temp_str == "COPY") {
            for (int j = 1; j < count_token; j++) {
                s += token[j];
            }
            token[1] = s;
            count_token = 2;
        }

        temp_str = token[1];
        std::transform(temp_str.begin(), temp_str.end(), temp_str.begin(), ::toupper);
        // trata instr com space e const que possua como arg EQU       
        if (temp_str == "SPACE" || temp_str == "CONST")
            if((*equ_table).find(token[2]) != (*equ_table).end())
                token[2] = (*equ_table).find(token[2])->second;

        // para instr de copy que tem rotulo, garante 3 tokens => |rotulo:| |copy| |end1,end2|
        if(temp_str == "COPY") {
            for (int j = 2; j < count_token; j++) {
                s += token[j];
            }
            token[2] = s;
            count_token = 3;
        }

        instruction = WriteInstruction(count_token, token);
        output_file << instruction;


    }

    output_file.close();
    std::cout << "O arquivo foi criado com sucesso!" << std::endl;

}

void FindEQU(std::vector<std::string>* file_content, std::map <std::string, std::string>* equ_table) {
    std::string label;
    std::string value;
    std::string token[4];
    std::string temp_str;
    char* line_token;
    char str[100];

    for(long unsigned int i = 0; i < file_content->size(); i++) {

        strcpy(str, (*file_content)[i].c_str());
        line_token = strtok(str, " ");

        // criacao dos tokens da linha
        int j = 0;
        while(line_token) {
            token[j] = line_token;
            j++; // qtd de tokens criados
            line_token = strtok(NULL, " "); // aponta para proximo token da linha
        }

        temp_str = token[0];
        std::transform(temp_str.begin(), temp_str.end(), temp_str.begin(), ::toupper);
        if(temp_str == "SECTION") {
            // Remove as instrucoes de definicao de EQU do vetor
            (*file_content).erase((*file_content).begin(), (*file_content).begin() + i);
            return ;
        }

        temp_str = token[1];
        std::transform(temp_str.begin(), temp_str.end(), temp_str.begin(), ::toupper);
        if(temp_str == "EQU") {
            token[0] = token[0].substr(0, token[0].find(":"));  // desconsidera ":" no final do rotulo
            label = token[0];
            value = token[2];
            (*equ_table)[label] = value;
        }

    }

}
