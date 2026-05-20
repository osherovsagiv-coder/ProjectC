#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "network.h"
#include "server.h"
 
/*
הלוגיקה של מטריצת הקשרים:
המטריצה הזו היא נטו מפת הדרכים של התולעת.
לדוגמא(בצורה ויזואלית)
שורה 0, עמודה 1 מייצגת את החיבור משרת 0 לשרת 1.

אם כתוב שם 1, יש כבל פיזי שמחבר ביניהם, והתולעת יכולה לתקוף מ-0 ל-1.

אם כתוב שם 0, אין חיבור. קיר חסום.
 */

//פונקציה שמקצה את המטריצה
//רשת (Grid) ריבועית שבה מספר השורות ומספר העמודות שווים שניהם לכמות השרתים.
int** init_network_matrix(int numOfServers) {
    int** network_matrix = (int**)malloc(numOfServers * sizeof(int*));

    if (network_matrix == NULL) {
        return NULL;
    }

    for (int i = 0; i < numOfServers; i++) {
        network_matrix[i] = (int*)calloc(numOfServers, sizeof(int));

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

//שחרור המטריצה מההקצאה הדינמית
void free_network_matrix(int** matrix, int numOfServers){
  for (int i = 0; i < numOfServers; i++) {
    free(matrix[i]);
  }
  free(matrix);
}
ServerNode* create_network(int numOfServers) {
    ServerNode* head = NULL;
    ServerNode* tail = NULL;

    for (int i = 0; i < numOfServers; i++) {
        ServerNode* newNode = (ServerNode*)malloc(sizeof(ServerNode));

        if (newNode == NULL) {
            free_network(head);
            return NULL;
        }

        init_server(&newNode->data, i, 0);
        newNode->next = NULL;

        if (head == NULL) {
            head = newNode;
            tail = newNode;
        }
        else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    return head;
}

void free_network(ServerNode* head) {
    ServerNode* current = head;

    while (current != NULL) {
        ServerNode* next = current->next;
        free(current);
        current = next;
    }
}

//פונקציה שהAI עצמו כתב לי כי אני לא ידעתי את זה:
//זה קשור לספרייה של stdio.h שמתשתמשים בפונקציה ממנה...
Network loadNetworkFromFile(const char* filename) {
  // שלב מקדים: יוצרים "קופסה" של רשת ריקה שאותה נמלא ונחזיר בסוף

  Network net;
  net.numOfServers = 0;
  net.head = NULL;
  net.connections = NULL;

  // 1. פותחים את "הספר" (הקובץ) למצב קריאה
  // ."r" עומד על Read המשתנה .file שומר את הכתובת של הקובץ הפתוח (כמו סימניה)
  FILE* file = fopen(filename, "r");

  // בדיקת בטיחות קריטית: מוודאים שהקובץ באמת קיים ונפתח בהצלחה
  // אם fopen נכשל (למשל כי השם שגוי או הקובץ חסר), הוא יחזיר NULL
  if (file == NULL) {
    printf("Error: Could not open the file %s\n", filename);
    return net; // מחזירים את הרשת הריקה ויוצאים, כדי שהתוכנית לא תקרוס
  }

  // ==========================================
  // כאן יבואו השלבים של קריאת הנתונים מהקובץ
  // (נקרא את כמות השרתים, ואז את המטריצה)
  // ==========================================
  // 2. קריאת כמות השרתים (המספר הראשון בקובץ)
  if (fscanf(file, "%d", &net.numOfServers) != 1) {
      printf("Error: Failed to read number of servers from file %s\n", filename);
      fclose(file);
      return net;
  }

  //3. יצירת הרשת (הרשימה המקושרת) בעזרת הפונקציה של השותפה
  net.head = create_network(net.numOfServers);
  /*
  Think of fscanf as a "claw machine" in an arcade, which has 3 roles:

1. file (where to pull from): Which box does the crane go into? (our open file).

2. "%d" (what to pull): What kind of object are we looking for? Here we ask the crane to pull an integer (d = Decimal).

3. &net.numOfServers (where to drop): Once the crane has grabbed the first number it sees (say 5), what box exactly does it drop it into? The & symbol says "take the address of the numOfServers box inside the net structure, and throw the number there."

   */



  //4. מילוי רמות האבטחה לכל שרת (השורה השנייה בקובץ)
  ServerNode* current = net.head;
  for (int i = 0; i < net.numOfServers; i++) {
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

  // 5. בניית לוח המשחק (מטריצת הקשרים) ואיפוסה
  net.connections = init_network_matrix(net.numOfServers);


  //פה מכניסים את הנתונים למטריצת קשרים בין השרתים שלנו
  // 6. מילוי המטריצה בנתוני הקשרים (שאר הקובץ - 0 ו-1)
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
      }// & כדי להגיד למטריצה באיזה כתובת לשתול את מפת הקשרים בין השרתים שהיא 0 1 כלומר בינארי
    }
  }

  // 7. סגירת הקובץ והחזרת הרשת המוכנה
  // סיום: תמיד סוגרים את הקובץ בסוף העבודה כדי לשחרר את הנעילה של מערכת ההפעלה
  fclose(file);

  return net;
}
void print_network_state(const Network* net) {
    if (net == NULL) {
        return;
    }

    ServerNode* current = net->head;

    printf("\nCurrent network state:\n");

    while (current != NULL) {
        printf("Server %d: %s, infection time: %.2f, attempts: %d, successful: %d\n",
            current->data.id,
            status_to_string(current->data.status),
            current->data.infection_time,
            current->data.attack_attempts,
            current->data.successful_attacks);

        current = current->next;
    }

    printf("\n");
}


//לשאול את הAI אם צריך לפתוח ולהשתמש בקבצים ורק לבנות את הסימולציה עצמה בתור הפלט במסך ואז כשעושים את הסטטיסטיקה לכתוב את כל הסטטיסטיקה מסודר בקובץ.
 
 

 
