
 #include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#include "simulation.h"
#include "event.h"
#include "server.h"
#include "logger.h"


// Helper function to generate a random delay between 0.50 and 2.99 seconds for live simulation
static double random_delay(void);

// Helper function to determine if a server should be protected based on its security level and attack attempts
static int should_protect_server(void);

void init_simulation(Network net, Event** fel_head) {
	ServerNode* current = net.head; // מציבים את המצביע על ההתחלה של השרת הראשון ברשימה

    while (current != NULL) { 
		if (current->data.id != 0 && should_protect_server()) { // אם השרת הנוכחי הוא לא השרת הראשון (כי אנחנו רוצים להתחיל את ההדבקה ממנו) ויש סיכוי שהוא יוגן אז אנחנו מגנים עליו כדי שיהיה לו סיכוי לא להידבק ולהמשיך לתקוף אחרים
            protect_server(&current->data);
        }

        current = current->next;
    }

    current = net.head;

    if (current == NULL) {
        return;
    }

    infect_server(&current->data, 0.0);

    for (int j = 0; j < net.numOfServers; j++) {
        if (net.connections[0][j] == 1) {
            double attack_time = 1.0 + j * 0.25;
			Event* new_attack = createEvent(attack_time, INFECTION_ATTEMPT, 0, j);//אם השרת הראשון מחובר לשכן שלו אז אנחנו יוצרים אירוע תקיפה ראשוני שמתחיל את הסימולציה ומכניסים אותו ליומן האירועים 
            *fel_head = insertEventSorted(new_attack, *fel_head);
        }
    }
}

void process_event(Event* current_event, Network net, Event** fel_head) {
    // 1. איתור המותקף
    ServerNode* target = net.head;
    while (target != NULL && target->data.id != current_event->targetServer) {
        target = target->next;
    }

    // 2. איתור התוקף
    ServerNode* source = net.head;
    while (source != NULL && source->data.id != current_event->sourceServer) {
        source = source->next;
    }

    if (target == NULL || source == NULL) {
        return;
    }

    // המותקף סופג את המכה, ולכן מונה "הותקפתי" שלו עולה
    target->data.attack_attempts++;

    // בדיקות סטטוס של המותקף (האם הוא מוגן או נתקף יותר מדי פעמים)
    if (!can_be_infected(&target->data)) {
        if (target->data.attack_attempts >= 3 && target->data.status != SERVER_INFECTED) {
            disable_server(&target->data); 
        }
        return;
    }

    if (target->data.security_level >= 3) {
        protect_server(&target->data); 
        return;
    }

    // --- ההדבקה הצליחה ---
    infect_server(&target->data, current_event->time);
    
    // התוקף יזם את ההדבקה המוצלחת, ולכן מקבל את הקרדיט
    source->data.successful_attacks++; 

    // השרת שהרגע הודבק (target) מתחיל לחפש שכנים לתקוף
    int my_id = target->data.id;
    for (int i = 0; i < net.numOfServers; i++) {
        if (net.connections[my_id][i] == 1) {
            double delay = 0.75 + (my_id + 1) * 0.13 + (i + 1) * 0.17 + target->data.security_level * 0.23;
            Event* new_attack = createEvent(current_event->time + random_delay(), INFECTION_ATTEMPT, my_id, i);
            *fel_head = insertEventSorted(new_attack, *fel_head);
        }
    }
}
void run_simulation(Network net) {//מקבלת את הרשת כולה מה-main
    Event* fel_head = NULL;//יומן אירועים ריק כדי לסמן שכרגע אין שום תקיפות
    init_simulation(net, &fel_head);// קריאה לפונקציה הראשונה בקובץ מעבירים לה את כל הרשת, ואת הכתובת של היומן הריק, הפונקציה ניגשת לשרת הראשון מדביקה אותו ומכניסה את התקיפות הראשונות ליומן
    while (fel_head != NULL) {//כל עוד היומן לא ריק כלומר כל עוד יש אירועים ביומן תרוץ
        Event* current_event = popNextEvent(&fel_head);//שולפים את האירוע הראשון התחילת היומן ושומרים בתוך משתנה מסוג אירוע
        process_event(current_event,net,&fel_head);//לוקחים את האירוע ששלפנו לפונקציה שבודקת את האירוע מחפשת איזה שרת נתקף, ואז בוחרת אם להדביק אותו ואם כן אז היא מייצרת עוד תקיפות חדשות שייכנסו ליומן אירועים.
        free(current_event);//טיפנו כבר באירוע הזה אז מנקים אותו בזיכרון כדי שלא יהיה שם זבל ואז הלולאה קופצת לאירוע הבא לשלוף אותו ולעשות בדיוק את אותם הפעולות הקודמות
    }
    printf("Simulation Complete!\n");//היומן ריק אין שום תקיפות לעשות והסימולציה הסתיימה בהצלחה
}
void run_simulation_live(Network net, int delay_ms) {
    srand((unsigned int)time(NULL));

    Event* fel_head = NULL;
    print_network_state(&net);
    Sleep(delay_ms);
	
    init_simulation(net, &fel_head);

    printf("\nInitial state:\n");
    print_network_state(&net);
    Sleep(delay_ms);

    while (fel_head != NULL) {
        Event* current_event = popNextEvent(&fel_head);

        print_event_info(current_event);
        process_event(current_event, net, &fel_head);
        print_network_state(&net);

        Sleep(delay_ms);

        free(current_event);
        current_event = NULL;
    }

    printf("Simulation Complete!\n");
}
static double random_delay(void) {
    return 0.50 + (rand() % 250) / 100.0;
}
static int should_protect_server(void) {
    return rand() % 100 < 35;
}
