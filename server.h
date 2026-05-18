//
// Created by Sagiv O. on 13/05/2026.
//
#pragma once
#ifndef PROJECTC_SERVER_H
#define PROJECTC_SERVER_H
typedef struct {
    int id;
    int status;
    int security_level;
    double infection_time;
    int attack_attempts;
    int successful_attacks;
}ServerData;

typedef struct ServerNode {
    ServerData data;
    struct ServerNode* next;
    struct ServerNode* prev;
}ServerNode;
ServerNode* create_network(int numOfServers);
#endif //PROJECTC_SERVER_H
