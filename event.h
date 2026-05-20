//
// Created by Sagiv O. on 12/05/2026.
//
#pragma once
#ifndef PROJECTC_EVENT_H
#define PROJECTC_EVENT_H
/* * EventType Enum:
 * Categorizes the event. The simulation loop will read this type
 * to decide which action (Event Handler) to execute.
 * Currently, we only have one action: a worm trying to infect a server.
 *
 */
typedef enum {
    INFECTION_ATTEMPT
}EventType;

/*
בשפת C, ברגע שהשתמשנו ב-enum, אנחנו פשוט נותנים שמות קריאים למספרים מאחורי הקלעים.
במקרה הזה, המחשב רואה את המילה INFECTION_ATTEMPT ומתייחס אליה בתור המספר 0.
למה עשינו את זה? כדי שלא נצטרך לכתוב בקוד שלנו משהו כמו if (type == 0).
הרבה יותר קל למוח האנושי לקרוא if (type == INFECTION_ATTEMPT).
זה פשוט "תגית" או תווית שאומרת לנו: "האירוע הזה הוא ניסיון תקיפה".



* =========================================================================
* FILE: event.h
* PURPOSE: Defines the core "Events" for the Discrete-Event Simulation (DES).
* * In a DES(Data Encryption Standard), time doesn't run continuously. Instead, the simulation jumps
* from one discrete event to the next based on a timeline.
* This file defines what an "Event" looks like in our cyber worm scenario.
* ========================================================================= */

typedef struct Event {
    double time;           /* The exact simulation time this event occurs */
    EventType type;        /* What kind of event is this? (e.g., INFECTION_ATTEMPT) */
    int sourceServer;      /* ID of the infected server launching the attack */
    int targetServer;      /* ID of the neighboring server being attacked */
    struct Event *next;    /* Pointer to the next event in the Future Event List (FEL) */
} Event;



Event* createEvent(double newTime, EventType newType, int newSourceServer, int newTargetServer);
Event* insertEventSorted(Event* newEvent, Event* head);
Event* popNextEvent(Event** head);
void freeEventList(Event* head);
void print_event_info(const Event* event);

#endif //PROJECTC_EVENT_H

