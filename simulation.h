#pragma once
#include "event.h"
#include "network.h"

#ifndef PROJECTC_SIMULATION_H
#define PROJECTC_SIMULATION_H

// 1. הפונקציה מקבלת את כל הרשת (Network).
void init_simulation(Network net, Event** fel_head);

// 2. הפונקציה לא מחזירה כלום (void), והיא חייבת לקבל גם את האירוע הספציפי שאנחנו מנתחים עכשיו.
void process_event(Event* current_event, Network net, Event** fel_head);

// 3. הפונקציה מריצה את הסימולציה על הרשת כולה.
void run_simulation(Network net);

#endif
