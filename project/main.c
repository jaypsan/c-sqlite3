#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <stdbool.h>

#define try bool __HadError=false;
#define catch(x) ExitJmp:if(__HadError)
#define throw(x) {__HadError=true;goto ExitJmp;}

sqlite3 *db;


int setup(){
    printf("Running setup...");
    char query[] =
        "CREATE TABLE IF NOT EXISTS  "
        "clients("
            "id INTEGER PRIMARY KEY"
            ", cnpj VARCHAR(20)"
            ", razao_social VARCHAR(50)"
            ", nome_fantasia VARCHAR(50)"
        ");"
        "CREATE TABLE IF NOT EXISTS  "
        "waste("
            "id INTEGER PRIMARY KEY"
            ", client_id INTEGER NOT NULL"
            ", quantity INTEGER NOT NULL"
            ", value INTEGER NOT NULL"
            ", month SMALLINT NOT NULL"
            ", year SMALLINT NOT NULL"
        ");";

    sqlite3_open("sqlite.db", &db);
    sqlite3_exec(db, query, NULL, NULL, NULL);
    sqlite3_close(db);
}

int main()
{
    setup();
    //createClient("99.999.999/9999-99", "test s/s ltda", "test");
    //selectWastesByDate(5, 2024);
    printf("Welcome!\n");
    char login[10] = "admin";
    char senha[10] = "admin";
    int opt;
    while (1) {
        printf("\n--- Login ---\n");
        char l[10];
        char p[10];

        printf("Login: ");
        gets(l);
        printf("Senha: ");
        gets(p);

        if (strcmp(login, l) == 0 && strcmp(senha, p) == 0){
            break;
        } else {
            printf("\nSenha Incorreta\n");
        }
    }

    while (1){
        printf("\n--- Menu ---\n"
               "Selecione uma opcao: \n"
               "1 - Cadastrar Cliente \n"
               "2 - Cadastrar Residuos \n"
               "3 - Emitir Relatorio \n"
               "0 - Sair \n"
               "\n");
        scanf("%d", &opt);
        if (opt == 1){
            getchar(); // consume the last Enter key input
            printf("--- Cadastro de Cliente ---\n");
            char cnpj[20];
            char rs[50];
            char nf[50];
            printf("CNPJ: ");
            fgets(cnpj, sizeof cnpj, stdin);
            printf("Razao Social: ");
            fgets(rs, sizeof rs, stdin);
            printf("Nome Fantasia: ");
            fgets(nf, sizeof nf, stdin);
            // Remove line breaks
            strtok(cnpj, "\n\r");
            strtok(rs, "\n\r");
            strtok(nf, "\n\r");
            createClient(cnpj, rs, nf);
        } else if (opt == 2) {
            getchar(); // consume the last Enter key input
            printf("--- Cadastro de Residuos ---\n");
            char cnpj[20];
            int qtd;
            double value;
            int month;
            int year;
            printf("CNPJ da Empresa: ");
            fgets(cnpj, sizeof cnpj, stdin);
            printf("Quantidade: ");
            scanf("%d", &qtd);
            printf("Valor: ");
            scanf("%lf", &value);
            printf("Mes: ");
            scanf("%d", &month);
            printf("Ano: ");
            scanf("%d", &year);
            insertWaste(1, qtd, value, month, year);
        }
        else if (opt == 3) {
            getchar(); // consume the last Enter key input
            printf("--- Emitir Relatorio Mensal ---\n");
            int month;
            int year;
            printf("Mes: ");
            scanf("%d", &month);
            printf("Ano: ");
            scanf("%d", &year);
            selectWastesByDate(month, year);
        }
        else if (opt == 0) {
            break;
        } else {
            opt = NULL;
            printf("Opcao invalida");
        }
    }
    return 0;
}

int The_Callback(void *a_param, int argc, char **argv, char **column){
    for (int i=0; i< argc; i++)
        printf("%s |\t", argv[i]);
    printf("\n");
    return 0;
}
int saveFileCallback(void *a_param, int argc, char **argv, char **column){
    /* definicao do ponteiro para o arquivo */
    FILE *file;
    file = fopen("arquivo.txt", "a");
    for (int i=0; i< argc; i++){
        printf("%s |\t", argv[i]);
        fprintf(file, "%s |\t", argv[i]);
    }
    printf("\n");
    fprintf(file, "\n");
    fclose(file);
    return 0;
}

int createClient(char cnpj[20], char razao_social[50], char nome_fantasia[50]) {
    try{
        sqlite3_open("sqlite.db", &db);
        char* query = sqlite3_mprintf(
            "INSERT INTO clients(cnpj, razao_social, nome_fantasia) "
            "VALUES(trim(%Q), trim(%Q), trim(%Q))"
            , cnpj, razao_social, nome_fantasia
        );
        sqlite3_exec(db, query, The_Callback, NULL, NULL);
        printf("Cliente cadastrado com sucesso: \n");
        sqlite3_exec(db, "SELECT cnpj, razao_social, nome_fantasia FROM clients WHERE ROWID = last_insert_rowid()", The_Callback, NULL, NULL);
        sqlite3_free(query);
        sqlite3_close(db);
        return (1);
    }
    catch(){
        return (0);
    }
}

int insertWaste(int client_id, int quantity, double value, int month, int year){
    try{
        sqlite3_open("sqlite.db", &db);
        char* query = sqlite3_mprintf(
            "INSERT INTO waste(client_id, quantity, value, month, year) VALUES(%d, %d, %f, %d, %d)"
            , client_id, quantity, value, month, year
        );

        sqlite3_exec(db, query, The_Callback, NULL, NULL);
        printf("Residuo cadastrado com sucesso: \n");
        sqlite3_exec(db, "SELECT a.id, b.cnpj, b.razao_social, quantity, value, month, year"
            " FROM waste a"
            " INNER JOIN clients b ON a.client_id = b.id"
            " WHERE ROWID = last_insert_rowid()"
        , The_Callback, NULL, NULL);
        sqlite3_free(query);
        sqlite3_close(db);
        return (1);
    }
    catch(){
        return (0);
    }
}

int selectWastesByDate(int month, int year) {
    try{
        sqlite3_open("sqlite.db", &db);
        char* query = sqlite3_mprintf(
            "SELECT a.id, b.cnpj, b.razao_social, quantity, value, month, year"
            " FROM waste a"
            " INNER JOIN clients b ON a.client_id = b.id"
            " WHERE a.month like %d and a.year like %d"
            , month, year
        );
        FILE *file;
        file = fopen("arquivo.txt", "w");
        fprintf(file, "ID | CNPJ | Cliente | Quantidade | Valor | Mes | Ano | \n");
        fclose(file);
        printf("ID | CNPJ | Cliente | Quantidade | Valor | Mes | Ano | \n");
        sqlite3_exec(db, query, saveFileCallback, NULL, NULL);
        printf("Relatorio salvo em disco ./arquivo.txt \n");
        sqlite3_free(query);
        sqlite3_close(db);
        return (1);
    }
    catch(){
        return (0);
    }
}


