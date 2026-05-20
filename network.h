//
// Created by Sagiv O. on 13/05/2026.
//
#pragma once
#ifndef PROJECTC_NETWORK_H
#define PROJECTC_NETWORK_H
#include "server.h"

typedef struct ServerNode {
    ServerData data;
    struct ServerNode* next;
} ServerNode;

typedef struct Network {
 int numOfServers;
 int** connections;//מפת הקשרים תהיה מטריצה דינמית לכן מצביע כפול
 ServerNode* head;
}Network;

int** init_network_matrix(int numOfServers);
void free_network_matrix(int** matrix, int numOfServers);

ServerNode* create_network(int numOfServers);
void free_network(ServerNode* head);

Network loadNetworkFromFile(const char* filename);

void print_network_state(const Network* net);

#endif //PROJECTC_NETWORK_H

/*

למה אנחנו צריכים אותו?
לפני שאנחנו מריצים את מנוע הסימולציה, אנחנו צריכים שיהיה לנו "לוח משחק". הקובץ הזה יגדיר את מבנה הרשת הכללי.
תחשוב על הקובץ הזה בתור "מפת השולחן" שעליה אנחנו מניחים את זירת הקרב.
המטרה שלו היא להגדיר מבנה אחד כולל (Struct) בשם Network, שישמש את מנוע הסימולציה שלך כדי לדעת מה קורה ברשת.
מה אנחנו צריכים להכניס לתוך ה-Network?
מנוע הסימולציה שלך לא צריך הרבה בשביל לנהל את הרשת. הוא צריך רק שני נתונים בסיסיים שישבו בתוך המבנה הזה:

כמות השרתים הכוללת: כדי שנדע מה גודל הרשת שאנחנו תוקפים (למשל, מספר שלם).

 הגישה לשרתים עצמם: כאן נכנסת נקודת החיבור היחידה לקוד של השותפה שלך. לינה יצרה את הרשת בתור שרשרת (רשימה מקושרת מעגלית).
כל חוליה בשרשרת הזאת מייצגת שרת, והיא קראה לסוג החוליה הזה ServerNode.
כדי של-Network שלך תהיה גישה לכל השרתים, הוא פשוט צריך להחזיק מצביע (Pointer) שיצביע לחוליה הראשונה (ה-head) בשרשרת שהיא יצרה.
זהו, מעבר לזה אתה לא צריך לדעת איך השרשרת שלה בנויה מבפנים!

1 2 3 3 4 5
0 0 0 1 0 1
0 1 0 0 0 2
0 0 0  0 3
 */


