 #include <stdio.h>

#include "network.h"
#include "simulation.h"
#include "stats.h"
#include "logger.h"

#define STEP_DELAY_MS 1000

int main(void) {
    const char* network_file = "C:\\Users\\Elina\\source\\repos\\Worm Propagation Simulation Final\\x64\\Debug\\network.txt";
    const char* report_file = "stats_report.txt";

    Network net = loadNetworkFromFile(network_file);

    if (net.numOfServers <= 0 || net.head == NULL || net.connections == NULL) {
        printf("Failed to load network from file: %s\n", network_file);
        return 1;
    }

    run_simulation_live(net, STEP_DELAY_MS);

    SimulationStats stats = calculate_stats(&net);

    if (write_stats_report(report_file, &stats)) {
        print_report_from_file(report_file);
    }

    free_network_matrix(net.connections, net.numOfServers);
    free_network(net.head);

    return 0;
}
