#include "assembler.h"

using namespace std;

void assemble(char* file_in, char* file_out) {

    ifstream input_file(file_in, ifstream::in);

    if(!input_file.good()) {
        cout << "Problema ao ler arquivo de entrada!" << endl;
        return;
    }

    string line;

    while(!input_file.eof()){
        getline(input_file, line);
    }

}

Linha Linha::getToken(std::string s, int line){
    
    int index = 0, dois_p = 0;
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

    if(dois_p > 1){

    }
}