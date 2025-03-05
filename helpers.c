#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "buffer.h"
#include "parson.h"

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void compute_message(char *message, const char *line)
{
    strcat(message, line);
    strcat(message, "\r\n");
}

int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag)
{
    struct sockaddr_in serv_addr;
    int sockfd = socket(ip_type, socket_type, flag);
    if (sockfd < 0)
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = ip_type;
    serv_addr.sin_port = htons(portno);
    inet_aton(host_ip, &serv_addr.sin_addr);

    /* connect the socket */
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return sockfd;
}

void close_connection(int sockfd)
{
    close(sockfd);
}

void send_to_server(int sockfd, char *message)
{
    int bytes, sent = 0;
    int total = strlen(message);

    do
    {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0) {
            error("ERROR writing message to socket");
        }

        if (bytes == 0) {
            break;
        }

        sent += bytes;
    } while (sent < total);
}

char *receive_from_server(int sockfd)
{
    char response[BUFLEN];
    buffer buffer = buffer_init();
    int header_end = 0;
    int content_length = 0;

    do {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0){
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
        
        header_end = buffer_find(&buffer, HEADER_TERMINATOR, HEADER_TERMINATOR_SIZE);

        if (header_end >= 0) {
            header_end += HEADER_TERMINATOR_SIZE;
            
            int content_length_start = buffer_find_insensitive(&buffer, CONTENT_LENGTH, CONTENT_LENGTH_SIZE);
            
            if (content_length_start < 0) {
                continue;           
            }

            content_length_start += CONTENT_LENGTH_SIZE;
            content_length = strtol(buffer.data + content_length_start, NULL, 10);
            break;
        }
    } while (1);
    size_t total = content_length + (size_t) header_end;
    
    while (buffer.size < total) {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
    }
    buffer_add(&buffer, "", 1);
    return buffer.data;
}

char *basic_extract_json_response(char *str)
{
    return strstr(str, "{\"");
}

char *basic_extract_json_response_plus(char *str) {
    return strstr(str, "[{\"");
}

char *extract_cookie_value(const char *response, const char *cookie_name) {
	char *start, *end, *value = NULL;
	/* Caut unde incepe "connect.sid=" */
	start = strstr(response, cookie_name);
	if (start) {
		/* Calculez lungimea totala pana la primul ';' pentru a include totul de la "connect.sid=" inainte */
		if ((end = strchr(start, ';')) != NULL) {
			int len = end - start;
			value = (char *) malloc(len + 1);
			if (value) {
                /* Copiez intreaga sectiune incluzand "connect.sid=" */
				strncpy(value, start, len);
				value[len] = '\0';
			}
		}
	}

	return value;
}

char *extract_token_value(const char *json_str) {
	const char *token_key = "\"token\":\"";
	const char *end_char = "\"}";

	/* Gasesc inceputul valorii tokenului */
	char *start = strstr(json_str, token_key);
	if (!start) {
		return NULL;
	}
    /* Mut pointerul de start dupa cheia tokenului */
	start += strlen(token_key);

	/* Gasesc sfarsitul valorii tokenului */
	char *end = strstr(start, end_char);
	if (!end) {
		return NULL;
	}

	/* Calculez lungimea valorii tokenului */
	int token_len = end - start;
	char *token = malloc(token_len + 1);
	if (!token) {
		return NULL;
	}

	strncpy(token, start, token_len);
	token[token_len] = '\0';

	return token;
}

void parse_and_print_json(const char *json_string) {

	JSON_Value *root_value = json_parse_string(json_string);
	JSON_Array *books = json_value_get_array(root_value);

	/* Afisez paranteza patrata de inceput */
	printf("[\n");

	size_t i;
	for (i = 0; i < json_array_get_count(books); i++) {
		JSON_Object *book = json_array_get_object(books, i);
		int id = (int) json_object_get_number(book, "id");
		const char *title = json_object_get_string(book, "title");

		/* Afisez informatiile */
		printf("  {\n    \"id\": %d,\n    \"title\": \"%s\"\n  }", id, title);

		/* Adaug virgula intre elemente, dar nu dupa ultimul element */
		if (i < json_array_get_count(books) - 1) {
			printf(",\n");
		} else {
			printf("\n");
		}
	}

	/* Afisez paranteza patrata de final */
	printf("]\n");

	json_value_free(root_value);
}

char *format_json(const char *json_input) {
	JSON_Value *root_value = json_parse_string(json_input);
	if (root_value == NULL) {
        /* Returnez NULL daca parse-ul esueaza */
		return NULL;
	}

	/* Serializez inapoi JSON-ul intr-un format frumos */
	char *formatted_json = json_serialize_to_string_pretty(root_value);

	json_value_free(root_value);

	return formatted_json;
}

int contine_spatiu(const char *sir) {
	while (*sir) {
		if (*sir == ' ') {
			return 0;
		}
		sir++;
	}
	return 1;
}

int contine_cifre(const char *sir) {
	while (*sir) {
		if (isdigit((unsigned char) *sir)) {
			return 1;
		}
		sir++;
	}
	return 0;
}

int contine_doar_cifre(const char *sir) {
	if (*sir == '\0') {
		return 0;
	}
	while (*sir) {
		if (!isdigit((unsigned char) *sir)) {
			return 0;
		}
		sir++;
	}
	return 1;
}

int este_goala_sau_spatii(const char *sir) {
    /* Presupun ca linia este doar spatii sau newline */
	int doarSpatii = 1;
	for (int i = 0; i < sir[i]; i++) {
		if (!isspace((unsigned char) sir[i])) {
            /* Daca gasesc un caracter non-spatiu sau newline, setez la 0 */
			doarSpatii = 0;
			break;
		}
	}
	return doarSpatii;
}

int numara_spatii(const char *sir) {
	int count = 0;
	const char *start = sir;

	while (*sir) {
		if (*sir == ' ') {
			count++;
		}
		sir++;
	}

	/* Verific daca numarul de spatii este egal cu lungimea totala a sirului */
	if (count == (sir - start)) {
		return 1;
	}
	return 0;
}
