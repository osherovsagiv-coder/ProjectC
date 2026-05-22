#include <stdio.h>
#include <stdlib.h>

#include "server.h"

// פונקציה לאתחול הנתונים של שרת. מגדירה אותו כבריא, מאפסת את זמני ההדבקה ומוני התקיפות.
void init_server(ServerData* server, int id, int security_level) {
    if (!server) {
        fprintf(stderr, "Invalid server pointer\n");
        return;
    }

    server->id = id;
    server->status = SERVER_HEALTHY; // ברירת המחדל: כל שרת מתחיל כבריא
    server->security_level = security_level;
    server->infection_time = NOT_INFECTED;
    server->attack_attempts = 0;
    server->successful_attacks = 0;
}

// פונקציה המקצה זיכרון דינמי עבור שרת חדש וקוראת לפונקציית האתחול
ServerData* create_server(int id, int security_level) {
    ServerData* new_server = NULL;

    new_server = (ServerData*)malloc(sizeof(ServerData));

    if (!new_server) {
        fprintf(stderr, "Memory allocation failed for server %d\n", id);
        return NULL;
    }

    init_server(new_server, id, security_level);
    return new_server;
}

// פונקציית בקרה: בודקת אם מותר בכלל להדביק את השרת
int can_be_infected(const ServerData* server) {
    if (!server) {
        fprintf(stderr, "Invalid server pointer for infection check\n");
        return 0;
    }

    // שרת יכול להידבק אך ורק אם הוא במצב "בריא". 
    // אם הוא כבר מודבק, מוגן או מושבת - הוא חסין להדבקה נוספת.
    if (server->status != SERVER_HEALTHY) {
        return 0;
    }

    return 1;
}

// מחזירה אמת (1) אם השרת מודבק, או שקר (0) אחרת
int is_server_infected(const ServerData* server) {
    if (!server) {
        fprintf(stderr, "Invalid server pointer for infection status check\n");
        return 0;
    }

    return server->status == SERVER_INFECTED;
}

// פונקציה לביצוע ההדבקה בפועל: משנה סטטוס ושומרת את זמן התקיפה
void infect_server(ServerData* server, double time) {
    if (!server) {
        fprintf(stderr, "Invalid server pointer for infection\n");
        return;
    }

    if (can_be_infected(server)) {
        server->status = SERVER_INFECTED;
        server->infection_time = time;
    }
}

// פונקציית הגנה: משנה את הסטטוס ל"מוגן" (מעין הפעלת חומת אש / חסימה מונעת)
void protect_server(ServerData* server) {
    if (!server) {
        fprintf(stderr, "Invalid server pointer for protection\n");
        return;
    }

    if (server->status == SERVER_HEALTHY) {
        server->status = SERVER_PROTECTED;
    }
}

// פונקציית השבתה: מכבה את השרת לחלוטין במקרה של עומס תקיפות (מעל 3 ניסיונות)
void disable_server(ServerData* server) {
    if (!server) {
        fprintf(stderr, "Invalid server pointer for disabling\n");
        return;
    }

    server->status = SERVER_DISABLED;
}

// פונקציית עזר להדפסת נתוני השרת למסך למטרות דיבאגינג ומעקב
void print_server(const ServerData* server) {
    if (!server) {
        fprintf(stderr, "Invalid server pointer for printing\n");
        return;
    }

    printf("Server ID: %d\n", server->id);
    printf("Status: %s\n", status_to_string(server->status));
    printf("Security Level: %d\n", server->security_level);
    printf("Infection Time: %.2f\n", server->infection_time);
    printf("Attack Attempts: %d\n", server->attack_attempts);
    printf("Successful Attacks: %d\n", server->successful_attacks);
}

// מתרגמת את המספר של הסטטוס למילה קריאה באנגלית להדפסה
const char* status_to_string(int status) {
    switch (status) {
    case SERVER_HEALTHY:
        return "Healthy";
    case SERVER_INFECTED:
        return "Infected";
    case SERVER_PROTECTED:
        return "Protected";
    case SERVER_DISABLED:
        return "Disabled";
    default:
        return "Unknown";
    }
}
