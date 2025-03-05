#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count, char *auth_token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    /* Scriu numele metodei, URL-ul si tipul protocolului */
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    /* Adaug gazda */
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    /* Adaug antetele necesare (Connection, Content-Type si Content-Length) */
    if (auth_token != NULL) {
        sprintf(line, "Authorization: Bearer %s", auth_token);
        compute_message(message, line);
    }

    /* Adaug cookie-uri */
    if (cookies != NULL) {
        strcat(message, "Cookie: ");
        for(int i = 0; i < cookies_count; i++) {
            strcat(message, cookies[i]);
            if (i != cookies_count - 1) {
                strcat(message, "; ");
            }
        }
        strcat(message, "\r\n");
    }

    /* Adaug o linie noua la sfarsitul antetului */
    compute_message(message, "");
    return message;
}

char *compute_delete_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count, char *auth_token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    /* Scriu numele metodei, URL-ul si tipul protocolului */
    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    /* Adaug gazda */
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    /* Adaug antetele necesare (Connection, Content-Type si Content-Length) */
    if (auth_token != NULL) {
        sprintf(line, "Authorization: Bearer %s", auth_token);
        compute_message(message, line);
    }

    /* Adaug cookie-uri */
    if (cookies != NULL) {
        strcat(message, "Cookie: ");
        for(int i = 0; i < cookies_count; i++) {
            strcat(message, cookies[i]);
            if (i != cookies_count - 1) {
                strcat(message, "; ");
            }
        }
        strcat(message, "\r\n");
    }
    /* Adaug o linie noua la sfarsitul antetului */
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char *content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count, char *auth_token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));
    char temp[LINELEN];

    /* Scriu numele metodei, URL-ul si tipul protocolului */
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    /* Adaug gazda */
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    /* Adaug antetele necesare (Connection, Content-Type si Content-Length) */
    if (auth_token != NULL) {
        sprintf(line, "Authorization: Bearer %s", auth_token);
        compute_message(message, line);
    }

    sprintf(line, "Connection: keep-alive");
    compute_message(message, line);

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    int len = 0;
    for (int i = 0; i < body_data_fields_count; i++) {
        len += strlen(body_data[i]);
        if (i < body_data_fields_count - 1) {
            len++; /* Pentru '&' */ 
        }
    }

    sprintf(line, "Content-Length: %d", len);
    compute_message(message, line);
    
    /* Adaug cookie-uri */
    if (cookies != NULL) {
        strcpy(temp, "Cookie: ");
        for (int i = 0; i < cookies_count; i++) {
            strcat(temp, cookies[i]);
            if (i < cookies_count - 1) {
                strcat(temp, "; ");
            }
        }
        compute_message(message, temp);
    }

    /* Adaug o linie noua la sfsrsitul antetului */
    compute_message(message, "");

    /* Adaug datele efective ale sarcinii */
    for (int i = 0; i < body_data_fields_count; i++) {
        strcat(body_data_buffer, body_data[i]);
        if (i < body_data_fields_count - 1) {
            strcat(body_data_buffer, "&");
        }
    }
    compute_message(message, body_data_buffer);

    free(line);
    free(body_data_buffer);
    return message;
}
