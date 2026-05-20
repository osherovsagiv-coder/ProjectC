#ifndef LOGGER_H
#define LOGGER_H

 
#include "network.h"
#include "event.h"
#include "stats.h"
 
/*
 * Writes the final simulation statistics into a text file.
 *
 * Parameters:
 * filename - name or path of the output file.
 * stats    - pointer to calculated simulation statistics.
 *
 * Returns:
 * 1 if the report was written successfully.
 * 0 if an error occurred.
 */

int write_stats_report(const char* filename, const SimulationStats* stats);

/*
 Opens a text file containing the simulation report and prints its contents to the console.
 */
int print_report_from_file(const char* filename);

 

#endif
