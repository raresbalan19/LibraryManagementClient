#ifndef _HELPERS_
#define _HELPERS_

#define BUFLEN 4096
#define LINELEN 1000

// shows the current error
void error(const char *msg);

// adds a line to a string message
void compute_message(char *message, const char *line);

// opens a connection with server host_ip on port portno, returns a socket
int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag);

// closes a server connection on socket sockfd
void close_connection(int sockfd);

// send a message to a server
void send_to_server(int sockfd, char *message);

// receives and returns the message from a server
char *receive_from_server(int sockfd);

// extracts and returns a JSON from a server response
char *basic_extract_json_response(char *str);

char *basic_extract_json_response_plus(char *str);

char *extract_cookie_value(const char *response, const char *cookie_name);

char *extract_token_value(const char *json_str);

void parse_and_print_json(const char *json_string);

char *format_json(const char *json_input);

int contine_spatiu(const char *sir);

int contine_cifre(const char *sir);

int contine_doar_cifre(const char *sir);

int este_goala_sau_spatii(const char *sir);

int numara_spatii(const char *sir);

#endif
