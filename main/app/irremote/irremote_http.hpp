#ifndef IRREMOTE_HTTP_HPP
#define IRREMOTE_HTTP_HPP

struct HttpPostEntity
{
    char *url;
    char *body;
    void (*postCb) (int, char*);
};

void initPostEntitry(HttpPostEntity* entity);
void setHttpPostURL(HttpPostEntity* entity, char* s);
void setHttpPostBody(HttpPostEntity* entity, char* s);
void setHttpPostCb(HttpPostEntity* entity, void (*cb) (int, char*));
void httpPostTask(void *pvParameters);

#endif