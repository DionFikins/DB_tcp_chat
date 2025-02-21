#include <iostream>
#include <postgresql/libpq-fe.h>

using namespace std;

void exitWithError(PGconn* con) {
    cerr << "Error: " << PQerrorMessage(con) << endl;
    PQfinish(con);
    exit(1);
}
void exitConnection(PGconn* con) {
    PQfinish(con);
}
PGconn* conDB() {
    const char* coninfo = "host=localhost port=5432 dbname=ВашиДанные user=ВашиДанные password=ВашиДанные";
    PGconn* con = PQconnectdb(coninfo);

    if(PQstatus(con) != CONNECTION_OK) exitWithError(con);
    cout << "Connection in database complited!" << endl;
    return con;
}
int queryDB(PGconn* con, const char* query) {
    PGresult* res = PQexec(con, query);
    if(PQresultStatus(res) != PGRES_COMMAND_OK) {
        cerr << "Error complite query: " << PQerrorMessage(con) << endl;
        PQclear(res);
        PQfinish(con);
        return -1;
    }
    return 0;
}
int queryDBInt(PGconn* con, const char* query) {
    PGresult* res = PQexec(con, query);
    if(PQresultStatus(res) != PGRES_TUPLES_OK) {
        cerr << "Error complite query: " << PQerrorMessage(con) << endl;
        PQclear(res);
        PQfinish(con);
        return -1;
    }
    return atoi(PQgetvalue(res, 0, 0));
}
std::string queryDBString(PGconn* con, const char* query) {
    PGresult* res = PQexec(con, query);
    if(PQresultStatus(res) != PGRES_TUPLES_OK) {
        cerr << "Error complite query: " << PQerrorMessage(con) << endl;
        PQclear(res);
        PQfinish(con);
        return "";
    }
    return PQgetvalue(res, 0, 0);
}
bool queryDBBool(PGconn* con, const char* query) {
    PGresult* res = PQexec(con, query);
    if(PQresultStatus(res) != PGRES_TUPLES_OK) {
        cerr << "Error complite query: " << PQerrorMessage(con) << endl;
        PQclear(res);
        PQfinish(con);
        return false;
    }
    
    int rows = PQntuples(res);
    if(rows == 1) {
        char* value = PQgetvalue(res, 0, 0);
        bool isEnable = (std::string(value) == "t");
        PQclear(res);
        return isEnable;
    } else {
        char* value = PQgetvalue(res, 0, 0);
        bool isEnable = (std::string(value) == "f");
        PQclear(res);
        return isEnable;
    }
}

std::string querySelect(PGconn* con, const char* query) {
    PGresult* res = PQexec(con, query);

    int rows = PQntuples(res);
    int cols = PQnfields(res);

    std::string text;

    if(query == "select * from users") {
        text += "Login, Password, Name\n";
        for(int i = 0; i < rows; i++) {
            for(int j = 0; j < cols; ++j) {
                if(j == 1 || j == 2 || j == 3) {
                    text += PQgetvalue(res, i, j);
                    text += " ";
                }
            }
            text += "\n";
        }
    } else if(query == "select * from all_chat") {
        text += "Name - Message";
        for(int i = 0; i < rows; i++) {
            for(int j = 0; j < cols; ++j) {
                if(j == 1) {
                    text += PQgetvalue(res, i, j);
                    text += " - ";
                } else if(j == 2) 
                    text += PQgetvalue(res, i, j);
            }
        }
    } else {
        text += "Result query: \n";
        for(int i = 0; i < rows; i++) {
            for(int j = 0; j < cols; ++j) {
                text += PQgetvalue(res, i, j);
                text += " ";
            }
            text += "\n";
        }
    }
    return text;
}