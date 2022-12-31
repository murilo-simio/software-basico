#include "assembler.h"

using namespace std;

char* file_name_globlal;

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

    bool data = false;
    bool error = false;
    bool text = false;

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
                if(tokens.at(0) == "STOP"){     // Se for stop acresecenta 1 no endereço
                    addr+=1;
                } else {    // Se não, não conheco a instr
                    if(tamanho_instr(tokens.at(0).c_str()) == 2 || tokens.at(0) == "COPY"){
                        cout << file_in << ":" << line_index << ": error: Syntax Error: ";
                        cout << "Invalid number of arguments" << endl;
                        error = true;
                    }
                }
                break;
            }
            case 2:     // Caso tenham 2 tokens
            {
                if(tokens.at(0).back() == ':'){    // Verifica se é uma label
                    map<string,int>::iterator token_ts;
                    map<string, vector<int>>::iterator token_tu;
                    
                    
                    tokens.at(0).pop_back();
                    string label = tokens.at(0);

                    if(!lexic_err(label, line_index)){ // Verifica se há algum erro lexico na label
                        error = true;
                    }

                    token_ts = ts.find(label);
                    token_tu = tu.find(label);

                    if (token_ts != ts.end() || token_tu != tu.end()){  // Verifica se a label encontrada ja foi definido
                        cout << file_in << ":" << line_index << ": error: Semantic Error: ";
                        cout << "Label Redefinition" << endl;
                        error = true;
                    }

                    if(tokens.at(1) != "EXTERN"){
                        ts.insert(pair<string, int>(label, addr));
                    }

                    if(tokens.at(1) == "STOP" || tokens.at(1) == "SPACE") {     // Verifica se a instr depois da label é de 1 espaco so
                        addr++;
                    } else {
                        if(tamanho_instr(tokens.at(1).c_str()) >= 2){
                            cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                            cout << "Invalid number of arguments" << endl;
                        } else {
                            cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                            cout << "Invalid instruction" << endl;
                        }
                        error = true;
                    }
                } else if(tamanho_instr(tokens.at(0).c_str()) == 2) {   // Caso não tenha label verifica se é uma instr de 2 addrs
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
                            cout << "!!!!!" << endl;
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
                        if(tokens.at(0) == "STOP") {
                            cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                            cout << "Invalid arguments" << endl;
                            error = true;
                        } else if (tokens.at(0) == "COPY"){
                            string::size_type pos = tokens.at(1).find(',');
                            if(pos != string::npos){
                                string antes = processa_primeiro_arg(tokens.at(1), ',');
                                string depois = processa_segundo_arg(tokens.at(1), ',');
                                for(auto& c:tu) {
                                    if(c.first == antes)
                                        c.second.push_back(addr+1);
                                    if(c.first == depois)
                                        c.second.push_back(addr+2);
                                }
                                addr+=3;
                                break;
                            } else {
                                cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                                cout << "Invalid arguments" << endl;
                                error = true;
                            }
                        } else if(tokens.at(0) == "SPACE" || tokens.at(0) == "LOAD") {
                            string::size_type pos = tokens.at(1).find('+');
                            if(pos != string::npos) { // caso space tenha argumento tipo X+2
                                string antes = processa_primeiro_arg(tokens.at(1), '+');
                                string depois = processa_segundo_arg(tokens.at(1), '+');
                                int end = 1 + stoi(depois);
                                addr+=end;
                            }else{
                                addr+=1;
                            }
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
                if(tokens.at(0).back() == ':') {
                    
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
                    } else if(tokens.at(1) == "COPY"){
                        string::size_type pos = tokens.at(2).find(',');
                        if(pos != string::npos) {
                            string antes = processa_primeiro_arg(tokens.at(2), ',');
                            string depois = processa_segundo_arg(tokens.at(2), ',');
                            for(auto& c:tu) {
                                if(c.first == antes)
                                    c.second.push_back(addr+1);
                                if(c.first == depois)
                                    c.second.push_back(addr+2);
                            }
                            addr+=3;
                            break;
                        } else {
                            cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                            cout << "Invalid arguments" << endl;
                            error = true;
                        }
                    } else if(tokens.at(1) == "SPACE" || tokens.at(1) == "LOAD") {
                        string::size_type pos = tokens.at(2).find('+');
                        if(pos != string::npos) { // caso space tenha argumento tipo X+2
                            string antes = processa_primeiro_arg(tokens.at(2), '+');
                            string depois = processa_segundo_arg(tokens.at(2), '+');
                            int end = 1 + stoi(depois);
                            addr+=end;
                        } else {
                            if(is_number(tokens.at(2))) {
                                int tmp = stoi(tokens.at(2)); 
                                for(int i = 1; i < tmp; i++) {
                                    label = tokens.at(0) + "+" + to_string(i);
                                    ts.insert(pair<string, int>(label, addr + i));
                                }
                                addr += tmp;
                            } else {
                                cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                                cout << "Invalid arguments" << endl;    // nao sei se o erro eh esse msm
                                error = true;
                            }
                        }
                    } else {
                        if(tokens.at(1) == "STOP"){
                            cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                            cout << "Invalid arguments" << endl;
                            error = true;
                        } else {
                            cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                            cout << "Invalid instruction" << endl;
                            error = true;
                        }
                    }
                } else {
                    cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                    cout << "Invalid arguments" << endl;
                    error = true;
                }
                break;
            }
            case 4:
            {
                if(tokens.at(0).back() == ':') {
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

    line_index = 0; addr = 0;

    while(getline(input_file1, line)) {
        istringstream str(line);
        string token;
        bool is_copy_instr = false;

        tokens.clear();
        line_index++;

        while(str >> token) {
            if(token == "COPY") {
                is_copy_instr = true;
                tokens.push_back(token);
                continue;         
            }
            if(is_copy_instr) {
                string antes = processa_primeiro_arg(token, ',');
                string depois = processa_segundo_arg(token, ',');
                tokens.push_back(antes);
                tokens.push_back(depois);
                continue;
            }
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
                if(tokens.at(0).back() == ':') {
                    if(tokens.at(0) == "STOP"){
                        output_file << opcode("STOP") << " ";
                    }else if(tokens.at(1) == "SPACE"){
                        output_file << "0" << " ";
                    }
                } else if (tamanho_instr(tokens.at(0).c_str()) == 2){
                    output_file << opcode(tokens.at(0)) << " ";
                    token_ts = ts.find(tokens.at(1));
                    string::size_type pos = tokens.at(1).find('+');
                    if(pos != string::npos) {
                        if(token_ts == ts.end()){
                            token_tu = tu.find(tokens.at(1));
                            if(token_tu == tu.end()){
                                cout << file_in << ":" << line_index << ": error: Semantic Error: ";
                                cout << "Label Undefined" << endl;
                                error = true; 
                                output_file << tokens.at(1) << " ";
                            }
                        } else {
                            output_file << ts.find(tokens.at(1))->second << " ";
                        }
                        break;
                    }
                    
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
                if(tokens.at(0).back() == ':') {
                    if(tamanho_instr(tokens.at(1).c_str()) == 2) {
                        output_file << opcode(tokens.at(1)) << " ";
                        token_ts = ts.find(tokens.at(2));

                        if(lexic_err(tokens.at(2), line_index)){
                            if(token_ts == ts.end()){
                                token_tu = tu.find(tokens.at(2));
                                if(token_tu == tu.end()){
                                    cout << file_in << ":" << line_index << ": error: Semantic Error: ";
                                    cout << "Label Undefined" << endl;
                                    error = true; 
                                    output_file << tokens.at(1) << " ";
                                } else {
                                    output_file << "0 ";
                                }
                            } else {
                                output_file << ts.find(tokens.at(2))->second << " ";
                            }
                        } else {
                            error = true;
                        }
                    }

                    if(tokens.at(1) == "CONST"){
                        output_file << htod(tokens.at(2)) << " ";
                    }
                    if(tokens.at(1) == "SPACE") {
                        string::size_type pos = tokens.at(2).find('+');
                        if(is_number(tokens.at(2))) {   // caso space tenha como arg so numero
                            for(int i = 0; i < stoi(tokens.at(2)); i++)
                                output_file << "0 ";
                        } else if(pos != string::npos) { // caso space tenha argumento tipo X+2
                            string antes = processa_primeiro_arg(tokens.at(2), '+');
                            string depois = processa_segundo_arg(tokens.at(2), '+');
                            for(int i=0;i<stoi(depois);i++){
                                output_file << "0 ";
                            }
                        } else {
                            cout << file_in << ":" << line_index << ": error: Syntax Error: "; 
                            cout << "Invalid arguments" << endl;    // nao sei se o erro eh esse msm
                            error = true;
                        }
                    }
                } else if(tokens.at(0) == "COPY") {

                    output_file << opcode(tokens.at(0)) << " ";
                    // tokens.at(1).pop_back();
                    token_ts = ts.find(tokens.at(1));
                    if(lexic_err(tokens.at(1), line_index)) {
                        if(token_ts == ts.end()) {
                            cout << file_in << ":" << line_index << ": error: Semantic Error: ";
                            cout << "Label Undefined" << endl;
                            error = true; 
                            output_file << tokens.at(1) << " ";
                        } else {
                            output_file << ts.find(tokens.at(1))->second << " ";
                        }
                    } else {
                        output_file << tokens.at(1) << " ";
                        error = true;
                    }
                    // tokens.at(2).pop_back();
                    token_ts = ts.find(tokens.at(2));
                    if(lexic_err(tokens.at(2), line_index)){
                        if(token_ts == ts.end()) {
                            cout << file_in << ":" << line_index << ": error: Semantic Error: ";
                            cout << "Label Undefined" << endl;
                            error = true; 
                            output_file << tokens.at(2) << " ";
                        } else {
                            output_file << ts.find(tokens.at(2))->second << " ";
                        }
                    } else {
                        output_file << tokens.at(2) << " ";
                        error = true;
                    }
                }
                break;
            }
            case 4:
            {
                output_file << opcode(tokens.at(1)) << " ";
                // tokens.at(2).pop_back();
                token_ts = ts.find(tokens.at(2));

                if(lexic_err(tokens.at(2), line_index)){
                    if(token_ts == ts.end()){
                        cout << file_in << ":" << line_index << ": error: Semantic Error: ";
                        cout << "Label Undefined" << endl;
                        error = true; 
                        output_file << tokens.at(2) << " ";
                    } else {
                        output_file << ts.find(tokens.at(2))->second << " ";
                    }
                } else {
                    output_file << tokens.at(2) << " ";
                    error = true;
                }

                token_ts = ts.find(tokens.at(3));
                if(lexic_err(tokens.at(3), line_index)){
                    if(token_ts == ts.end()){
                        cout << file_in << ":" << line_index << ": error: Semantic Error: ";
                        cout << "Label Undefined" << endl;
                        error = true; 
                        output_file << tokens.at(3) << " ";
                    } else {
                        output_file << ts.find(tokens.at(3))->second << " ";
                    }
                } else {
                    output_file << tokens.at(3) << " ";
                    error = true;
                }
                break;
            }
            default:
            {
                cout << file_in << ":" << "Something went wrong!";
                error = true;
                break;
            }
        }
    }

    input_file1.close();
    output_file.close();

    if(error){
        cout << file_in << ": Something went wrong!" << endl;
    } else {
        cout << file_in << ": Arquivo gravado em: " << file_out << endl;
    }

    return;
}

int tamanho_instr(char const* token) {
    string str = upper(token);
    if (str == "COPY") {
        return 3;
    } else if (str == "STOP") {
        return 1;
    } else if (operacoes.find(str)->second){
        return 2;
    } else {
        return -1;
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

// Hexadecimal string number to Decimal string number
string htod(const string& s){
    string tmp = s;
    if(s.at(0) == '0' && toupper(s.at(1)) == 'X'){
        int decimal = stoi(s, nullptr, 16);
        tmp = to_string(decimal);
    }
    return tmp;
}

string processa_primeiro_arg(string const& s, char t){
    string::size_type pos = s.find(t);
    if(pos != string::npos){
        return s.substr(0,pos);
    }else{
        return s;
    }
}

string processa_segundo_arg(string const& s, char t){
    string::size_type pos = s.find(t);
    if(pos != string::npos){
        return s.substr(pos+1);
    }else{
        return s;
    }
}

bool is_number(string const& s) {
    for(char const &car : s) {
        if(std::isdigit(car) == 0)
            return false;
    }
    return true;
}