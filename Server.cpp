#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <thread>
#include <vector>
#include "databaseConnection.cpp"
#include "Logger.h"

#define PORT 7777

using std::cout;
using std::endl;

Logger logger("log.txt");

PGconn* conn = conDB();
std::string messages;
int output, input, chatid;

std::vector<std::thread> threads;

void writer(Logger& logger, std::string ms) {
    logger.writeLog(ms);
}
void reader(Logger& logger, std::string ms) {
    std::string log = logger.readLog();
    if(!log.empty())
        std::cout << log << std::endl;
}
void joinThread() {
    for(auto& t : threads) t.join();
}

class Server {
    protected:
        int serverD, socketD;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);
        char buffer[1024] = {0};

        std::string l, p, n, result, out, inp, al;
    public:
        Server() { }
        ~Server() { }

        void StartServer() {
            if((serverD = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
                perror("Socket failed");
                ExitProgram();
            }
            if(setsockopt(serverD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
                perror("Setsockopt");
                ExitProgram();
            }
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(PORT);
            if(bind(serverD, (struct sockaddr*)&address, sizeof(address)) < 0) {
                perror("Bind failed");
                ExitProgram();
            }
            if(listen(serverD, 3) < 0) {
                perror("Listen");
                ExitProgram();
            }

            cout << "Server waiting any connection..." << endl;

            if((socketD = accept(serverD, (struct sockaddr*)&address, (socklen_t*)&address)) < 0) {
                perror("accept");
                ExitProgram();
            }
            cout << "User online." << endl;

            while(true) {
                MainMenu(socketD);

                std::string choice = OnMessage();
                
                if(choice == "1") RegMenu(socketD, choice);
                else if(choice == "2") {
                    if(LoginMenu(socketD, choice)) {
                        while(true) {
                            ChatMenu(socketD);

                            std::string choice = OnMessage();

                            if(choice == "1") {
                                SendMessageAll(socketD, choice, output);
                            } else if(choice == "2") {
                                EnterMessage(querySelect(conn, "select * from users") + "\nselect login for input message\n");
                                std::string tst = OnMessage();
                                std::stringstream res;
                                res << "select id_user from users where name_user = '" << tst << "'";
                                input = queryDBInt(conn, res.str().c_str());
                                
                                std::stringstream per;
                                per << "select count(*) from chat_users where id_useroutput = " << output << " and id_userinput = " << input << " and deleted_chat = 'false'";
                                std::stringstream per1;
                                per1 << "select count(*) from chat_users where id_userinput = " << output << " and id_useroutput = " << input << " and deleted_chat = 'false'";

                                if(queryDBInt(conn, per.str().c_str()) > 0) {
                                    std::stringstream pgr;
                                    pgr << "select id_chat from chat_users where id_useroutput = " << output << " and id_userinput = " << input;
                                    chatid = queryDBInt(conn, pgr.str().c_str());
                                    SendMessageOneUser(socketD, choice, output, chatid);
                                }
                                else if(queryDBInt(conn, per1.str().c_str())) {
                                    std::stringstream pgr;
                                    pgr << "select id_chat from chat_users where id_userinput = " << output << " and id_useroutput = " << input;
                                    chatid = queryDBInt(conn, pgr.str().c_str());
                                    SendMessageOneUser(socketD, choice, output, chatid);
                                }
                                else {
                                    std::stringstream query;
                                    query << "insert into chat_users(id_useroutput, id_userinput) values("<< output << ", " << input << ")";
                                    queryDB(conn, query.str().c_str());

                                    std::stringstream st;
                                    st << "select id_chat from chat_users where id_useroutput = " << output << " and id_userinput = " << input;
                                    chatid = queryDBInt(conn, st.str().c_str());
                                    SendMessageOneUser(socketD, choice, output, chatid);
                                }
                            } else if(choice == "3") {
                                OpenChatAll(socketD, choice, output);
                            } else if(choice == "4") {
                                EnterMessage(querySelect(conn, "select * from users") + "\nselect login for input message\n");
                                std::string tst = OnMessage();
                                std::stringstream res;
                                res << "select id_user from users where name_user = '" << tst << "'";
                                input = queryDBInt(conn, res.str().c_str());
                                
                                std::stringstream per;
                                per << "select count(*) from chat_users where id_useroutput = " << output << " and id_userinput = " << input << " and deleted_chat = 'false'";
                                std::stringstream per1;
                                per1 << "select count(*) from chat_users where id_userinput = " << output << " and id_useroutput = " << input << " and deleted_chat = 'false'";

                                if(queryDBInt(conn, per.str().c_str()) > 0) {
                                    std::stringstream pgr;
                                    pgr << "select id_chat from chat_users where id_useroutput = " << output << " and id_userinput = " << input;
                                    chatid = queryDBInt(conn, pgr.str().c_str());
                                    OpenOneChat(socketD, choice, output, input, chatid);
                                }
                                else if(queryDBInt(conn, per1.str().c_str())) {
                                    std::stringstream pgr;
                                    pgr << "select id_chat from chat_users where id_userinput = " << output << " and id_useroutput = " << input;
                                    chatid = queryDBInt(conn, pgr.str().c_str());
                                    OpenOneChat(socketD, choice, input, output, chatid);
                                }
                                else EnterMessage("Non chat");
                            } else if(choice == "5") {
                                RebindLogin(socketD, choice, output);
                                break;
                            } else if(choice == "6") {
                                RebindPass(socketD, choice, output);
                                break;
                            } else if(choice == "7") {
                                RebindName(socketD, choice, output);
                                break;
                            } else if(choice == "0") break;
                            else EnterMessage("This option not yet");
                        }
                    }
                } else if(choice == "3") EnterMessage(querySelect(conn, "select * from users"));
                else if(choice == "4") {
                    EnterMessage(querySelect(conn, "select * from users"));
                    EnterMessage("Enter name user for ban: ");
                    std::string tst = OnMessage();
                    std::stringstream query;
                    query << "update users set enable_user = 'false' where name_user = '" << tst << "'";
                    queryDB(conn, query.str().c_str());
                } else if(choice == "5") {
                    EnterMessage(querySelect(conn, "select * from users"));
                    EnterMessage("Enter name user for unban: ");
                    std::string tst = OnMessage();
                    std::stringstream query;
                    query << "update users set enable_user = 'true' where name_user = '" << tst << "'";
                    queryDB(conn, query.str().c_str());
                } else if(choice == "endl") ExitProgram();
                else EnterMessage("This option not yet");
            }
        }

        void EnterMessage(std::string response) {
            send(socketD, response.c_str(), response.length(), 0);
        }

        std::string OnMessage() {
            memset(buffer, 0, sizeof(buffer));
            int bytes = read(socketD, buffer, 1024);
            if(bytes <= 0) {
                std::cout << "User offline." << std::endl;
                ExitProgram();
            } 
            std::cout << buffer << std::endl;
            return buffer;
        }

        void MainMenu(int socketD) {
            EnterMessage("\nWellcome to the chat!\n\n"
                "Choice options: "
                "\n1 - Registration with user"
                "\n2 - Login in chat"
                "\n3 - View all user"
                "\n4 - Ban user"
                "\n5 - Unban user"
                "\nendl - Exit program\n");
        }
        void RegMenu(int socketD, std::string choice) {
            int _check = queryDBInt(conn, "select count(*) from users");
            if(_check == 0) EnterMessage("\nNot user in program");
            else {
                EnterMessage("\nEnter login for new user: ");
                l = OnMessage();
    
                EnterMessage("Enter password for new user: ");
                p = OnMessage();
    
                EnterMessage("Enter name for new user: ");
                n = OnMessage();
                
                queryDB(conn, ("insert into users(login_user, pass_user, name_user) values('" + l + "', '" + p + "', '" + n + "')").c_str());
            }
        }
        bool LoginMenu(int socketD, std::string choice) {
            int _check = queryDBInt(conn, "select count(*) from users");
            
            if(_check == 0) EnterMessage("\nNot user in program");
            else {
                EnterMessage("Enter login user: ");
                l = OnMessage();
                EnterMessage("Enter password user: ");
                p = OnMessage();

                output = queryDBInt(conn, ("select id_user from users where login_user = '" + l + "' and pass_user = '" + p + "'").c_str());

                if(output != 0) return true;
                else return false;
            }
            return false;
        }
        void ChatMenu(int socketD) {
            EnterMessage("Use chioce: "
                "\n1 - Send message all"
                "\n2 - Send message one user"
                "\n3 - Open chat all"
                "\n4 - Open chat one user"
                "\n5 - Rebind login"
                "\n6 - Rebind password"
                "\n7 - Rebind name"
                "\n0 - Back to main menu");
        }
        void SendMessageAll(int socketD, std::string choice, int output) {
            EnterMessage("Enter all chat\n Enter message: ");
            std::string text = OnMessage();
            std::stringstream query;
            query << "insert into all_chat(id_user, message_chatall) values(" << output << ", " << PQescapeLiteral(conn, text.c_str(), text.length()) << ")";
            threads.emplace_back(writer, std::ref(logger), "0 " + std::to_string(output) + " " + text);
            queryDB(conn, query.str().c_str());
            EnterMessage("Message upload\n");
        }
        void SendMessageOneUser(int socketD, std::string choice, int output, int chatid) {
            EnterMessage("Send message: ");
            std::string text = OnMessage();
            std::stringstream option;
            threads.emplace_back(writer, std::ref(logger), std::to_string(chatid) + " " + std::to_string(output)+ " " + text);
            joinThread();
            option << "insert into msg_usr_chat(id_chat, id_user_upload, message_chat) values(" << chatid << ", " << output << ", '" << text << "')";
            queryDB(conn, option.str().c_str());
            EnterMessage("Message upload\n");
        }
        void OpenChatAll(int socketD, std::string choice, int output) {
            EnterMessage(querySelect(conn, "select * from all_chat"));
            threads.emplace_back(reader, std::ref(logger), "0");
            joinThread();
        }
        void OpenOneChat(int socketD, std::string choice, int output, int input, int chatid) {
            std::stringstream que;
            que << "select usr.name_user, message_chat from msg_usr_chat muc inner join users usr on muc.id_user_upload = usr.id_user";
            EnterMessage(querySelect(conn, que.str().c_str()));
            threads.emplace_back(reader, std::ref(logger), std::to_string(chatid));
            joinThread();
        }
        void RebindLogin(int socketD, std::string choice, int output) {
            EnterMessage("Enter new login: \n");
            std::string text = OnMessage();
            std::stringstream que;
            que << "update users set login_user = '" << text << "' where id_user = " << output;
            queryDB(conn, que.str().c_str());
            EnterMessage("Update login");
        }
        void RebindPass(int socketD, std::string choice, int output) {
            EnterMessage("Enter new pass: \n");
            std::string text = OnMessage();
            std::stringstream que;
            que << "update users set pass_user = '" << text << "' where id_user = " << output;
            queryDB(conn, que.str().c_str());
            EnterMessage("Update password");
        }
        void RebindName(int socketD, std::string choice, int output) {
            EnterMessage("Enter new name: \n");
            std::string text = OnMessage();
            std::stringstream que;
            que << "update users set name_user = '" << text << "' where id_user = " << output;
            queryDB(conn, que.str().c_str());
            EnterMessage("Update name");
        } 
        void ExitProgram() {
            exitConnection(conn);
            close(socketD);
            exit(1);
        }
};
