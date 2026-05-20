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

// Counts the total number of attack attempts across all servers in the network.
int count_total_attack_attempts(const Network* net);

// Counts the total number of successful attacks (infections) across all servers in the network.
int count_total_successful_attacks(const Network* net);

// Finds the time of the first infection in the network. Returns NOT_INFECTED if no infections occurred.
double find_first_infection_time(const Network* net);

// Finds the time of the last infection in the network. Returns NOT_INFECTED if no infections occurred.
double find_last_infection_time(const Network* net);

// A generic function to find an infection time (first or last) based on a provided comparison function.
double find_infection_time(
    const Network* net,
    int (*should_update)(double current_time, double saved_time)
);

// Comparison functions for infection times.
int is_earlier_infection_time(double current_time, double saved_time);

// Comparison function to check if the current infection time is later than the saved time.
int is_later_infection_time(double current_time, double saved_time);

// These functions can be used to get specific statistics for an individual server.
int get_attack_attempts(const ServerData* server);

// Gets the number of successful attacks (infections) for a specific server.
int get_successful_attacks(const ServerData* server);

// A generic function to sum a specific field across all servers in the network using a provided getter function.
int sum_server_field(
    const Network* net,
    int (*get_value)(const ServerData* server)
);
 


#endif
