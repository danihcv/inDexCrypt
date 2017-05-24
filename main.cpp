#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <vector>
#include <fstream>
#include <regex>
#include <map>
#include <unistd.h>

#define CONFIG_FunctionHeaderWidth 41
#define FILE_UserSettings "usrSttngs.conf"
#define FILE_EncryptedContent "cntfile.dve"
#define FILE_EncryptedLinks "lnkFile.dve"
#define FILE_DecryptedHTML "file.html"
#define FILE_CMD "cmd.bat"
#define PASSWORD "hackthepass"
#define StrLenMax 10000
#define CODE_FunctionFinished 1
#define CODE_FunctionError -3
#define CODE_LOGOUT -2
#define CODE_EXIT -1
#define CODE_BackToMenu 0
#define MENU_OPTIONS 7
#define MENU_EncryptContent 1
#define MENU_EncryptLink 2
#define MENU_EditContent 3
#define MENU_RemoveLinks 4
#define MENU_OpenLinks 5
#define MENU_DecryptAll 6
#define MENU_DecryptedPage 7
#define REGEX_URL "(?:https?:\\/\\/)(?:[\\w]+\\.)([a-zA-Z\\.]{2,63})([\\/\\w\\.-]*)*\\/?"

using  namespace std;

///Variaveis globais:
string menuOptions[MENU_OPTIONS], menuOptionsDescr[MENU_OPTIONS][2];
map<int, int> mapIntNull;

///Funcoes secundarias:
//Funcoes utilitarias:
bool fileExists(string file);                                               /*Retorna TRUE caso o arquivo (file) exista e retorna FALSE caso contrario*/
bool createFile(string name, string content);                               /*Retorna TRUE se o arquivo (file) for criado com sucesso e retorna FALSE caso contrario*/
void swapStrings(string *a, string *b);                                     /*Troca o conteudo de uma string pelo conteudo da outra*/
bool quicksort(vector<string>& lines, int idxi, int idxf, bool ignoreNote); /*Retorna TRUE se conseguir ordenar um vetor de strings e retorna FALSE caso contrario*/
void randomizeVector(vector<string>& links);                                /*Randomiza a ordem dos itens do vetor passado*/
void constructMenuOptionsArray();                                           /*Inicializa o array menuOptions com as opções do menu principal*/
string intToString(int n);                                                  /*Converte um inteiro em string e retorna essa string*/
int vectorContainsString(vector<string> vec, string str, int i, int f);     /*Faz uma busca binaria no vector passado em busca do item str passado. Retorna o indice do item, caso ele esteja presente no vector e retorna -1 caso contrario*/
//Funcoes auxiliares:
void printHeader();                                                         /*Imprime o cabecalho com a logo do programa*/
void printFunctionHeader(int func);                                         /*Imprime o cabecalho de uma funcao: imprime o titulo e a descricao de uso da funcao*/
void clearScreen();                                                         /*Limpa a tela e chama a funcao printHeader()*/
void printBlankSpaces(int n, int mode);                                     /*Imprime a quantidade N de espaços em branco (no MODE especificado) e setta o consoleTextAttribute para o MODE especificado*/
int previousLineLength(int idx, string content);                            /*Retorna a quantidade de caracteres presentes na linha acima a atual, para isso ela necessita saber o indice (idx) atual do cursor e o conteudo (content)*/
int getTotalLinesInFile(string file);                                       /*Retorna a quantidade de linhas presentes no arquivo (file), caso ele exista. Se não existir a funcao retorna CODE_FunctionError*/
int addContentInEncryptedFile(string content);                              /*Imprime o "content" (encriptado) dentro do arquivo encriptado. Retorna CODE_FunctionFinished se a funcao for executada corretamente e retorna CODE_FunctionError caso contrario*/
int addLinkInEncryptedFile(string content, int nota);                       /*Imprime o novo link ("content") dentro do arquivo encriptado. Retorna CODE_FunctionFinished se a funcao for executada corretamente e retorna CODE_FunctionError caso contrario*/
int overwriteEncryptedFile(vector<string> lines, string type);              /*Substitui todo dado, do tipo de dado informado, pelos dados passados, dentro do arquivo encriptado*/
int sortEncryptedFile();                                                    /*Ordena/organiza o arquivo encriptado: primeiro decriptografa, ordena/organiza, criptografa e imprime no arquivo criptografado, sobrescrevendo o conteudo anterior*/
void handleBackupAndTempFiles(bool showReport);                             /*Deleta os arquivos temporarios e lida com o backup. Pode fazer um relatorio de erros se o usuario desejar.*/
bool createHTMLPage();                                                      /*Cria o arquivo HTML para ser aberto no navegador do usuario. Retorna TRUE se o arquivo for feito com sucesso e retorna FALSE caso contrario.*/
int selectLinks(vector<string> links, map<int, int> &selected, int func);   /*Funcao que serve para selecionar links. Ela preenche o map passado, relacionado aos links tambem passados, para que a funcao que a chamou saiba quais foram os links selecionados pelo usuario*/
//Funcoes de seguranca e criptografia:
bool passCheck(string str);                                                 /*Checa se a senha inserida (str) confere com a senha definida (PASSWORD). Retorna TRUE caso sejam iguais e retorna FALSE caso contrario*/
string encrypt(string str, int fact);                                       /*Recebe a string (str) que sera criptografada e o fator (fact) de criptografia. Retorna a string criptografada*/
string decrypt(string srt, int fact);                                       /*Recebe a string (str) que sera decriptada e o fator (fact) de decriptografia. Retorna a string decriptada*/
///Telas:
bool quit();                                                                /*Tela que pergunta se o usuario deseja sair (fechar) do programa. Retorna TRUE caso o usuario queira sair e retorna FALSE caso contrario*/
bool backToMenu();                                                          /*Tela que pergunta se o usuario deseja voltar ao menu principal. Retorna TRUE caso o usuario queira voltar e retornar FALSE caso contrario*/
bool login(int aviso);                                                      /*Tela que pede a senha de login do usuario. Ela recebe uma flag para saber se deve emitir um aviso de "atencao" ou nao. Ela retorna TRUE caso as credenciais inserida sejam validas e retorna FALSE caso contrario*/
void informFunctionError(int func);                                         /*Tela que mostra um aviso informando que a funcao correspondente a func (parametro) finalizou de forma inesperada*/
int screenEncryptContent(string content, bool overwrite, int func);         /*Tela que serve para o usuario inserir conteudo a ser criptografado. Retorna CODE_FunctionFinished se a funcao for executada corretamente, CODE_BackToMenu caso o usuario queira voltar ao menu principal e CODE_FunctionError caso ocorra algum erro no momento de imprimir o conteudo no arquivo criptografado*/
int screenEncryptLink(string content);                                      /*Tela que serve para o usuario inserir um novo link a ser criptografado. Retorna CODE_FunctionFinished se a funcao for executada corretamente, CODE_BackToMenu caso o usuario queira voltar ao menu principal e CODE_FunctionError caso ocorra algum erro no momento de imprimir o conteudo no arquivo criptografado*/
int screenEditContent();                                                    /*Tela que separa todo o dado de tipo Content e passa para a funcao de encriptar novo conteudo*/
int screenRemoveLinks(map<int, int> selected);                              /*Tela que serve para selecionar multiplos links salvos e exclui-los de uma unica vez*/
int screenOpenLinks(map<int, int> selected);                                /*Tela que serve para selecionar multiplos links e abri-los automaticamente, com um intervalo de tempo entre cada link. Este intervalo eh informado pelo usuario. Retorna CODE_FunctionFinished caso o usuario finalize a visualizacao, retorna CODE_BackToMenu caso o usuario deseje voltar ao menu principal e retorna CODE_FunctionError caso o arquivo encriptado nao possa ser aberto*/
int screenDecryptAll();                                                     /*Tela que mostra o conteudo do arquivo criptografado totalmente decriptografado e com esquema de destaque de links. Retorna CODE_FunctionFinished caso o usuario finalize a visualizacao e retorna CODE_BackToMenu caso o usuario deseje voltar ao menu principal*/
int screenDecryptedPage();                                                  /*Funcao que abre o arquivo .html, com todo o conteudo decriptografado, e no navegador e modo escolhido pelo usuario, definidos dentro do arquivo FILE_UserSettings. Retorna CODE_FunctionFinished caso o usuario finalize a visualizacao, retorna CODE_BackToMenu caso o usuario deseje voltar ao menu principal e retorna CODE_FunctionError caso o arquivo HTML nao possa ser criado*/
int mainMenu(int option);                                                   /*Funcao que gerencia o menu principal. Ela retorna a opcao escolhida pelo usuario*/

