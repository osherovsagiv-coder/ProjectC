#ifndef SERVER_H
#define SERVER_H

/* Status values */
#define SERVER_HEALTHY 0
#define SERVER_INFECTED 1
#define SERVER_PROTECTED 2
#define SERVER_DISABLED 3

/* Default infection time */
#define NOT_INFECTED -1.0

/* Server data */
typedef struct ServerData
{
    int id;
    int status;
    int security_level;
    double infection_time;
    int attack_attempts;
    int successful_attacks;
} ServerData;

/* Initialize server data */
void init_server(ServerData* server, int id, int security_level);

/* Create a new server data object */
ServerData* create_server(int id, int security_level);

/* Check if the server can be infected */
int can_be_infected(const ServerData* server);

/* Check if the server is infected */
int is_server_infected(const ServerData* server);

/* Infect the server at a given time */
void infect_server(ServerData* server, double time);

/* Protect the server from infection */
void protect_server(ServerData* server);

/* Disable the server */
void disable_server(ServerData* server);

/* Print one server */
void print_server(const ServerData* server);

/* Convert server status to text */
const char* status_to_string(int status);

#endif
