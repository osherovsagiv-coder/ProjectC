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

	int total_attack_attempts; // Total number of times any server was attacked (including failed attempts)
	int total_successful_attacks; // Total number of times a server was successfully infected (excluding protected or disabled servers)

    double first_infection_time;
    double last_infection_time;
} SimulationStats;

/*
  Creates an empty statistics object.
  All counters start from 0.
  Infection times start from -1.0, meaning "no infection yet".
 */
SimulationStats init_stats(void);

/*
  Reads the current network state and calculates statistics.
*/
SimulationStats calculate_stats(const Network* net);


#endif