///#####################################################################################################################
///#############################################  Funcoes Secundarias  #################################################
///#####################################################################################################################
//**********************************************************************************************************************
//**********************************************  Funcoes utilitarias  *************************************************
bool fileExists(string file){
    ifstream f(file.c_str());
    return f.good();
}

bool createFile(string file, string content  = ""){
    FILE *arquivo = fopen(file.c_str(), "w");
    if(arquivo != NULL)
        fprintf(arquivo, "%s", content.c_str());
    else
        return false;
    fclose(arquivo);
    return true;
}

void swapStrings(string *a, string *b){
    string aux = *a; *a = *b; *b = aux;
    return;
}

bool quicksort(vector<string>& lines, int idxi, int idxf, bool ignoreNote = false){
    if(idxf - idxi < 1)
        return true;
    string pivot = lines[(idxi + idxf) / 2];
    int left = idxi, right = idxf;
    if(!ignoreNote) {
        while (left <= right) {
            while (lines[left] > pivot && left < idxf) left++;
            while (lines[right] < pivot && right > 0) right--;

            if (left <= right) {
                swapStrings(&lines[left], &lines[right]);
                ++left;
                --right;
            }
        }
    }
    else{
        pivot = pivot.substr(6, pivot.size()-1);
        while (left <= right) {
            while (lines[left].substr(6, lines[left].size()-1) > pivot && left < idxf) left++;
            while (lines[right].substr(6, lines[right].size()-1) < pivot && right > 0) right--;

            if (left <= right) {
                swapStrings(&lines[left], &lines[right]);
                ++left;
                --right;
            }
        }
    }

    return quicksort(lines, idxi, right, ignoreNote) && quicksort(lines, left, idxf, ignoreNote);
}

void randomizeVector(vector<string>& links){
    vector<string> temp = links;
    links.clear();

    typedef struct{
        int idx;
        int valor;
    }box_t;
    box_t box[temp.size()];

    srand ((unsigned int) time(NULL));
    for(int i = 0; i < temp.size(); i++){
        box[i].idx = i;
        int v = rand() % temp.size();
        box[i].valor = v;
    }
    for(int i = temp.size()-1; i > 0; i--){
        for(int j = 0; j < i; j++){
            if(box[j].valor > box[j+1].valor){
                box_t aux = box[j]; box[j] = box[j+1]; box[j+1] = aux;
            }
        }
    }
    for(int i = 0; i < temp.size(); i++)
        links.push_back(temp[box[i].idx]);

    return;
}

void constructMenuOptionsArray() {
    menuOptions[MENU_EncryptContent - 1] = "Encriptar novo conteudo";
    menuOptions[MENU_EncryptLink - 1] = "Encriptar novo link";
    menuOptions[MENU_EditContent - 1] = "Editar conteudo";
    menuOptions[MENU_RemoveLinks - 1] = "Remover links";
    menuOptions[MENU_OpenLinks - 1] = "Abrir links";
    menuOptions[MENU_DecryptAll - 1] = "Decriptar conteudo";
    menuOptions[MENU_DecryptedPage - 1] = "Abrir arquivo decriptografado";

    menuOptionsDescr[MENU_EncryptContent - 1][0] = "Aperte CTRL + ENTER para finalizar a edicao";
    menuOptionsDescr[MENU_EncryptLink - 1][0] = "Aperte ENTER para finalizar a edicao";
    menuOptionsDescr[MENU_EditContent - 1][0] = "Aperte CTRL + ENTER para finalizar a edicao";
    menuOptionsDescr[MENU_RemoveLinks - 1][0] = "Aperte ENTER para selecionar e";
    menuOptionsDescr[MENU_RemoveLinks - 1][1] = "aperte CTRL + ENTER para excluir";
    menuOptionsDescr[MENU_OpenLinks - 1][0] = "Aperte CTRL + ENTER para concluir a visualizacao";
    menuOptionsDescr[MENU_DecryptAll - 1][0] = "Aperte CTRL + ENTER para concluir a visualizacao";
    menuOptionsDescr[MENU_DecryptedPage - 1][0] = "Aperte CTRL + ENTER para concluir a visualizacao";
    menuOptionsDescr[MENU_DecryptedPage - 1][1] = "e destruir o arquivo";
}

string intToString(int n){
    char aux[100];
    itoa(n, aux, 10);
    string str = aux;
    return str;
}

int vectorContainsString(vector<string> vec, string str, int i = 0, int f = -2){
    if(f == -2) f = vec.size()-1;
    if(i > f)
        return -1;
    int mid = (i+f)/2;
    if(vec[mid].substr(6, vec[mid].size()-1) == str)
        return mid;
    if(str.compare(vec[mid].substr(6, vec[mid].size()-1)) > 0)
        return vectorContainsString(vec, str, i, mid-1);
    return vectorContainsString(vec, str, mid+1, f);
}
//**********************************************************************************************************************
//*******************************************  Funcoes auxiliares  *****************************************************
void printHeader(){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 129);
    cout << "                              _         _____                _____                      _                              " << endl;
    cout << "                             (_)       |  __ \\              / ____|                    | |                             " << endl;
    cout << "                              _  _ __  | |  | |  ___ __  __| |      _ __  _   _  _ __  | |_                            " << endl;
    cout << "                             | || '_ \\ | |  | | / _ \\\\ \\/ /| |     | '__|| | | || '_ \\ | __|                           " << endl;
    cout << "                             | || | | || |__| ||  __/ >  < | |____ | |   | |_| || |_) || |_                            " << endl;
    cout << "                             |_||_| |_||_____/  \\___|/_/\\_\\ \\_____||_|    \\__, || .__/  \\__|                           " << endl;
    cout << "                                                                           __/ || |                                    " << endl;
    cout << "                                                                          |___/ |_|                                    " << endl;
    cout << "                                                                                                                       " << endl;

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << "-----------------------------------------------------------------------------------------------------------------------" << endl << endl;
    return;
}

