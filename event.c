
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "event.h"
//createEvent
//insertEventSorted
//insertEventSorted
//popNextEvent
//freeEventList

Event* createEvent(double newTime, EventType newType, int newSourceServer,  int newTargetServer) {
    Event* newEvent = (Event*) malloc(sizeof(Event));
    assert(newEvent);
    newEvent->time = newTime;
    newEvent->type = newType;
    newEvent->sourceServer = newSourceServer;
    newEvent->targetServer = newTargetServer;
    newEvent->next = NULL;
    return newEvent;
}

Event* insertEventSorted(Event* newEvent, Event* head) {
    //The list is empty the event becomes the head
    if (head == NULL) {
        return newEvent;
    }

    if (newEvent->time < head->time) {
        newEvent->next = head;
        return newEvent;
    }
    Event* current = head;
    while (current->next != NULL && current->next->time < newEvent->time) {
       current = current->next;
    }
    newEvent->next = current->next;//החדש מושיט יד קדימה
    current->next = newEvent;//הנוכחי מקבל את החדש מהתחת שלו
    return head;
}
//current insert place  current->next


//פונקציה שתולשת את האיבר הראשון מהרשימה כלומר מוחקת אותו
//שומרת את האירוע הראשון כדי להחזיר אותו בסוף עם מצביע כפול אחרי שמחקנו אותו
Event* popNextEvent(Event** head) {
    Event* current = *head;
    *head = current->next;
    current->next = NULL;// כדי שמי שמקבל את האירוע לא ימשוך בטעות את התור יחד איתו
    return current;
}

/* =========================================================================
 * FUNCTION: popNextEvent
 * PURPOSE: Removes the first event from the Future Event List (FEL) and
 * returns it so the simulation engine can process it.
 * EXPLANATION:
 * - We use a double pointer (Event** head) so we can permanently update
 * the original list's head pointer in the main simulation loop.
 * - We temporarily save the first event, move the head pointer to the
 * next event, disconnect the saved event from the list, and return it.
 * ========================================================================= */


void freeEventList(Event* head) {
    Event* current;
    while (head != NULL) {
        current = head;
        head = head->next;
        free(current);
    }
}

// Prints detailed information about a single event, including its type, source server, target server, and time.
void print_event_info(const Event* event) {
    if (event == NULL) {
        return;
    }

    printf("Processing event: time %.2f, attacking server %d, attacked server %d\n",
        event->time,
        event->sourceServer,
        event->targetServer);
}
