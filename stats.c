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
    stats.infection_percentage = 0.0;
    stats.average_security_level = 0.0;

    return stats;
}

SimulationStats calculate_stats(const Network* net) {
    SimulationStats stats = init_stats();

    stats.total_servers = count_total_servers(net);
    stats.healthy_servers = count_servers_by_status(net, SERVER_HEALTHY);
    stats.infected_servers = count_servers_by_status(net, SERVER_INFECTED);
    stats.protected_servers = count_servers_by_status(net, SERVER_PROTECTED);
    stats.disabled_servers = count_servers_by_status(net, SERVER_DISABLED);

    stats.infection_percentage = calculate_percentage(
        stats.infected_servers,
        stats.total_servers
    );

    stats.average_security_level = calculate_average_security_level(net);

    return stats;
}

int count_total_servers(const Network* net) {
    if (!net) {
        return 0;
    }

    return net->numOfServers;
}

int count_servers_by_status(const Network* net, int status) {
    const ServerNode* current = NULL;
    int count = 0;

    if (!net || !net->head) {
        return 0;
    }

    current = net->head;

    while (current != NULL) {
        if (current->data.status == status) {
            count++;
        }

        current = current->next;
    }

    return count;
}

double calculate_percentage(int part, int total) {
    if (total == 0) {
        return 0.0;
    }

    return (double)part * 100.0 / total;
}

double calculate_average_security_level(const Network* net) {
    const ServerNode* current = NULL;
    int security_sum = 0;
    int total_servers = 0;

    if (!net || !net->head) {
        return 0.0;
    }

    current = net->head;

    while (current != NULL) {
        security_sum += current->data.security_level;
        total_servers++;
        current = current->next;
    }

    if (total_servers == 0) {
        return 0.0;
    }

    return (double)security_sum / total_servers;
}
 