void printFunctionHeader(int func){
    --func;
    int marginL = (CONFIG_FunctionHeaderWidth/2) - (menuOptions[func].size()/2), marginR = (CONFIG_FunctionHeaderWidth - (CONFIG_FunctionHeaderWidth/2)) - (menuOptions[func].size() - (menuOptions[func].size()/2));

    printBlankSpaces(39, 0);
    printBlankSpaces(marginL, 30);
    cout << menuOptions[func];
    printBlankSpaces(marginR, 30);
    cout << endl;

    printBlankSpaces(59 - (menuOptionsDescr[func][0].size()/2), 0);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
    cout << menuOptionsDescr[func][0] << endl;

    if(menuOptionsDescr[func][1].size() > 0){
        printBlankSpaces(59 - (menuOptionsDescr[func][1].size()/2), 0);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
        cout << menuOptionsDescr[func][1] << endl;
    }

    cout << endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

void clearScreen(){
    system("CLS");
    printHeader();
    return;
}

void printBlankSpaces(int n, int mode){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD) mode);
    for(int i = 0; i < n; i++)
        putchar(' ');
    return;
}

int previousLineLength(int idx, string content){
    for(int i = idx-1; i >= 0; i--){
        if((int)content[i] == 13)
            return idx - i;
    }
    return idx + 1;
}

int getTotalLinesInFile(string file){
    if(fileExists(file)) {
        ifstream arquivo(file.c_str());
        int lines = 0;
        string line;
        while (getline(arquivo, line))
            ++lines;
        return lines;
    }
    else
        return CODE_FunctionError;
}

int addContentInEncryptedFile(string content){
    if(content.length() > 0) {
        vector<string> lines;
        int i = 0;
        string str = "";
        do {
            if ((int)content[i] == 13 || (int)content[i] == (int) '\0') {
                lines.push_back(str);
                str = "";
            } else
                str += content[i];
        } while (content[i++] != '\0');

        FILE *encryptFile = NULL;
        encryptFile = fopen(FILE_EncryptedContent, "a+");

        if (encryptFile == NULL)
            return CODE_FunctionError;

        int n = lines.size(), totalLines = getTotalLinesInFile(FILE_EncryptedContent);
        for (i = 0; i < n; i++)
            fprintf(encryptFile, "%s\n", encrypt(lines[i], i + totalLines).c_str());

        fclose(encryptFile);
    }
    return CODE_FunctionFinished;
}

int addLinkInEncryptedFile(string content, int nota){
    if(content.length() > 0) {
        int i = 0;
        string str = "";

        FILE *encryptFile = NULL;
        encryptFile = fopen(FILE_EncryptedLinks, "a+");

        if (encryptFile == NULL)
            return CODE_FunctionError;

        for(i = 0; i < nota; i++)
            str += "*";
        for(i = nota ; i < 6; i++)
            str += " ";
        str += content;
        int totalLines = getTotalLinesInFile(FILE_EncryptedLinks);
        fprintf(encryptFile, "%s\n", encrypt(str, totalLines).c_str());

        fclose(encryptFile);
    }
    return CODE_FunctionFinished;
}

int overwriteEncryptedFile(vector<string> vec, string type){
    FILE *file = NULL;

    if(type == "link"){
        if(CopyFile(FILE_EncryptedLinks, ("_"+ (string)FILE_EncryptedLinks).c_str(), false) == 0)
            return CODE_FunctionError;

        file = fopen(FILE_EncryptedLinks, "w");
        if(file == NULL)
            return CODE_FunctionError;

        for(int i = 0; i < vec.size(); i++)
            fprintf(file, "%s\n", encrypt(vec[i], i).c_str());

        if(remove(("_"+ (string)FILE_EncryptedLinks).c_str()) != 0) {
            fclose(file);
            return CODE_FunctionError;
        }
    }
    else if(type == "content"){
        if(CopyFile(FILE_EncryptedContent, ("_"+ (string)FILE_EncryptedContent).c_str(), false) == 0)
            return CODE_FunctionError;

        file = fopen(FILE_EncryptedContent, "w");
        if(file == NULL)
            return CODE_FunctionError;

        int i = 0;
        string str = "", temp = vec[0];
        do {
            if ((int)temp[i] == 13 || (int)temp[i] == (int) '\0') {
                if(vec[0].size() > 0)
                    vec.push_back(str);
                str = "";
            } else
                str += temp[i];
        } while (temp[i++] != '\0');
        for(i = 1; i < vec.size(); i++) {
            fprintf(file, "%s\n", encrypt(vec[i], i - 1).c_str());
        }

        if(remove(("_"+ (string)FILE_EncryptedContent).c_str()) != 0) {
            fclose(file);
            return CODE_FunctionError;
        }
    }
    fclose(file);

    return CODE_FunctionFinished;
}

int sortEncryptedFile(){
    vector<string> links;

    ifstream arquivo (FILE_EncryptedLinks);
    string line;
    int lineAt = 0;
    while (getline(arquivo, line)) {
        line = decrypt(line, lineAt++);
        links.push_back(line);
    }
    arquivo.close();

    if(quicksort(links, 0, links.size() - 1)){
        if(CopyFile(FILE_EncryptedLinks, ("_"+ (string)FILE_EncryptedLinks).c_str(), false) == 0)
            return CODE_FunctionError;

        FILE *encryptedFile = NULL;
        encryptedFile = fopen(FILE_EncryptedLinks, "w");

        if(encryptedFile == NULL)
            return CODE_FunctionError;

        for (int i = 0; i < links.size(); i++)
            fprintf(encryptedFile, "%s\n", encrypt(links[i], i).c_str());

        fclose(encryptedFile);
        if(remove(("_"+ (string)FILE_EncryptedLinks).c_str()) != 0)
            return CODE_FunctionError;
    }
    else {
        cout << "Error while sorting '"+ (string)FILE_EncryptedLinks +"'" << endl;
        return CODE_FunctionError;
    }

    return CODE_FunctionFinished;
}

