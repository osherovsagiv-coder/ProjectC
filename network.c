#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "network.h"
#include "server.h"
 
// פונקציה שמקצה את המטריצה הדו-ממדית (מפת הקשרים בין השרתים)
int** init_network_matrix(int numOfServers) {
    // הקצאת המערך הראשי: מערך של מצביעים (כל מצביע ייצג שורה במטריצה)
    int** network_matrix = (int**)malloc(numOfServers * sizeof(int*));

    if (network_matrix == NULL) {
        return NULL;
    }

    // מעבר בלולאה כדי להקצות זיכרון לכל שורה בנפרד (העמודות)
    for (int i = 0; i < numOfServers; i++) {
        network_matrix[i] = (int*)calloc(numOfServers, sizeof(int));

        // מנגנון התאוששות: אם נגמר הזיכרון באמצע ההקצאה, רצים אחורה
        // ומשחררים בדיוק את השורות שכבר הספקנו להקצות כדי למנוע זליגת זיכרון
        if (network_matrix[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(network_matrix[j]);
            }

            free(network_matrix);
            return NULL;
        }
    }

    return network_matrix;
}


// פונקציה לבניית רשימה מקושרת של השרתים ברשת
ServerNode* create_network(int numOfServers) {
    ServerNode* head = NULL;
    ServerNode* tail = NULL;

    for (int i = 0; i < numOfServers; i++) {
        // יצירת חוליה חדשה (שרת חדש) בזיכרון
        ServerNode* newNode = (ServerNode*)malloc(sizeof(ServerNode));

        if (newNode == NULL) {
            free_network(head);
            return NULL;
        }

        // אתחול הנתונים של השרת (כמו ID וסטטוס) וניתוק המצביע הבא שלו
        init_server(&newNode->data, i, 0);
        newNode->next = NULL;

        // טיפול במקרה של השרת הראשון: הוא הופך להיות גם הראש וגם הזנב
        if (head == NULL) {
            head = newNode;
            tail = newNode;
        }
        // הוספת שרת חדש לסוף הרשימה ועדכון המצביע של הזנב
        else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    return head;
}


// פונקציה שאחראית לטעון את כל נתוני הרשת מתוך קובץ הטקסט
Network loadNetworkFromFile(const char* filename) {
    Network net;
    net.numOfServers = 0;
    net.head = NULL;
    net.connections = NULL;

    // פתיחת הקובץ למצב קריאה ("r" - Read)
    FILE* file = fopen(filename, "r");

    // אם הקובץ לא נמצא או פגום, מחזירים את הרשת הריקה כדי למנוע קריסה
    if (file == NULL) {
        printf("Error: Could not open the file %s\n", filename);
        return net;
    }

    // קריאת המספר הראשון בקובץ - כמות השרתים
    // הבדיקה (!= 1) מוודאת שבאמת הצלחנו לקרוא נתון אחד תקין
    if (fscanf(file, "%d", &net.numOfServers) != 1) {
        printf("Error: Failed to read number of servers from file %s\n", filename);
        fclose(file);
        return net;
    }

    // בניית הרשימה המקושרת בגודל המתאים
    net.head = create_network(net.numOfServers);

    // קריאת השורה השנייה בקובץ: רמות האבטחה של כל שרת
    ServerNode* current = net.head;
    for (int i = 0; i < net.numOfServers; i++) {
        // במידה ויש שגיאה בקריאה, סוגרים את הקובץ ומשחררים את הזיכרון שכבר הוקצה
        if (fscanf(file, "%d", &current->data.security_level) != 1) {
            printf("Error: Failed to read security_level for server %d\n", i);
            fclose(file);
            free_network_matrix(net.connections, net.numOfServers);
            free_network(net.head);
            net.connections = NULL;
            net.head = NULL;
            net.numOfServers = 0;
            return net;
        }
        current = current->next;
    }

    // בניית מטריצת הקשרים הריקה
    net.connections = init_network_matrix(net.numOfServers);

    // קריאת שאר הקובץ: מילוי המטריצה בערכים של 0 (אין חיבור) ו-1 (יש חיבור)
    // i מייצג את השרת התוקף, j מייצג את השרת המותקף
    for (int i = 0; i < net.numOfServers; i++) {          
        for (int j = 0; j < net.numOfServers; j++) {
            if (fscanf(file, "%d", &net.connections[i][j]) != 1) {
                printf("Error: Failed to read connection value at [%d][%d]\n", i, j);
                fclose(file);
                free_network_matrix(net.connections, net.numOfServers);
                free_network(net.head);
                net.connections = NULL;
                net.head = NULL;
                net.numOfServers = 0;
                return net;
            }
        }
    }

    // סיום העבודה מול הקובץ וסגירתו לשחרור משאבי מערכת ההפעלה
    fclose(file);

    return net;
}
