#pragma once
#include "event.h"
#include "network.h"

#ifndef SIMULATION_H
#define SIMULATION_H

// 1. הפונקציה מקבלת את כל הרשת (Network).
void init_simulation(Network net, Event** fel_head);

// 2. הפונקציה לא מחזירה כלום (void), והיא חייבת לקבל גם את האירוע הספציפי שאנחנו מנתחים עכשיו.
void process_event(Event* current_event, Network net, Event** fel_head);

// 3. הפונקציה מריצה את הסימולציה על הרשת כולה.
void run_simulation(Network net);

// Runs the simulation in live mode with a step delay (in milliseconds)
void run_simulation_live(Network net, int step_delay_ms);

#endif