void handleBackupAndTempFiles(bool showReport = false){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    bool failed = false;
    if(!fileExists(FILE_UserSettings)){
        if(!createFile(FILE_UserSettings, "start") && showReport) {
            if (!failed)
                cout << endl << endl << "*Errors Report:" << endl, failed = true;
            perror(("Error while creating '" + (string) FILE_UserSettings + "'").c_str());
        }
    }

    if(!fileExists(FILE_EncryptedContent)){
        if(fileExists("BKP-"+ (string)FILE_EncryptedContent) && !fileExists("_"+ (string)FILE_EncryptedContent)) {
            if (CopyFile(("BKP-" + (string) FILE_EncryptedContent).c_str(), FILE_EncryptedContent, false) == 0 && showReport){
                if (!failed)
                    cout << endl << endl << "*Errors Report:" << endl, failed = true;
                perror(("Error while restoring '" + (string) FILE_EncryptedContent + "' from general backup").c_str());
            }
        }
        else if(fileExists("_"+ (string)FILE_EncryptedContent)){
            if(CopyFile(("_"+ (string)FILE_EncryptedContent).c_str(), FILE_EncryptedContent, false) == 0 && showReport){
                if (!failed)
                    cout << endl << endl << "*Errors Report:" << endl, failed = true;
                perror(("Error while restoring '" + (string) FILE_EncryptedContent + "' from backup").c_str());
            }
            else if(remove(("_"+ (string)FILE_EncryptedContent).c_str()) != 0 && showReport){
                if (!failed)
                    cout << endl << endl << "*Errors Report:" << endl, failed = true;
                perror(("Error while deleting '_" + (string) FILE_EncryptedContent + "'").c_str());
            }
        }
        else if(!createFile(FILE_EncryptedContent, "") && showReport) {
            if (!failed)
                cout << endl << endl << "*Errors Report:" << endl, failed = true;
            perror(("Error while creating '" + (string) FILE_EncryptedContent + "'").c_str());
        }
    }

    if(!fileExists(FILE_EncryptedLinks)){
        if(fileExists("BKP-"+ (string)FILE_EncryptedLinks) && !fileExists("_"+ (string)FILE_EncryptedLinks)) {
            if (CopyFile(("BKP-" + (string) FILE_EncryptedLinks).c_str(), FILE_EncryptedLinks, false) == 0 && showReport){
                if (!failed)
                    cout << endl << endl << "*Errors Report:" << endl, failed = true;
                perror(("Error while restoring '" + (string) FILE_EncryptedLinks + "' from general backup").c_str());
            }
        }
        else if(fileExists("_"+ (string)FILE_EncryptedLinks)){
            if(CopyFile(("_"+ (string)FILE_EncryptedLinks).c_str(), FILE_EncryptedLinks, false) == 0 && showReport){
                if (!failed)
                    cout << endl << endl << "*Errors Report:" << endl, failed = true;
                perror(("Error while restoring '" + (string) FILE_EncryptedLinks + "' from backup").c_str());
            }
            else if(remove(("_"+ (string)FILE_EncryptedLinks).c_str()) != 0 && showReport){
                if (!failed)
                    cout << endl << endl << "*Errors Report:" << endl, failed = true;
                perror(("Error while deleting '_" + (string) FILE_EncryptedLinks + "'").c_str());
            }
        }
        else if(!createFile(FILE_EncryptedLinks, "") && showReport) {
            if (!failed)
                cout << endl << endl << "*Errors Report:" << endl, failed = true;
            perror(("Error while creating '" + (string) FILE_EncryptedLinks + "'").c_str());
        }
    }

    if(fileExists(FILE_CMD) && remove(FILE_CMD) != 0 && showReport) {
        if (!failed)
            cout << endl << endl << "*Errors Report:" << endl, failed = true;
        perror(("Error while deleting '" + (string) FILE_CMD + "'").c_str());
    }

    if(fileExists(FILE_DecryptedHTML) && remove(FILE_DecryptedHTML) != 0 && showReport) {
        if (!failed)
            cout << endl << endl << "*Errors Report:" << endl, failed = true;
        perror(("Error while deleting '" + (string) FILE_DecryptedHTML + "'").c_str());
    }

    if(sortEncryptedFile() == CODE_FunctionError && showReport) {
        if (!failed)
            cout << endl << endl << "*Errors Report:" << endl;
        cout << "Error while sorting '"+ (string)FILE_EncryptedLinks +"'." << endl;
    }
    return;
}

