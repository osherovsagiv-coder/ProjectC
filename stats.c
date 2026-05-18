#include <stdio.h>

#include "stats.h"
#include "server.h"

SimulationStats init_stats(void) {
    SimulationStats stats;

    stats.total_servers = 0;

    stats.healthy_servers = 0;
    stats.infected_servers = 0;
    stats.protected_servers = 0;
    stats.disabled_servers = 0;

    stats.total_attack_attempts = 0;
    stats.total_successful_attacks = 0;

	stats.first_infection_time = NOT_INFECTED; //  not infected time is -1.0, meaning "no infection yet"
    stats.last_infection_time = NOT_INFECTED;

    return stats;
}

SimulationStats calculate_stats(const Network* net) {
    SimulationStats stats = init_stats();

    if (net == NULL) {
        return stats;
    }

    stats.total_servers = net->numOfServers;

    ServerNode* current = net->head;

    while (current != NULL) {
        const ServerData* server = &current->data;

        switch (server->status) {
        case SERVER_HEALTHY:
            stats.healthy_servers++;
            break;

        case SERVER_INFECTED:
            stats.infected_servers++;

            if (stats.first_infection_time == NOT_INFECTED ||
                server->infection_time < stats.first_infection_time) {
                stats.first_infection_time = server->infection_time;
            }

            if (stats.last_infection_time == NOT_INFECTED ||
                server->infection_time > stats.last_infection_time) {
                stats.last_infection_time = server->infection_time;
            }
            break;

        case SERVER_PROTECTED:
            stats.protected_servers++;
            break;

        case SERVER_DISABLED:
            stats.disabled_servers++;
            break;

        default:
            break;
        }

        stats.total_attack_attempts += server->attack_attempts;
        stats.total_successful_attacks += server->successful_attacks;

        current = current->next;
    }

    return stats;
}

 