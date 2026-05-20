#include <stdio.h>

#include "logger.h"
#include "stats.h"
#include "server.h"

int write_stats_report(const char* filename, const SimulationStats* stats) {
    if (filename == NULL || stats == NULL) {
        return 0;
    }

    FILE* file = fopen(filename, "w");

    if (file == NULL) {
        return 0;
    }

    fprintf(file, "=== Simulation Statistics Report ===\n");
    fprintf(file, "Total servers: %d\n", stats->total_servers);
    fprintf(file, "Healthy servers: %d\n", stats->healthy_servers);
    fprintf(file, "Infected servers: %d\n", stats->infected_servers);
    fprintf(file, "Protected servers: %d\n", stats->protected_servers);
    fprintf(file, "Disabled servers: %d\n", stats->disabled_servers);
    fprintf(file, "Total attack attempts: %d\n", stats->total_attack_attempts);
    fprintf(file, "Total successful attacks: %d\n", stats->total_successful_attacks);

    if (stats->infected_servers > 0) {
        fprintf(file, "First infection time: %.2f\n", stats->first_infection_time);
        fprintf(file, "Last infection time: %.2f\n", stats->last_infection_time);
    }
    else {
        fprintf(file, "No servers were infected.\n");
    }

    fprintf(file, "====================================\n");

    fclose(file);
    file = NULL;

    return 1;
}
int print_report_from_file(const char* filename) {
    if (filename == NULL) {
        return 0;
    }

    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        return 0;
    }

    int ch = 0;

    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }

    fclose(file);
    file = NULL;

    return 1;
}
 