bool createHTMLPage(){
    vector<string> links5, links4, links3, links2, links1, content;
    ifstream file(FILE_EncryptedContent);
    string line;
    int lineAt = 0, linksCount = 0;
    if(!file.is_open())
        return false;
    while (getline(file, line)){
        line = decrypt(line, lineAt++);
        content.push_back(line);
    }
    file.close();

    file.open(FILE_EncryptedLinks);
    if(!file.is_open())
        return false;

    lineAt = 0;
    while (getline(file, line)){
        line = decrypt(line, lineAt++);
        if(line.substr(0, 6) == "***** ")
            links5.push_back(line);
        else if(line.substr(0, 6) == "****  ")
            links4.push_back(line);
        else if(line.substr(0, 6) == "***   ")
            links3.push_back(line);
        else if(line.substr(0, 6) == "**    ")
            links2.push_back(line);
        else
            links1.push_back(line);
        linksCount++;
    }
    file.close();

    //region HTML Formation
    string HTML = "<!DOCTYPE html>\n"
            "<html lang=\"pt-br\">\n"
            "<head>\n"
            "\t<meta charset=\"UTF-8\" />\n"
            "\n"
            "\t<title>inDexCrypt | Decrypted Page</title>\n"
            "    <style>\n"
            "        body{\n"
            "            background: rgba(1, 1, 1, 0.80);\n"
            "        }\n"
            "        div#header{\n"
            "            font-family: monospace, sans-serif;\n"
            "            color: aqua;\n"
            "            font-weight: bold;\n"
            "            margin: 25px 0 -15px 0;\n"
            "        }\n"
            "        h1.estrelas{\n"
            "            color: #ffde00;\n"
            "            font-size: 50px; /*OK*/\n"
            "            font-family: \"Wide Latin\", \"Bauhaus 93\", \"Berlin Sans FB\", monospace;\n"
            "            text-decoration: overline;\n"
            "            margin-bottom: -25px;\n"
            "            margin-top: 40px;\n"
            "        }\n"
            "        section{\n"
            "            background-color: #1d1d1d;\n"
            "            margin: 60px 0;\n"
            "            width: 950px;\n"
            "            border-radius: 35px;\n"
            "            padding: 0 0 20px 0;\n"
            "            font-family: \"Berlin Sans FB\", monospace;\n"
            "            font-size: 25px;\n"
            "            border: solid 5px #cfcfcf;\n"
            "        }\n"
            "        section div#content{\n"
            "            color: #0dff00;\n"
            "            /*color: greenyellow;*/\n"
            "            word-wrap: break-word;\n"
            "            padding: 0 20px;\n"
            "        }\n"
            "        div.title{\n"
            "            font-size: 35px;\n"
            "            margin: -45px 0 -10px 0;\n"
            "            background-color: #cfcfcf;\n"
            "            border-radius: 27px 27px 0 0; /*OK*/\n"
            "            position: relative;\n"
            "        }\n"
            "        div.link{\n"
            "            margin: 15px 5px;\n"
            "        }\n"
            "        a{\n"
            "            text-decoration: none;\n"
            "            color: darkgreen;\n"
            "            margin-right: 30px;\n"
            "            margin-left: 10px;\n"
            "        }\n"
            "        .preview{\n"
            "            height: 50px;\n"
            "            margin-bottom: -15px;\n"
            "            border: solid 1px white;\n"
            "        }\n"
            "        div#linksTotal{\n"
            "            margin: -40px 10px -30px 10px;\n"
            "            color: white;\n"
            "            text-align: left;\n"
            "            font-size: 25px;\n"
            "        }\n"
            "        span.id{\n"
            "            color: #F0F8FF;\n"
            "        }\n"
            "    </style>\n"
            "</head>\n"
            "<body>\n"
            "<center>\n"
            "    <div id=\"header\">\n"
            "&nbsp;_&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;_____&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;_____&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;_&nbsp;&nbsp;&nbsp;<br>\n"
            "(_)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;__&nbsp;\\&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;/&nbsp;____|&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|&nbsp;|&nbsp;&nbsp;<br>\n"
            "&nbsp;_&nbsp;&nbsp;_&nbsp;__&nbsp;&nbsp;|&nbsp;|&nbsp;&nbsp;|&nbsp;|&nbsp;&nbsp;___&nbsp;__&nbsp;&nbsp;__|&nbsp;|&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;_&nbsp;__&nbsp;&nbsp;_&nbsp;&nbsp;&nbsp;_&nbsp;&nbsp;_&nbsp;__&nbsp;&nbsp;|&nbsp;|_&nbsp;<br>\n"
            "|&nbsp;||&nbsp;'_&nbsp;\\&nbsp;|&nbsp;|&nbsp;&nbsp;|&nbsp;|&nbsp;/&nbsp;_&nbsp;\\\\&nbsp;\\/&nbsp;/|&nbsp;|&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|&nbsp;'__||&nbsp;|&nbsp;|&nbsp;||&nbsp;'_&nbsp;\\&nbsp;|&nbsp;__|<br>\n"
            "|&nbsp;||&nbsp;|&nbsp;|&nbsp;||&nbsp;|__|&nbsp;||&nbsp;&nbsp;__/&nbsp;>&nbsp;&nbsp;<&nbsp;|&nbsp;|____&nbsp;|&nbsp;|&nbsp;&nbsp;&nbsp;|&nbsp;|_|&nbsp;||&nbsp;|_)&nbsp;||&nbsp;|_&nbsp;<br>\n"
            "|_||_|&nbsp;|_||_____/&nbsp;&nbsp;\\___|/_/\\_\\&nbsp;\\_____||_|&nbsp;&nbsp;&nbsp;&nbsp;\\__,&nbsp;||&nbsp;.__/&nbsp;&nbsp;\\__|<br>\n"
            "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;__/&nbsp;||&nbsp;|&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<br>\n"
            "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|___/&nbsp;|_|&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<br>\n"
            "    </div>\n"
            "\n"
            "    <section id=\"secLinks\">\n"
            "        <div class=\"title\">\n"
            "            <h1>LINKS</h1>\n"
            "        </div>\n"
            "        <div id=\"linksTotal\">\n"
            "            Total: "+ intToString(linksCount) +"\n"
            "        </div>\n";
    HTML += "        <div>\n"
            "            <h1 class=\"estrelas\">\n"
            "                *****\n"
            "            </h1>\n";
    linksCount = 0;
    for(int i = 0; i < links5.size(); i++){
        links5[i] = links5[i].substr(6, links5[i].length()-6);
        HTML += "            <div class=\"link\">\n"
                "                <span class=\"id\">("+ intToString(++linksCount) +")</span>\n"
                "                <a href=\""+ links5[i] +"\" target=\"_blank\">"+ links5[i] +"</a>\n"
                "                <img class=\"preview\" src=\""+ links5[i] +"\" />\n"
                "            </div>\n";
    }
    HTML += "        </div>\n";

    HTML += "        <div>\n"
            "            <h1 class=\"estrelas\">\n"
            "                ****\n"
            "            </h1>\n";
    for(int i = 0; i < links4.size(); i++){
        links4[i] = links4[i].substr(6, links4[i].length()-6);
        HTML += "            <div class=\"link\">\n"
                "                <span class=\"id\">("+ intToString(++linksCount) +")</span>\n"
                "                <a href=\""+ links4[i] +"\" target=\"_blank\">"+ links4[i] +"</a>\n"
                "                <img class=\"preview\" src=\""+ links4[i] +"\" />\n"
                "            </div>\n";
    }
    HTML += "        </div>\n";

    HTML += "        <div>\n"
            "            <h1 class=\"estrelas\">\n"
            "                ***\n"
            "            </h1>\n";
    for(int i = 0; i < links3.size(); i++){
        links3[i] = links3[i].substr(6, links3[i].length()-6);
        HTML += "            <div class=\"link\">\n"
                "                <span class=\"id\">("+ intToString(++linksCount) +")</span>\n"
                "                <a href=\""+ links3[i] +"\" target=\"_blank\">"+ links3[i] +"</a>\n"
                "                <img class=\"preview\" src=\""+ links3[i] +"\" />\n"
                "            </div>\n";
    }
    HTML += "        </div>\n";

    HTML += "        <div>\n"
            "            <h1 class=\"estrelas\">\n"
            "                **\n"
            "            </h1>\n";
    for(int i = 0; i < links2.size(); i++){
        links2[i] = links2[i].substr(6, links2[i].length()-6);
        HTML += "            <div class=\"link\">\n"
                "                <span class=\"id\">("+ intToString(++linksCount) +")</span>\n"
                "                <a href=\""+ links2[i] +"\" target=\"_blank\">"+ links2[i] +"</a>\n"
                "                <img class=\"preview\" src=\""+ links2[i] +"\" />\n"
                "            </div>\n";
    }
    HTML += "        </div>\n";

    HTML += "        <div>\n"
            "            <h1 class=\"estrelas\">\n"
            "                *\n"
            "            </h1>\n";
    for(int i = 0; i < links1.size(); i++){
        links1[i] = links1[i].substr(6, links1[i].length()-6);
        HTML += "            <div class=\"link\">\n"
                "                <span class=\"id\">("+ intToString(++linksCount) +")</span>\n"
                "                <a href=\""+ links1[i] +"\" target=\"_blank\">"+ links1[i] +"</a>\n"
                "                <img class=\"preview\" src=\""+ links1[i] +"\" />\n"
                "            </div>\n";
    }
    HTML += "        </div>\n";

    HTML += "    </section>\n"
            "\n"
            "    <section id=\"secContent\">\n"
            "        <div class=\"title\">\n"
            "            <h1>CONTENT</h1>\n"
            "        </div>\n";

    HTML += "        <div id=\"content\">\n";
    for(int i = 0; i < content.size(); i++)
        HTML += content[i] + "<br>";
    HTML += "        </div>\n";

    HTML += "    </section>\n"
            "</center>\n"
            "</body>";
    //endregion

    return createFile(FILE_DecryptedHTML, HTML);
}

int selectLinks(vector<string> links, map<int, int> &selected, int func = 0){
    int c, idx = 0, page = 1;
    bool viewing = true;
    do{
        if(page*24 >= links.size())
            clearScreen();
        else
            system("cls");
        printFunctionHeader(func);

        if(links.size() == 0){
            printBlankSpaces(40, 0);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
            cout << "Nao ha links para serem selecionados.";
        }
        else {
            for (int i = 24 * (page-1); i < links.size() && i < 24 * page; i++) {
                string str = links[i];
                string substr = str.substr(0, 5);

                if (idx == i && selected[i] == 1) {
                    if (substr == "*****")
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 94);
                    else if (substr == "**** ")
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 90);
                    else if (substr == "***  ")
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 89);
                    else if (substr == "**   ")
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 93);
                    else if (substr == "*    ")
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 92);
                    else
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 95);
                } else if (idx == i) {
                    if (substr == "*****")
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 142);
                    else if (substr == "**** ")
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 138);
                    else if (substr == "***  ")
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 137);
                    else if (substr == "**   ")
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 141);
                    else if (substr == "*    ")
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 140);
                    else
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 143);
                } else if (selected[i] == 1) {
                    if (substr == "*****")
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 78);
                    else if (substr == "**** ")
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 74);
                    else if (substr == "***  ")
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 73);
                    else if (substr == "**   ")
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 77);
                    else if (substr == "*    ")
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 76);
                    else
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 79);
                } else {
                    if (substr == "*****")
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
                    else if (substr == "**** ")
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
                    else if (substr == "***  ")
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
                    else if (substr == "**   ")
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
                    else if (substr == "*    ")
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
                    else
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
                }
                printf("\t\t\t\t(%d) %s\n", i+1, links[i].c_str());
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
            }
            cout << endl;
            if(page == 1 && page*24 < links.size()) {
                //imprime -->
                printBlankSpaces(52, 15);
                cout << "Avancar (-->)";
            }
            else if(page > 1 && page*24 < links.size()) {
                //imprime <-- | -->
                printBlankSpaces(45, 15);
                cout << "(<--) Voltar | Avancar (-->)";
            }
            else if(page > 1){
                //imprime <--
                printBlankSpaces(54, 15);
                cout << "(<--) Voltar";
            }
        }
        c = getch();

        if(c == 224){
            do {
                c = getch();
                if (c == 80 && idx < links.size() - 1 && idx < page*24 - 1) {
                    ++idx;
                    break;
                }
                else if (c == 72 && idx > (page-1)*24) {
                    --idx;
                    break;
                }
                else if(c == 77 && page*24 < links.size()){
                    ++page;
                    idx = (page-1)*24;
                    break;
                }
                else if(c == 75 && page > 1){
                    --page;
                    idx = (page-1)*24;
                    break;
                }
            }while(c != 13 && c != 10 && c != 27);
        }
        if(c == 10)
            viewing = false;
        else if(c == 27){
            if(backToMenu())
                return CODE_BackToMenu;
            else
                return selectLinks(links, selected, func);
        }
        else if(c == 13) {
            if (selected[idx] == 1)
                selected[idx] = 0;
            else
                selected[idx] = 1;
        }
    }while(viewing);

    return CODE_FunctionFinished;
}
//**********************************************************************************************************************
//***************************************  Funcoes de seguranca e criptografia  ****************************************
bool passCheck(string str){
    return str == PASSWORD;
}

