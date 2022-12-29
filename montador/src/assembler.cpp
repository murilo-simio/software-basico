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

    vector<string> td; // Tabela de definicao
    map<string, int> ts; // Tabela de simbolos
    map<string, vector<int>> tu; // Tabela de uso

    while(getline(input_file, line)){
        istringstream str(line);
        string token;

        // Limpa o vetor de tokens e coloca o indice na primeira linha
        tokens.clear();
        line_index++;

        // Copia os tokens para o vetor de tokens
        while(str >> token) {
            tokens.push_back(upper(token));     // Transforma tudo em maiuscula
        }

        switch(tokens.size()){
            case 1:     // Caso só tenha um token
            {
                // Se esse token nao for stop nem end chama um erro
                if(tokens.at(0) != "STOP"){
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

                    if(tokens.at(1) == "STOP" || tokens.at(1) == "SPACE") {
                        addr++;
                    } else {
                        cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                        cout << "Invalid instruction" << endl;
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
                if(tokens.at(0).back() == DOIS_PTS) {
                    map<string, int>::iterator temp_token;
                    
                    tokens.at(0).pop_back();
                    string label = tokens.at(0);

                    temp_token = ts.find(label);
                    if(temp_token == ts.end()) {
                        if(!lexic_err(label, line_index))
                            error=true;
                        ts.insert(pair<string, int>(label, addr));
                    } else {
                        cout << file_in << ":" << line_index << ": error: Semantic Error: ";
                        cout << "Label Redefined" << endl;
                        error = true;  
                    }

                    if(tokens.at(1) == "CONST") {
                        addr++;
                    } else if(tamanho_instr(tokens.at(1).c_str()) == 2) {
                        for(auto& c : tu){
                            if(c.first == tokens.at(2)){
                                c.second.push_back(addr+1);
                            }
                        }
                        addr+=2;
                    } else {
                        if(tokens.at(1) == "STOP" || tokens.at(1) == "COPY"){
                            cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                            cout << "Invalid arguments" << endl;
                            error = true;
                        } else {
                            cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                            cout << "Invalid instruction" << endl;
                            error = true;
                        }
                    }
                } else if(tokens.at(0) == "COPY") {
                    for(auto& c:tu) {
                        if(c.first == tokens.at(1))
                            c.second.push_back(addr+1);
                        if(c.first == tokens.at(2))
                            c.second.push_back(addr+2);
                    }
                    addr+=3;
                } else {
                    cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                    cout << "Invalid arguments" << endl;
                    error = true;
                }
                break;
            }
            case 4:
            {
                if(tokens.at(0).back() == DOIS_PTS) {
                    map<string, int>::iterator temp_token;
                    
                    tokens.at(0).pop_back();
                    string label = tokens.at(0);

                    temp_token = ts.find(label);
                    if(temp_token == ts.end()) {
                        if(!lexic_err(label, line_index))
                            error=true;
                        ts.insert(pair<string, int>(label, addr));
                    } else {
                        cout << file_in << ":" << line_index << ": error: Semantic Error: ";
                        cout << "Label Redefined" << endl;
                        error = true;  
                    }

                    if(tokens.at(1) == "COPY"){
                        for(auto& c:tu) {
                            if(c.first == tokens.at(2))
                                c.second.push_back(addr+1);
                            if(c.first == tokens.at(3))
                                c.second.push_back(addr+2);
                        }
                        addr+=3;
                    } else {
                        cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                        cout << "Invalid instruction" << endl;
                        error = true;
                    }
                } else {
                    cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                    cout << "Invalid instruction" << endl;
                    error = true;
                }
                break;
            }
            default:
            {
                cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                cout << "Invalid arguments" << endl;
                error = true;
                break;
            }
        }
    }

    if(!text){
        cout << file_in << ": error: Semantic Error: "; 
        cout << "No TEXT section" << endl;
        error = true;
    }
    input_file.close();

    ofstream output_file(file_out, ofstream::out);
    if(!output_file.good()){
        cout << "Problema ao ler arquivo de saida!" << endl;
        return;
    }

    ifstream input_file1(file_in, ifstream::in);

    if(!input_file1.good()) {
        cout << "Problema ao ler arquivo de entrada!" << endl;
        return;
    }

    map<string,int>::iterator token_ts;
    map<string, vector<int>>::iterator token_tu;

    while(getline(input_file1, line)) {
        istringstream str(line);
        string token;

        tokens.clear();
        line_index++;

        while(str >> token) {
            tokens.push_back(token);
        }

        switch(tokens.size()){
            case 1:
            {
                if(tokens.at(0) == "STOP"){
                    output_file << opcode(tokens.at(0)) << " ";
                }
                break;
            }
            case 2:
            {
                if(tokens.at(0).back() == DOIS_PTS) {
                    if(tokens.at(0) == "STOP"){
                        output_file << opcode("STOP") << " ";
                    }else if(tokens.at(1) == "SPACE"){
                        output_file << "0";
                    }
                } else if (tamanho_instr(tokens.at(0).c_str()) == 2){
                    output_file << opcode(tokens.at(0)) << " ";
                    token_ts = ts.find(tokens.at(1));
                    if(lexic_err(tokens.at(1), line_index)){
                        if(token_ts == ts.end()){
                            token_tu = tu.find(tokens.at(1));
                            if(token_tu == tu.end()){
                                cout << file_in << ":" << line_index << ": error: Semantic Error: ";
                                cout << "Label Undefined" << endl;
                                error = true; 
                                output_file << tokens.at(1) << " ";
                            } else {
                                output_file << "0 ";
                            }
                        } else {
                            output_file << ts.find(tokens.at(1))->second << " ";
                        }
                    } else {
                        error=true;
                    }
                }
                break;
            }
            case 3:
            {
                
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

string upper(string s){
    for(unsigned i=0;i<s.length();i++){
        s[i] = toupper(s[i]);
    }
    return s;
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
            cout << "label contains invalid characters" << endl;
        return false;
        }
    }

    return true;
}

int opcode(const string& str){
    return operacoes[str];
}
