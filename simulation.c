
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

void process_event(Event* current_event, Network net, Event** fel_head) {//הפונקציה מקבלת אירוע ספציפי שקורה עכשיו, את הראשת ואת היומן של הסימולציה.
    ServerNode*target = net.head;//מציבים את המצביע על ההתחלה של השרת הראשון ברשימה
    while (target != NULL && target->data.id != current_event->targetServer) {//כל עוד לא הגענו לסוף הרשימה וכל עוד הכתובת של השרת שמותקף באירוע שלנו לא שווה לשרת שבודקים עכשיו אותו.
        target = target->next;//מתקדמים אל השרת הבא בתור וככה סורקים את השרתים אחד אחד עד שמוצאים את השרת הנכון
    }
    if (target == NULL) {
        return;
    }

	target->data.attack_attempts++; //מעלים את מספר התקיפות שנעשו על השרת הזה ב1 כי הגענו אליו דרך אירוע תקיפה אז זה אומר שמישהו ניסה לתקוף אותו

    if (!can_be_infected(&target->data)) {
        if (target->data.attack_attempts >= 3 && target->data.status != SERVER_INFECTED) {
			disable_server(&target->data); //אם השרת לא יכול להיות מודבק (למשל כי הוא כבר מוגן או כבר נתקף) אז אנחנו בודקים אם הוא עבר את סף התקיפות המותרות (3) ואם כן אז אנחנו משביתים אותו כי זה אומר שהוא נתקף יותר מדי פעמים ולא הצליח להדביק אותו אז הוא לא יכול להמשיך לתקוף אחרים ולכן הוא צריך להיות מושבת
        }

        return;
    }

    if (target->data.security_level >= 3) {
		protect_server(&target->data); //אם השרת יכול להיות מודבק אבל רמת האבטחה שלו גבוהה מדי (3 ומעלה) אז אנחנו פשוט מגנים עליו כדי שהוא לא יוכל להדבק ולא יוכל להמשיך לתקוף אחרים
        return;
    }

    infect_server(&target->data, current_event->time);
	target->data.successful_attacks++;// אם הגענו עד הלום זה אומר שהשרת נתקף בהצלחה אז אנחנו מעלים את מספר התקיפות המוצלחות שלו ב1 
    int my_id = target->data.id;//שומרים את הכתובת של השרת שהודבק במשתנה כדי שיהיה נוח להשתמש בו עוד רגע
    for (int i = 0; i < net.numOfServers; i++) {//רצים על כל העמודות במטריצה מ0 ועד כמות השרתים הכוללת וi ייצג את השכנים הפוטנציאלים
        if (net.connections[my_id][i] == 1) {//בודקים במטריצת הקשרים אץ השורה של השרת המותקף ואת העמודה הנוכחית ואם הערך הוא 1 שם אז זה אומר שהם מחוברים אחד לשני
			double delay = 0.75 + (my_id + 1) * 0.13 + (i + 1) * 0.17 + target->data.security_level * 0.23; //אם הם מחוברים אז אנחנו מחשבים את הזמן שבו השרת שהודבק עכשיו ינסה לתקוף את השכן שלו לפי הנוסחה הנתונה
            Event* new_attack = createEvent(current_event->time + random_delay(), INFECTION_ATTEMPT, my_id, i);
            *fel_head = insertEventSorted(new_attack, *fel_head);//מכניסים את התקיפה החדשה ליומן האירועים ואז הלולאה ממשיכה לחפש שכנים עד שנגמר השורות במטריצה
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