string encrypt(string str, int fact){
    string strEncrypted = "";
    int size = str.size();
    for(int i = 0; i < size; i++)
        strEncrypted += (char)((int)(str[i]) + ((fact + i + 2)%17));
    return strEncrypted;
}

string decrypt(string str, int fact){
    string strDecrypted = "";
    int size = str.size();
    for(int i = 0; i < size; i++) {
        strDecrypted += (char)((int) (str[i]) - ((fact + i + 2) % 17));
    }
    return  strDecrypted;
}

///#####################################################################################################################
///##################################################  Telas  ##########################################################
///#####################################################################################################################
bool quit(){
    clearScreen();
    cout << endl;
    printBlankSpaces(44, 0);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 236);
    cout << "           Atencao!           " << endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    cout << "                                            |Tem certeza que deseja sair?|" << endl;
    cout << "                                            |          (S / N)           |" << endl;
    cout << "                                            |            >               |";
    for(int i = 0; i < 90; i++)
        putch(' ');
    cout << "|____________________________|";
    for(int i = 0; i < 15 + 90 + 30; i++)
        putchar('\b');

    int digitado = 0;
    char c, res = 'n';
    do{
        c = (char) getch();
        if((c == 's' || c == 'S' || c == 'n' || c == 'N') && digitado == 0) {
            digitado = 1;
            res = c;
            putchar(c);
        }
        else if(c == 8 && digitado){
            digitado = 0;
            putchar('\b');
            putchar('\0');
            putchar('\b');
        }
        else if((c == 13 && digitado) || c == 27) //13 indica ENTER e 27 indica ESC
            break;
    }while(true);
    if(c == 27)
        res = 'n';

    return (res == 's' || res == 'S');
}

bool backToMenu(){
    clearScreen();
    cout << endl;
    printBlankSpaces(39, 0);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 236);
    cout << "                Atencao!                " << endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    cout << "                                       |Tem certeza que deseja voltar ao menu?|" << endl;
    cout << "                                       |               (S / N)                |" << endl;
    cout << "                                       |                 >                    |";
    for(int i = 0; i < 80; i++)
        putch(' ');
    cout << "|______________________________________|";
    for(int i = 0; i < 15 + 90 + 35; i++)
        putchar('\b');

    int digitado = 0;
    char c, res = 'n';
    do{
        c = (char) getch();
        if((c == 's' || c == 'S' || c == 'n' || c == 'N') && digitado == 0) {
            digitado = 1;
            res = c;
            putchar(c);
        }
        else if(c == 8 && digitado){
            digitado = 0;
            putchar('\b');
            putchar('\0');
            putchar('\b');
        }
        else if((c == 13 && digitado) || c == 27) //13 indica ENTER e 27 indica ESC
            break;
    }while(true);
    if(c == 27)
        res = 'n';

    return (res == 's' || res == 'S');
}

bool login(int aviso = 0){
    clearScreen();

    if(aviso) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0);
        cout << "                                     ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 78);
        cout << " Por favor, tome cuidado ao inserir a senha! " << endl;
    }

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << "\t\tDigite sua senha: ";

    string pass;
    int digitados = 0, errados = 0;
    bool typing = true, correct = false;
    do{
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
        char c = (char)getch();
        if(isalnum(c)){
            putchar('*');
            pass += c;
            digitados++;
        }
        if(c == 13) {
            correct = passCheck(pass);
            if (correct)
                typing = false;
            else if(errados < 5){
                errados++;
                pass = "";
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
                cout << endl << "\t\tSenha incorreta! Tente novamente." << endl;
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                cout << "\t\tDigite sua senha: ";
                digitados = 0;
            }
            else
                return login(1);
        }
        else if(c == 8 && digitados){ //8 indica que a tecla BACKSPACE foi pressionada
            digitados--;
            putchar('\b');
            putchar('\0');
            putchar('\b');
            string str = pass.substr(0, pass.length()-1);
            pass = str;
        }
        else if(c == 27){ //27 indica que o ESC foi pressionado
            if(quit())
                typing = false;
            else
                return login();
        }
    }while(typing);
    return correct;
}

void informFunctionError(int func){
    clearScreen();
    int espacamento = 42;
    cout << endl << endl;
    printBlankSpaces(espacamento, 0);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 78);
    cout << "              Atencao!              " << endl;
    printBlankSpaces(espacamento, 0);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    cout << "|   Ocorreu um erro com a funcao   |" << endl;
    printBlankSpaces(espacamento, 0);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    cout << "| ";
    printBlankSpaces(16 - menuOptions[func-1].size()/2, 12);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
    cout << menuOptions[func-1];
    printBlankSpaces(16 - (menuOptions[func-1].size() - menuOptions[func-1].size()/2), 12);
    cout << " |" << endl;
    printBlankSpaces(espacamento, 0);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    cout << "|  Ela NAO encerrou corretamente!  |" << endl;
    printBlankSpaces(espacamento, 0);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    cout << "|__________________________________|" << endl;

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << endl << endl << endl << "\tAperte ENTER para prosseguir...";

    int c;
    do{
        c = getch();
    }while(c != 13);
    return;
}

int screenEncryptContent(string content = "", bool overwrite = false, int func = MENU_EncryptContent){
    clearScreen();
    printFunctionHeader(func);

    bool typing = true;
    int digitados = content.length(), c;

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    int i = 0;
    for(i = 0; i < content.length(); i++) {
        if((int)content[i] == 13)
            printf("\n");
        else
            putchar(content[i]);
    }
    do{
        c = getch();

        if(c == 10)
            typing = false;
        else if(c == 27){
            if(backToMenu())
                return CODE_BackToMenu;
            else
                return screenEncryptContent(content, overwrite, func);
        }
        else if(c == 8 && digitados > 0){
            int n = 0;
            if((int)content[--digitados] == 13){
                n = previousLineLength(digitados, content);
                for(i = 0; i < 120-n; i++)
                    putchar('\b');
            }

            content = content.substr(0, content.length()-1);
            putchar('\b');
            putchar('\0');
            putchar('\b');
        }
        else if(digitados < StrLenMax && ((32 <= c && c <= 126) || c == 13)){
            if(c == 13) {
                printf("\n");
                content += (char) c;
                digitados++;
            }
            else if(c != 42 || digitados > 1){
                putchar(c);
                content += (char) c;
                digitados++;
            }
        }
    }while(typing);

    int res;
    if(overwrite) {
        vector<string> updatedContent;
        updatedContent.push_back(content);
        res = overwriteEncryptedFile(updatedContent, "content");
    }
    else
        res = addContentInEncryptedFile(content);

    return res;
}

