
#include <stdio.h>
#include <stdlib.h>
#include "simulation.h"
#include "event.h"




void init_simulation(Network net, Event** fel_head) {
    ServerNode* current = net.head;

    current->data.status = 1;//השרת הראשון הודבק
    current->data.infection_time = 0.0;//השרת הראשון הודבק בזמן 0.0

    for (int j = 0; j < net.numOfServers; j++) {
        if (net.connections[0][j] == 1) {//אם יש חיבור בין שרת 0 לשרת 1
            Event* new_attack = createEvent(1.0, INFECTION_ATTEMPT, 0, j);//יוצרים אירוע תקיפה חדש בזמן 1.0 והתוקף שרת 0 המותקף שרת j וההתקפה נשמרת בnew_attack
           *fel_head = insertEventSorted(new_attack, *fel_head);//לוקחים את התקיפה החדשה שיצרנו ומכניסים ליומן התקיפות בצורה מסודרת כדי שהתוכנית תדע לבצע בהמשך ואז הלולאה חוזרת כדי לבדוק את השרת הבא
        }
    }
}

void process_event(Event* current_event, Network net, Event** fel_head) {//הפונקציה מקבלת אירוע ספציפי שקורה עכשיו, את הראשת ואת היומן של הסימולציה.
    ServerNode*target = net.head;//מציבים את המצביע על ההתחלה של השרת הראשון ברשימה
    while (target != NULL && target->data.id != current_event->targetServer) {//כל עוד לא הגענו לסוף הרשימה וכל עוד הכתובת של השרת שמותקף באירוע שלנו לא שווה לשרת שבודקים עכשיו אותו.
        target = target->next;//מתקדמים אל השרת הבא בתור וככה סורקים את השרתים אחד אחד עד שמוצאים את השרת הנכון
    }
    if (target == NULL || target->data.status == 1) {//אם לא מצאנו את השרת בכלל (NULL), או שמצאנו אותו אבל הוא כבר מודבק (1) - צא החוצה ואל תעשה כלום
        return;//אם הגענו לסוף הרשימה בלי למצוא את השרת ואם מצאנו אותו אבל הוא כבר מודבק אז יוצאים בלי לעשות כלום מהפונקציה
    }
   target->data.status = 1;//אם לא יצאנו מהפונקציה סימן שמצאנו שרת נקי אז מדביקים אותו ומשנים אותו ל1 כדי לקבוע שהוא הודבק
   target->data.infection_time = current_event->time;//מסמנים את זמן ההדבקה לזמן של האירוע הנוכחי
    int my_id = target->data.id;//שומרים את הכתובת של השרת שהודבק במשתנה כדי שיהיה נוח להשתמש בו עוד רגע
    for (int i = 0; i < net.numOfServers; i++) {//רצים על כל העמודות במטריצה מ0 ועד כמות השרתים הכוללת וi ייצג את השכנים הפוטנציאלים
        if (net.connections[my_id][i] == 1) {//בודקים במטריצת הקשרים אץ השורה של השרת המותקף ואת העמודה הנוכחית ואם הערך הוא 1 שם אז זה אומר שהם מחוברים אחד לשני
            Event* new_attack = createEvent(current_event->time + 1.0, INFECTION_ATTEMPT, my_id, i);//מצאנו חיבור אז יוצרים אירוע תקיפה חדש, שומרים את התקיפה שם בתור אירוע חדש של התקיפה.
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