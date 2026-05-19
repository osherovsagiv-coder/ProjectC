#ifndef STATS_H
#define STATS_H

#include "network.h"

/*
  SimulationStats stores the final summary of the simulation.
  It does not change the network, it only describes what happened after or during the simulation.
 */
typedef struct SimulationStats {
    int total_servers;

    int healthy_servers;
    int infected_servers;
    int protected_servers;
    int disabled_servers;

	int total_attack_attempts;      // Total number of times any server was attacked (including failed attempts)
	int total_successful_attacks;     // Total number of times a server was successfully infected (excluding protected or disabled servers)

    double first_infection_time;
    double last_infection_time;
} SimulationStats;

/* Main stats functions */

// Creates an empty statistics object.

SimulationStats init_stats(void);

// Reads the current network state and calculates statistics.
 
SimulationStats calculate_stats(const Network* net);

/* Helper stats functions */

// These functions can be used to calculate specific statistics based on the network state.

int count_total_servers(const Network* net);

// Counts the number of servers in the network with a specific status (e.g., healthy, infected, protected, disabled).
int count_servers_by_status(const Network* net, int status);

// Calculates the percentage of servers with a specific status out of the total number of servers.

double calculate_percentage(int part, int total);

// Calculates the average security level of all servers in the network.
double calculate_average_security_level(const Network* net);


#endif