int screenEncryptLink(string content = ""){
    clearScreen();
    printFunctionHeader(MENU_EncryptLink);

    int c, digitados = 0;
    bool typing = true;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
    cout << "\t______________________________" << endl;
    cout << "\t|Link: ";

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
    for(int i = 0; i < content.length(); i++, digitados++)
        putchar(content[i]);
    do{
        c = getch();

        if(c == 13 && digitados)
            typing = false;
        else if(c == 27){
            if(backToMenu())
                return CODE_BackToMenu;
            else
                return screenEncryptLink(content);
        }
        else if(c == 8 && digitados > 0){
            content = content.substr(0, content.length()-1);
            --digitados;
            putchar('\b');
            putchar('\0');
            putchar('\b');
        }
        else if(digitados < StrLenMax && 32 <= c && c <= 126){
            putchar(c);

            content += (char) c;
            ++digitados;
        }
    }while(typing);

    int nota = 0;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
    cout << endl << "\t|Nota: ";
    digitados = 0;
    do{
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
        c = getch();
        if((int)'0' < c && c <= (int)'5' && digitados < 1) {
            digitados = 1;
            nota = c - (int)'0';
            putchar(c);
        }
        else if(c == 8 && digitados){
            digitados = 0;
            nota = 0;
            putchar('\b');
            putchar('\0');
            putchar('\b');
        }
        else if(c == 27){
            if(backToMenu())
                return CODE_BackToMenu;
            else
                return screenEncryptLink(content);
        }
        else if(c == 13 && digitados)
            break;
    }while(true);

    if(!regex_match(content, regex(REGEX_URL))) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
        cout << endl << endl << "\tLink invalido." << endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
        cout << "\tAperte qualquer tecla para tentar novamente.";
        getch();
        return screenEncryptLink();
    }
    ifstream arquivo (FILE_EncryptedLinks);
    if(!arquivo.is_open())
        return CODE_FunctionError;
    vector<string> links;
    string line;
    int lineAt = 0;
    while (getline(arquivo, line)) {
        line = decrypt(line, lineAt++);
        links.push_back(line);
    }
    arquivo.close();

    quicksort(links, 0, links.size()-1, true);
    int ret = vectorContainsString(links, content);
    if(ret != -1){
        nota = 0;
        for(int i = 0; i < 5; i++) if((char)links[ret][i] == (char)'*') nota++;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
        cout << endl << endl << "\tEste link ja foi adicionado. Ele esta classificado com nota " << nota << "." << endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
        cout << "\tAperte qualquer tecla para tentar novamente.";
        getch();
        return screenEncryptLink();
    }

    return addLinkInEncryptedFile(content, nota);
}

int screenEditContent(){
    string content = "";
    ifstream arquivo (FILE_EncryptedContent);
    if(!arquivo.is_open())
        return CODE_FunctionError;

    string line;
    int lineAt = 0;
    while (getline(arquivo, line)) {
        line = decrypt(line, lineAt++);
        content += line + (char)13;
    }
    arquivo.close();

    return screenEncryptContent(content, true, MENU_EditContent);
}

int screenRemoveLinks(map<int, int> selected){
    vector<string> links, updatedLinks;

    ifstream arquivo (FILE_EncryptedLinks);
    if(!arquivo.is_open())
        return CODE_FunctionError;

    string line;
    int lineAt = 0;
    while (getline(arquivo, line)) {
        line = decrypt(line, lineAt++);
        links.push_back(line);
    }
    arquivo.close();

    int ret = selectLinks(links, selected, MENU_RemoveLinks);
    if(ret != CODE_FunctionFinished)
        return ret;

    for(int i = 0; i < links.size(); i++)
        if(selected[i] != 1)
            updatedLinks.push_back(links[i]);

    return overwriteEncryptedFile(updatedLinks, "link");
}

int screenOpenLinks(map<int, int> selected){
    vector<string> links, linksToOpen;

    ifstream arquivo (FILE_EncryptedLinks);
    if(!arquivo.is_open())
        return CODE_FunctionError;

    string line;
    int lineAt = 0;
    while (getline(arquivo, line)) {
        line = decrypt(line, lineAt++);
        links.push_back(line);
    }
    arquivo.close();

    int ret = selectLinks(links, selected, MENU_OpenLinks);
    if(ret != CODE_FunctionFinished)
        return ret;

    for(int i = 0; i < links.size(); i++)
        if(selected[i] == 1)
            linksToOpen.push_back(links[i]);
    if(linksToOpen.size() == 0)
        return CODE_FunctionFinished;

    clearScreen();
    printFunctionHeader(MENU_OpenLinks);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    char midH = (char)205, midV = (char)186, cornerTL = (char)201, cornerTR = (char)187, cornerBL = (char)200, cornerBR = (char)188;
    cout << "\t\t\t\t\b" << cornerTL; for(int i = 0; i < 56; i++)cout << midH; cout << cornerTR << endl;
    cout << "\t\t\t\t\b" << midV << " Digite o intervalo, em segundos, entre os links: ";
    cout << "      " << midV << "                                                              " << cornerBL; for(int i = 0; i < 56; i++) cout << midH; cout << cornerBR;
    for(int i = 0; i < 127; i++)
        putchar('\b');
    int c, digitados = 0;
    string tempo = "";
    do{
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
        c = getch();
        if((int)'0' <= c && c <= (int)'9' && digitados < 5) {
            digitados++;
            tempo += c;
            putchar(c);
        }
        else if(c == 8 && digitados){
            digitados--;
            tempo = tempo.substr(0, tempo.size()-1);
            putchar('\b');
            putchar('\0');
            putchar('\b');
        }
        else if(c == 27){
            if(backToMenu())
                return CODE_BackToMenu;
            else{
                clearScreen();
                printFunctionHeader(MENU_OpenLinks);

                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
                cout << "\t\t\t\t\b" << cornerTL; for(int i = 0; i < 56; i++)cout << midH; cout << cornerTR << endl;
                cout << "\t\t\t\t\b" << midV << " Digite o intervalo, em segundos, entre os links: ";
                cout << "      " << midV << "                                                              " << cornerBL; for(int i = 0; i < 56; i++) cout << midH; cout << cornerBR;
                for(int i = 0; i < 127; i++)
                    putchar('\b');
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
                cout << tempo;
            }
        }
        else if(c == 13 && digitados)
            break;
    }while(true);

    int time = atoi(tempo.c_str());

    arquivo.open(FILE_UserSettings);
    if(!arquivo.is_open())
        return CODE_FunctionError;
    getline(arquivo, line);
    arquivo.close();

    cout << endl << endl << endl;

    randomizeVector(linksToOpen);

    bool running = true, screenBack = false, esc = false;
    int threadAt = 0, returnCode = CODE_FunctionFinished;
    #pragma omp parallel num_threads(3)
    {
        if(++threadAt == 1){
            if(running){
                for(int i = 0; i < linksToOpen.size() && running; i++){
                    string link = linksToOpen[i].substr(6, linksToOpen[i].size());
                    if(!createFile(FILE_CMD, "@echo off\n"+ line +" "+ link +"")){
                        returnCode = CODE_FunctionError; running = false; break;
                    }
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
                    if(!screenBack)
                        cout << "Abrindo (" << i+1 << "/" << linksToOpen.size() <<") "+ link + "..." << endl;
                    system(FILE_CMD);
                    remove(FILE_CMD);

                    if(i < linksToOpen.size() - 1) {
                        for(float k = 0.05; k <= time && running; k += 0.05) {
                            usleep(50000);
                        }
                    }
                }
                running = false;
            }
        }
        else if(threadAt == 2){
            while(running){
                if(esc && running){
                    getch();
                    screenBack = true;
                    if(running) {
                        if (backToMenu()) {
                            running = false;
                            if (returnCode != CODE_FunctionError)
                                returnCode = CODE_BackToMenu;
                            break;
                        } else if (running) {
                            clearScreen();
                            printFunctionHeader(MENU_OpenLinks);

                            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
                            cout << "\t\t\t\t\b" << cornerTL;
                            for (int i = 0; i < 56; i++)cout << midH;
                            cout << cornerTR << endl;
                            cout << "\t\t\t\t\b" << midV << " Digite o intervalo, em segundos, entre os links: ";
                            cout << "      " << midV << "                                                              "
                                 << cornerBL;
                            for (int i = 0; i < 56; i++) cout << midH;
                            cout << cornerBR;
                            for (int i = 0; i < 127; i++)
                                putchar('\b');
                            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
                            cout << tempo << endl << endl << endl;
                        }
                        screenBack = false;
                    }
                    esc = false;
                }
            }
        }
        else{
            while(running){
                esc = GetAsyncKeyState(VK_ESCAPE) != 0;
                usleep(100000);
            }
        }
    }

    return returnCode;
}

