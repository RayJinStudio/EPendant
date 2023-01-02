#ifndef APP_IRREMOTE_H
#define APP_IRREMOTE_H

#include "../../sys/interface.h"

extern APP_OBJ irremote_app;

typedef struct Category
{
    int id;
    char name[32];
};

void send();
void getToken();
void getCategories();

#endif