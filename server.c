#include <stdio.h>
#include <stdlib.h>

#include "server.h"

void init_server(ServerData* server, int id, int security_level) {
    if (!server) {
        fprintf(stderr, "Invalid server pointer\n");
        return;
    }

    server->id = id;
    server->status = SERVER_HEALTHY;
    server->security_level = security_level;
    server->infection_time = NOT_INFECTED;
    server->attack_attempts = 0;
    server->successful_attacks = 0;
}

ServerData* create_server(int id, int security_level) {
    ServerData* new_server = NULL;

    new_server = (ServerData*)malloc(sizeof(ServerData));

    if (!new_server) {
        fprintf(stderr, "Memory allocation failed for server %d\n", id);
        return NULL;
    }

    init_server(new_server, id, security_level);
    return new_server;
}

int can_be_infected(const ServerData* server) {
    if (!server) {
        fprintf(stderr, "Invalid server pointer for infection check\n");
        return 0;
    }

    if (server->status != SERVER_HEALTHY) {
        return 0;
    }

    return 1;
}

int is_server_infected(const ServerData* server) {
    if (!server) {
        fprintf(stderr, "Invalid server pointer for infection status check\n");
        return 0;
    }

    return server->status == SERVER_INFECTED;
}

void infect_server(ServerData* server, double time) {
    if (!server) {
        fprintf(stderr, "Invalid server pointer for infection\n");
        return;
    }

    if (can_be_infected(server)) {
        server->status = SERVER_INFECTED;
        server->infection_time = time;
    }
}

void protect_server(ServerData* server) {
    if (!server) {
        fprintf(stderr, "Invalid server pointer for protection\n");
        return;
    }

    if (server->status == SERVER_HEALTHY) {
        server->status = SERVER_PROTECTED;
    }
}

void disable_server(ServerData* server) {
    if (!server) {
        fprintf(stderr, "Invalid server pointer for disabling\n");
        return;
    }

    server->status = SERVER_DISABLED;
}

void print_server(const ServerData* server) {
    if (!server) {
        fprintf(stderr, "Invalid server pointer for printing\n");
        return;
    }

    printf("Server ID: %d\n", server->id);
    printf("Status: %s\n", status_to_string(server->status));
    printf("Security Level: %d\n", server->security_level);
    printf("Infection Time: %.2f\n", server->infection_time);
    printf("Attack Attempts: %d\n", server->attack_attempts);
    printf("Successful Attacks: %d\n", server->successful_attacks);
}

const char* status_to_string(int status) {
    switch (status) {
    case SERVER_HEALTHY:
        return "Healthy";
    case SERVER_INFECTED:
        return "Infected";
    case SERVER_PROTECTED:
        return "Protected";
    case SERVER_DISABLED:
        return "Disabled";
    default:
        return "Unknown";
    }
}
