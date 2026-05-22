#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "event.h"

// פונקציה ליצירת אירוע תקיפה חדש בזיכרון
Event* createEvent(double newTime, EventType newType, int newSourceServer,  int newTargetServer) {
    // הקצאת זיכרון דינמי עבור האירוע החדש
    Event* newEvent = (Event*) malloc(sizeof(Event));
    assert(newEvent); // מוודאים שההקצאה הצליחה כדי למנוע קריסה במקרה שאין זיכרון

    // אתחול השדות של האירוע בנתונים שקיבלנו (זמן, סוג, תוקף ומותקף)
    newEvent->time = newTime;
    newEvent->type = newType;
    newEvent->sourceServer = newSourceServer;
    newEvent->targetServer = newTargetServer;
    
    //NULL- האירוע החדש עדיין לא מחובר ליומן, אז המצביע הבא שלו מאותחל ל
    newEvent->next = NULL;
    
    return newEvent;
}

// (FFL)פונקציה להכנסת אירוע חדש ליומן האירועים  
//בצורה ממוינת כרונולוגית (לפי זמן)
Event* insertEventSorted(Event* newEvent, Event* head) {
    // מקרה קצה 1: אם היומן ריק לגמרי, האירוע החדש הופך להיות הראש של הרשימה
    if (head == NULL) {
        return newEvent;
    }

    // מקרה קצה 2: אם האירוע החדש קורה מוקדם יותר מהאירוע הראשון ביומן,
    // הוא צריך להידחף לתחילת הרשימה ולהפוך לראש החדש.
    if (newEvent->time < head->time) {
        newEvent->next = head;
        return newEvent;
    }

    // אם לא נכנסנו למקרי הקצה, מתחילים לסרוק את הרשימה כדי למצוא את המקום הנכון מבחינת זמנים
    Event* current = head;
    
    // מתקדמים ברשימה כל עוד לא הגענו לסוף, וכל עוד הזמן של האירוע הבא קטן מהזמן של האירוע החדש שלנו
    while (current->next != NULL && current->next->time < newEvent->time) {
       current = current->next;
    }
    
    // מצאנו את המקום! עכשיו "תופרים" את האירוע החדש לתוך השרשרת:
    // האירוע החדש מושיט יד ומצביע לאירוע שהיה אמור להיות הבא בתור
    newEvent->next = current->next;
    
    // והאירוע הנוכחי (שעצרנו עליו) מצביע עכשיו לאירוע החדש
    current->next = newEvent;
    
    // מחזירים את ראש הרשימה (שלא השתנה במקרה הזה)
    return head;
}

// פונקציה ששולפת ומנתקת את האירוע הבא מהיומן כדי שהסימולציה תוכל להריץ אותו.
// משתמשת במצביע כפול (Event** head) כדי לעדכן באופן קבוע את תחילת הרשימה האמיתית ב-main.
Event* popNextEvent(Event** head) {
    // שומרים את האירוע הראשון (זה שאנחנו עומדים לשלוף) במשתנה זמני
    Event* current = *head;
    
    // מקדמים את ראש הרשימה להצביע לאירוע הבא בתור ביומן
    *head = current->next;
    
    // ניתוק קריטי: מאפסים את הנקסט של האירוע ששלפנו כדי שהסימולציה תעבד אותו
    // כיחידה עצמאית, מבלי למשוך ולגרור בטעות את שאר התור יחד איתו לתוך הפונקציות.
    current->next = NULL;
    
    return current; // מחזירים לסימולציה את האירוע המנותק והמוכן לעיבוד
}

// פונקציה לניקוי ושחרור כל יומן האירועים מהזיכרון (בסיום התוכנית או במקרה של שגיאה)
void freeEventList(Event* head) {
    Event* current;
    
    // רצים על כל הרשימה: בכל איטרציה שומרים את האיבר הנוכחי, 
    // מקדמים את הראש לאיבר הבא, ואז משחררים את הנוכחי בבטחה.
    while (head != NULL) {
        current = head;
        head = head->next;
        free(current);
    }
}

// פונקציה להדפסת פרטי אירוע ספציפי למסך (זמן התקיפה, זהות התוקף וזהות המותקף)
void print_event_info(const Event* event) {
    if (event == NULL) {
        return;
    }

    printf("Processing event: time %.2f, attacking server %d, attacked server %d\n",
        event->time,
        event->sourceServer,
        event->targetServer);
}
