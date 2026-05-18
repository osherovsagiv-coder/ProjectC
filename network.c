
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
  int** network_matrix = (int**) malloc(numOfServers*sizeof(int*));
  for (int i = 0; i < numOfServers; i++) {
      network_matrix[i] = (int*) calloc(numOfServers , sizeof(int));
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
  fscanf(file, "%d", &net.numOfServers);

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
    fscanf(file, "%d", &current->data.security_level);
    current = current->next;
  }

  // 5. בניית לוח המשחק (מטריצת הקשרים) ואיפוסה
  net.connections = init_network_matrix(net.numOfServers);


  //פה מכניסים את הנתונים למטריצת קשרים בין השרתים שלנו
  // 6. מילוי המטריצה בנתוני הקשרים (שאר הקובץ - 0 ו-1)
  for (int i = 0; i < net.numOfServers; i++) {
    for (int j = 0; j < net.numOfServers; j++) {
      fscanf(file, "%d", &net.connections[i][j]);// & כדי להגיד למטריצה באיזה כתובת לשתול את מפת הקשרים בין השרתים שהיא 0 1 כלומר בינארי
    }
  }

  // 7. סגירת הקובץ והחזרת הרשת המוכנה
  // סיום: תמיד סוגרים את הקובץ בסוף העבודה כדי לשחרר את הנעילה של מערכת ההפעלה
  fclose(file);

  return net;
}

//לשאול את הAI אם צריך לפתוח ולהשתמש בקבצים ורק לבנות את הסימולציה עצמה בתור הפלט במסך ואז כשעושים את הסטטיסטיקה לכתוב את כל הסטטיסטיקה מסודר בקובץ.