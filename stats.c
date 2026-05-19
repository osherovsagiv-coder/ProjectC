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

    if (!net) {
        return stats;
    }

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

    return net->server_count;
}

int count_servers_by_status(const Network* net, int status) {
    int count = 0;
    int i;

    if (!net || !net->servers) {
        return 0;
    }

    for (i = 0; i < net->server_count; i++) {
        if (net->servers[i].status == status) {
            count++;
        }
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
    int security_sum = 0;
    int i;

    if (!net || !net->servers || net->server_count == 0) {
        return 0.0;
    }

    for (i = 0; i < net->server_count; i++) {
        security_sum += net->servers[i].security_level;
    }

    return (double)security_sum / net->server_count;
}


 