int screenDecryptAll(){
    clearScreen();
    printFunctionHeader(MENU_DecryptAll);

    ifstream file(FILE_EncryptedLinks);
    string line;
    int lineAt = 0;
    if (file.is_open()) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        while (getline(file, line)) {
            line = decrypt(line, lineAt++);
            string substr = line.substr(0, 5);
            if (substr == "*****")
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
            else if (substr == "**** ")
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
            else if (substr == "***  ")
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
            else if (substr == "**   ")
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
            else if (substr == "*    ")
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
            else
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

            cout << "\t(" << lineAt << ") " << line << endl;
        }
        file.close();

        printBlankSpaces(30, 8);
        cout << "------------------------------------------------------------" << endl;

        file.open(FILE_EncryptedContent);
        lineAt = 0;
        if (file.is_open()) {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
            while (getline(file, line)) {
                line = decrypt(line, lineAt++);
                cout << "\t" << line << endl;
            }
            file.close();
        } else {
            cout << "Unable to open file" << endl;
        }

    } else {
        cout << "Unable to open file" << endl;
    }

    int c;
    while (c = getch(), c != 10 && c != 27);

    int op = CODE_FunctionFinished;
    if (c == 27)
        op = backToMenu() ? CODE_BackToMenu : screenDecryptAll();

    return op;
}

int screenDecryptedPage(){
    clearScreen();
    printFunctionHeader(MENU_DecryptedPage);

    string loading = "Carregando...";
    cout << endl << endl;
    printBlankSpaces(59 - loading.size()/2, 10);
    cout << loading;

    ifstream arquivo(FILE_UserSettings);
    string line;
    getline(arquivo, line);
    arquivo.close();

    if(!createHTMLPage())
        return CODE_FunctionError;
    if(!createFile(FILE_CMD, "@echo off\n"+ line +" "+ FILE_DecryptedHTML +""))
        return CODE_FunctionError;
    system(FILE_CMD);

    int c = 0;
    bool viewing = true;
    while(viewing) {
        clearScreen();
        printFunctionHeader(MENU_DecryptedPage);
        printBlankSpaces(59, 0);
        do {
            c = getch();
            if (c == 10)
                viewing = false;
            else if (c == 27) {
                if (backToMenu())
                    viewing = false;
            }
        } while (viewing && c != 27);
    }

    if(remove(FILE_DecryptedHTML) != 0 || remove(FILE_CMD) != 0)
        return CODE_FunctionError;

    return CODE_FunctionFinished;
}

int mainMenu(int option = 1){
    bool menu = true;
    int op = 0, sel;
    do {
        clearScreen();

        cout << endl;
        printBlankSpaces(42, 0);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 27);
        cout << "               Menu               " << endl;

        for(int i = 0; i < MENU_OPTIONS; i++){
            printBlankSpaces(42, 11);
            if(option == i + 1)
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 185);
            cout << "  >" << menuOptions[i];
            printBlankSpaces((30 -1) - menuOptions[i].length(), option == i+1 ? 185 : 11);
            cout << "  " << endl;
        }
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0);
        handleBackupAndTempFiles(true);
        bool cont = false;
        do {
            sel = getch();

            if (sel == 27) { //27 indica ESC
                if (quit()) {
                    menu = false;
                    op = CODE_EXIT;
                }
                else
                    cont = true;
            } else if(sel == 12){ //12 indica CTRL + L
                menu = false;
                op = CODE_LOGOUT;
            }
            else if (sel == 13) { //13 indica ENTER
                menu = false;
                op = option;
            }
            else if (sel == 224) { //224 indica que alguma seta foi pressionada
                sel = getch();

                if (sel == 72) { //72 indica que a seta ↑ foi apertada
                    if (option > 1) {
                        option--;
                        cont = true;
                    }
                }
                else if (sel == 80) { //80 indica que a seta ↓ foi apertada
                    if (option < MENU_OPTIONS) {
                        option++;
                        cont = true;
                    }
                }
            }
            else if('0' <= sel && sel <= '9'){
                sel -= '0';
                if(sel == 0)
                    sel = 10;
                if(sel <= MENU_OPTIONS) {
                    option = sel;
                    cont = true;
                }
            }
        }while(!cont && menu);
    }while(menu);

    return op;
}

int main(){
    constructMenuOptionsArray();

    int res = CODE_FunctionError;
    bool control = true;
    while(control) {
        control = false;
        bool logged = login(0);
        if (logged) {
            bool menu = true;
            while (menu) {
                int op = mainMenu(); res = CODE_FunctionFinished;
                if (op == MENU_EncryptContent) {
                    res = screenEncryptContent();
                } else if(op == MENU_EncryptLink){
                    res = screenEncryptLink();
                } else if(op == MENU_EditContent){
                    res = screenEditContent();
                } else if(op == MENU_RemoveLinks){
                    res = screenRemoveLinks(mapIntNull);
                } else if(op == MENU_OpenLinks){
                    res = screenOpenLinks(mapIntNull);
                } else if (op == MENU_DecryptAll) {
                    res = screenDecryptAll();
                } else if(op == MENU_DecryptedPage){
                    res = screenDecryptedPage();
                } else if (op == CODE_EXIT)
                    menu = false;
                else if(op == CODE_LOGOUT) {
                    menu = false;
                    control = true;
                }

                if(res == CODE_FunctionError)
                    informFunctionError(op);
            }
        }
    }
    if(res != CODE_FunctionError){
        CopyFile(FILE_EncryptedLinks, ("BKP-"+ (string)FILE_EncryptedLinks).c_str(), false);
        CopyFile(FILE_EncryptedContent, ("BKP-"+ (string)FILE_EncryptedContent).c_str(), false);
    }
    system("cls");
    return  0;
}