#include "assembler.h"

using namespace std;

char* file_name_globlal;

void assemble(char* file_in, char* file_out) {

    ifstream input_file(file_in, ifstream::in);

    if(!input_file.good()) {
        cout << "Problema ao ler arquivo de entrada!" << endl;
        return;
    }

    file_name_globlal = file_in;

    int line_index = 0, addr = 0;
    string line;
    vector<string> tokens;
    bool error = false;
    bool text = false;
    bool data = false;

    map<string, int> ts; // Tabela de simbolos
    vector<string> td; // Tabela de definicao
    map<string, vector<int>> tu; // Tabela de uso

    while(!input_file.eof()){
        getline(input_file, line);

        istringstream str(line);
        string token;

        // Limpa o vetor de tokens e coloca o indice na primeira linha
        tokens.clear();
        line_index++;

        // Copia os tokens para o vetor de tokens
        while(str >> token) {
            tokens.push_back(token);
        }

        switch(tokens.size()){
            case 1:     // Caso só tenha um token
            {
                // Se esse token nao for stop nem end chama um erro
                if(tokens.at(0) != "STOP" && tokens.at(0) != "END"){
                    if(tamanho_instr(tokens.at(0).c_str()) == 2 || tokens.at(0) == "COPY"){
                        cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                        cout << "Invalid number of arguments" << endl;
                        error = true;
                        break;
                    }
                }

                if(tokens.at(0) == "STOP"){     // Se for stop acresecenta 1 no endereço
                    addr++;
                    break;
                } else {    // Se não, é algum argumento desconhecido
                    cout << file_in << ":" << line_index << ": error: Syntax Error: ";
                    cout << "Unknown argument" << endl;
                    error = true;
                    break;
                }
            }
            case 2:     // Caso tenham 2 tokens
            {
                map<string,int>::iterator token_ts;
                map<string, vector<int>>::iterator token_tu;

                if(tokens.at(0).back() == DOIS_PTS){
                    tokens.at(0).pop_back();
                    string label = tokens.at(0);

                    if(!lexic_err(label, line_index)) // Verifica se há algum erro lexico
                        error = true;

                    token_ts = ts.find(label);  // Verifica se a label encontrada ja foi definido
                    token_tu = tu.find(label);

                    if (token_ts != ts.end() || token_tu != tu.end()){
                        cout << file_in << ":" << line_index << ": error: Semantic Error: ";
                        cout << "Label Redefinition" << endl;
                        error = true;
                    }

                    if (tokens.at(1) == "EXTERN") { // Verifica se é uma var externa
                        vector<int> e_addr;
                        tu.insert(pair<string, vector<int>>(label, e_addr));
                    } else {
                        ts.insert(pair<string, int>(label, addr));
                    }

                    if(tokens.at(1) == "STOP" || tokens.at(1) == "SPACE") {
                        addr++;
                    } else if(tokens.at(0) == "BEGIN") {
                        // Fazer alg coisa
                    } else if(tokens.at(1) != "EXTERN") {
                        if((tamanho_instr(tokens.at(1).c_str()) == 2) || tokens.at(0) == "COPY") {
                            cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                            cout << "Invalid arguments" << endl;
                            error = true;
                        }
                    } else {
                        cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                        cout << "Invalid instruction" << endl;
                        error = true;
                    }
                } else if(tokens.at(0) == "PUBLIC") {
                    if(find(td.begin(), td.end(), tokens.at(1)) == td.end()) {
                        td.push_back(tokens.at(1));
                    } else {
                        cout << file_in << ":" << line_index << ": error: Semantic Error: ";
                        cout << "Public Redefinition" << endl;
                        error = true;                        
                    }
                } else if(tamanho_instr(tokens.at(0).c_str()) == 2) {
                    for(auto& c : tu){
                        if(c.first == tokens.at(1)){
                            c.second.push_back(addr+1);
                        }
                    }
                    addr+=2;
                } else {
                    if(line == "SECTION TEXT"){
                        if(text){
                            cout << file_in << ":" << line_index << ": error: Semantic Error: ";
                            cout << "Text Redefined" << endl;
                            error = true;  
                        }
                        text = true;
                    } else if(line == "SECTION DATA"){
                        if(data){
                            cout << file_in << ":" << line_index << ": error: Semantic Error: ";
                            cout << "Data Redefined" << endl;
                            error = true;  
                        }
                        if(!text){
                            cout << file_in << ":" << line_index << ": error: Semantic Error: ";
                            cout << "Text should be before data" << endl;
                            error = true; 
                        }
                        data = true;
                    } else {
                        if(tokens.at(0) == "STOP" || tokens.at(0) == "COPY") {
                            cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                            cout << "Invalid arguments" << endl;
                            error = true;
                        } else {
                            cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                            cout << "Invalid instruction" << endl;
                            error = true;
                        }
                        error = true;
                    }
                }
                break;
            }   // Case 2
            case 3:
            {
                return;
            }
        }
    }

}

int tamanho_instr(const char* token) {
    string str(string_maiusc(token));
    if (str == "COPY") {
        return 3;
    } else if (str == "STOP") {
        return 1;
    } else {
        return 2;
    }
}

char* string_maiusc(const char* s) {
  string str(s);
  for (char& c : str) {
    c = toupper(c);
  }
}

bool lexic_err(const string& label, int line) {
    if(label.at(0) >= '0' && label.at(0) <= '9'){
        cout << file_name_globlal << ":" << line << ": error: Lexical Error: "; 
        cout << "labels cant start with non-alphabetical characters" << endl;
        return false;
    }

    for(auto c : label){
        if(!((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == '_'))) {
            cout << file_name_globlal << ":" << line << ": error: Lexical Error: "; 
            cout << "label contains with invalid characters" << endl;
        return false;
        }
    }

    return true;
}

/*
Linha Linha::getToken(std::string s, int line){
    
    int index = 0, dois_p = 0, erros = 0;
    string temp, buff = "";
    string aux[10] = {"","","",""};

    if(string::npos == s.find(DOIS_PTS))
        index = 1;  // Não há rótulos na linha

    for(char& c : s){
        if(c == DOIS_PTS)
            dois_p++;   // Conta quantos rótulos numa mesma linha
        
        if(c == PONTO_VIR)
            break;  // Verifica o fim da linha

        temp = c;
        if(!(pontos.find(temp) != pontos.end())){
            buff += c;  // Verifca se não é " ", ":", ","
        }else{
            aux[index] = buff;
            buff = "";
            index++;
        }
    }
    aux[index] = buff;

    string labels[2];

    // Trata erro de mais de 1 label por linha
    if(dois_p > 1) {     // Se tiver mais de uma label na mesma linha
        for(int i=1;i<dois_p;i++) {
            labels[i-1] = aux[i];   // Coloca a 2 label num array temp
        }

        for(int i=1;i<index;i++) {      // Corrige as posicoes
            aux[i] = aux[i+(dois_p-1)];
        }

        index -= (dois_p + 1);
        erros++;

        cout << file_name_globlal << ":" << line << ": error: Syntax Error: "; 
        cout << dois_p << "labels in the same line!" << endl;
    }

    // Gera a linha com a label, opcode e argumentos
    Linha linha;

    linha.label = aux[0];
    linha.op = aux[1];
    linha.arg1 = aux[2];
    linha.arg2 = aux[3];


    return linha;
}*/