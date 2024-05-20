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
            ", razao_social CHAR(50)"
            ", nome_fantasia CHAR(50)"
        ");"
        "CREATE TABLE IF NOT EXISTS  "
        "waste("
            "id INTEGER AUTOINCREMENT"
            ", client_id INTEGER NOT NULL"
            ", quantity INTEGER NOT NULL"
            ", value INTEGER FOREIGN KEY(client_id) REFERENCES(clients.id)"
            ", PRIMARY KEY(id, client_id)"
        ");";
    sqlite3_open("sqlite.db", &db);
    sqlite3_exec(db, query, NULL, NULL, NULL);
    sqlite3_close(db);
}

int main()
{
    setup();
    printf("Welcome!\n");
    createClient("99.999.999/9999-99", "test S/S ltda", "test");
    return 0;
}

int The_Callback(void *a_param, int argc, char **argv, char **column){
    for (int i=0; i< argc; i++)
        printf("%s,\t", argv[i]);
    printf("\n");
    return 0;
}

void clearDatabase() {
    sqlite3_open("sqlite.db", &db);
    sqlite3_db_config(db, SQLITE_DBCONFIG_RESET_DATABASE, 1, 0);
    sqlite3_exec(db, "VACUUM", 0, 0, 0);
    sqlite3_db_config(db, SQLITE_DBCONFIG_RESET_DATABASE, 0, 0);
    sqlite3_close(db);
}

int createClient(char cnpj[20], char razao_social[50], char nome_fantasia[50]) {
    try{
        sqlite3_open("sqlite.db", &db);
        char* query = sqlite3_mprintf(
            "INSERT INTO clients(cnpj, razao_social, nome_fantasia) "
            "VALUES(%Q, %Q, %Q)"
            , cnpj, razao_social, nome_fantasia
        );
        sqlite3_exec(db, query, The_Callback, NULL, NULL);
        sqlite3_exec(db, "select * from clients", The_Callback, NULL, NULL);
        sqlite3_free(query);
        sqlite3_close(db);
        return (1);
    }
    catch(){
        return (0);
    }
}
