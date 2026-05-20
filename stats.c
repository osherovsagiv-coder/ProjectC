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

    stats.first_infection_time = NOT_INFECTED;
    stats.last_infection_time = NOT_INFECTED;

    return stats;
}

SimulationStats calculate_stats(const Network* net) {
    SimulationStats stats = init_stats();

    stats.total_servers = count_total_servers(net);

    stats.healthy_servers = count_servers_by_status(net, SERVER_HEALTHY);
    stats.infected_servers = count_servers_by_status(net, SERVER_INFECTED);
    stats.protected_servers = count_servers_by_status(net, SERVER_PROTECTED);
    stats.disabled_servers = count_servers_by_status(net, SERVER_DISABLED);

    stats.total_attack_attempts = count_total_attack_attempts(net);
    stats.total_successful_attacks = count_total_successful_attacks(net);

    stats.first_infection_time = find_first_infection_time(net);
    stats.last_infection_time = find_last_infection_time(net);

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

int count_total_attack_attempts(const Network* net) {
    return sum_server_field(net, get_attack_attempts);
}

int count_total_successful_attacks(const Network* net) {
    return sum_server_field(net, get_successful_attacks);
}

int sum_server_field(
    const Network* net,
    int (*get_value)(const ServerData* server)
) {
    const ServerNode* current = NULL;
    int total = 0;

    if (!net || !net->head || !get_value) {
        return 0;
    }

    current = net->head;

    while (current != NULL) {
        total += get_value(&current->data);
        current = current->next;
    }

    return total;
}

int get_attack_attempts(const ServerData* server) {
    return server->attack_attempts;
}

int get_successful_attacks(const ServerData* server) {
    return server->successful_attacks;
}

double find_first_infection_time(const Network* net) {
    return find_infection_time(net, is_earlier_infection_time);
}

double find_last_infection_time(const Network* net) {
    return find_infection_time(net, is_later_infection_time);
}

double find_infection_time(
    const Network* net,
    int (*should_update)(double current_time, double saved_time)
) {
    const ServerNode* current = NULL;
    double result = NOT_INFECTED;

    if (!net || !net->head || !should_update) {
        return NOT_INFECTED;
    }

    current = net->head;

    while (current != NULL) {
        if (current->data.status == SERVER_INFECTED &&
            current->data.infection_time != NOT_INFECTED) {

            if (should_update(current->data.infection_time, result)) {
                result = current->data.infection_time;
            }
        }

        current = current->next;
    }

    return result;
}

int is_earlier_infection_time(double current_time, double saved_time) {
    return saved_time == NOT_INFECTED || current_time < saved_time;
}

int is_later_infection_time(double current_time, double saved_time) {
    return saved_time == NOT_INFECTED || current_time > saved_time;
} 
 
