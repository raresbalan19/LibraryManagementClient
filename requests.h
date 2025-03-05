#ifndef _REQUESTS_
#define _REQUESTS_

/* Calculeaza si returneaza un sir pentru cererea GET (query_params si cookies pot fi setate pe NULL daca nu sunt necesare) */
char *compute_get_request(char *host, char *url, char *query_params,
							char **cookies, int cookies_count, char *auth_token);

/* Calculeaza si returneaza un sir pentru cererea DELETE (query_params si cookies pot fi setate pe NULL daca nu sunt necesare) */
char *compute_delete_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count, char *auth_token);

/* Calculeaza și returneaza un șir pentru cererea POST (cookies poate fi NULL daca nu este necesar) */ 
char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
							int body_data_fields_count, char** cookies, int cookies_count, char *auth_token);

#endif
