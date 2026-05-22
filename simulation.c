#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#include "simulation.h"
#include "event.h"
#include "server.h"
#include "logger.h"


// פונקציות עזר פנימיות (סטטיות) שלא חשופות לקבצים אחרים
static double random_delay(void);
static int should_protect_server(void);

// =========================================================================
// פונקציה לאתחול הסימולציה: מכינה את הרשת להרצה הראשונית
// =========================================================================
void init_simulation(Network net, Event** fel_head) {
    ServerNode* current = net.head; 

    // שלב 1: מעבר על כל השרתים ברשת כדי להעניק חסינות התחלתית לחלקם
    while (current != NULL) { 
        // שרת 0 הוא "חולה אפס" ולכן לא יקבל הגנה. שאר השרתים מקבלים סיכוי סטטיסטי להיות מוגנים
        if (current->data.id != 0 && should_protect_server()) { 
            protect_server(&current->data);
        }
        current = current->next;
    }

    current = net.head;
    if (current == NULL) {
        return;
    }

    // שלב 2: הדבקה אוטומטית של שרת 0 (זמן 0.0) כדי להתחיל את שרשרת התקיפות
    infect_server(&current->data, 0.0);

    // שלב 3: יצירת אירועי התקיפה הראשונים
    for (int j = 0; j < net.numOfServers; j++) {
        // בודקים במטריצת הקשרים מי מחובר לשרת 0
        if (net.connections[0][j] == 1) {
            // חישוב זמן עתידי לתקיפה והכנסת האירוע ליומן (FEL) בצורה ממוינת
            double attack_time = 1.0 + j * 0.25;
            Event* new_attack = createEvent(attack_time, INFECTION_ATTEMPT, 0, j);
            *fel_head = insertEventSorted(new_attack, *fel_head);
        }
    }
}

// =========================================================================
// המוח של הסימולציה: מקבלת אירוע בודד מהיומן ומבצעת את הלוגיקה שלו
// =========================================================================
void process_event(Event* current_event, Network net, Event** fel_head) {
    // 1. סריקת הרשימה לאיתור השרת המותקף (Target)
    ServerNode* target = net.head;
    while (target != NULL && target->data.id != current_event->targetServer) {
        target = target->next;
    }

    // 2. סריקת הרשימה לאיתור השרת התוקף (Source)
    ServerNode* source = net.head;
    while (source != NULL && source->data.id != current_event->sourceServer) {
        source = source->next;
    }

    // הגנת קריסה: מוודאים ששני השרתים קיימים ברשת
    if (target == NULL || source == NULL) {
        return;
    }

    // המותקף סופג את המכה, ולכן המונה שסופר כמה פעמים הוא הותקף עולה
    target->data.attack_attempts++;

    // מנגנון הגנה: בודקים האם השרת המותקף חסין להדבקה
    if (!can_be_infected(&target->data)) {
        // אם תקפו אותו 3 פעמים והוא עדיין לא מודבק - משביתים אותו לחלוטין מפעילות
        if (target->data.attack_attempts >= 3 && target->data.status != SERVER_INFECTED) {
            disable_server(&target->data); 
        }
        return; // יוצרים מהאירוע כי ההדבקה נכשלה
    }

    // חומת אש אוטומטית לשרתים בעלי רמת אבטחה גבוהה (3 ומעלה)
    if (target->data.security_level >= 3) {
        protect_server(&target->data); 
        return;
    }

    // --- אם הגענו לשורה הזו, ההדבקה הצליחה! ---
    infect_server(&target->data, current_event->time);
    
    // התוקף (ולא המותקף) מקבל את הקרדיט על כך שיזם תקיפה מוצלחת
    source->data.successful_attacks++; 

    // השרת שהרגע הודבק הופך לתוקף בעצמו: סורקים את שכניו במטריצה ומייצרים אירועים עתידיים
    int my_id = target->data.id;
    for (int i = 0; i < net.numOfServers; i++) {
        if (net.connections[my_id][i] == 1) {
            // חישוב זמני התקיפה הבאים לפי רמת אבטחה ועיכוב אקראי
            double delay = 0.75 + (my_id + 1) * 0.13 + (i + 1) * 0.17 + target->data.security_level * 0.23;
            Event* new_attack = createEvent(current_event->time + random_delay(), INFECTION_ATTEMPT, my_id, i);
            
            // שימוש במצביע כפול כדי לעדכן את ראש הרשימה המקורי
            *fel_head = insertEventSorted(new_attack, *fel_head);
        }
    }
}

// =========================================================================
// מנוע הסימולציה הרגיל (מריץ הכל ברקע עד סיום הלוגיקה)
// =========================================================================
void run_simulation(Network net) {
    Event* fel_head = NULL; // יומן אירועים (FEL) מתחיל כריק
    init_simulation(net, &fel_head); 

    // לולאת ה-DES: כל עוד יש אירועים ביומן, ממשיכים לקפוץ בזמן
    while (fel_head != NULL) {
        // שולפים את האירוע הקרוב ביותר (הראשון בתור)
        Event* current_event = popNextEvent(&fel_head);
        
        // מעבירים את האירוע למוח של הסימולציה לעיבוד הלוגיקה
        process_event(current_event, net, &fel_head);
        
        // לאחר העיבוד, משחררים את האירוע מהזיכרון כדי למנוע דליפות
        free(current_event);
    }
    printf("Simulation Complete!\n");
}

// =========================================================================
// מנוע הסימולציה החי (מריץ עם השהיות והדפסות למסך למעקב בזמן אמת)
// =========================================================================
void run_simulation_live(Network net, int delay_ms) {
    srand((unsigned int)time(NULL));

    Event* fel_head = NULL;
    print_network_state(&net);
    Sleep(delay_ms); // השהייה כדי לאפשר למשתמש לקרוא את המסך
    
    init_simulation(net, &fel_head);

    printf("\nInitial state:\n");
    print_network_state(&net);
    Sleep(delay_ms);

    // לולאה זהה למנוע הרגיל, אך עם הדפסות של סטטוס הרשת בכל שלב
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

// פונקציית עזר לייצור עיכוב אקראי המדמה זמני תגובה מציאותיים של שרתים
static double random_delay(void) {
    return 0.50 + (rand() % 250) / 100.0;
}

// פונקציית עזר המגרילה סיכוי של 35% ששרת יקבל חסינות התחלתית
static int should_protect_server(void) {
    return rand() % 100 < 35;
}

