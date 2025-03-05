#include <stdio.h>				/* printf, sprintf */
#include <stdlib.h>				/* exit, atoi, malloc, free */
#include <unistd.h>				/* read, write, close */
#include <string.h>				/* memcpy, memset */
#include <sys/socket.h>			/* socket, connect */
#include <netinet/in.h>			/* struct sockaddr_in, struct sockaddr */
#include <netdb.h>				/* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <ctype.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

#define SERVER "34.246.184.49:8080"
#define SERVER_IP "34.246.184.49"
#define SERVER_PORT 8080
#define SERVER_API_REGISTER "/api/v1/tema/auth/register"
#define SERVER_API_LOGIN "/api/v1/tema/auth/login"
#define SERVER_API_ENTER_LIBRARY "/api/v1/tema/library/access"
#define SERVER_API_GET_BOOKS "/api/v1/tema/library/books"
#define SERVER_API_GET_BOOK "/api/v1/tema/library/books/"
#define SERVER_API_ADD_BOOK "/api/v1/tema/library/books"
#define SERVER_API_DELETE_BOOK "/api/v1/tema/library/books/"
#define SERVER_API_LOGOUT "/api/v1/tema/auth/logout"

int main(int argc, char *argv[]) {

	char *cookies[1];
	cookies[0] = NULL;
	char *auth_token = NULL;

	while (1) {
		char command[100];
		fgets(command, 100, stdin);

		if (strcmp(command, "register\n") == 0) {
			char *message = NULL, *response = NULL;
			int sockfd;

			/* Verific daca un utilizator este deja logat */
			if (cookies[0] == NULL) {
				sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
				if (sockfd < 0) {
					printf("EROARE : Eroare la socket!\n");
				}

				char **data = malloc(sizeof (char *));
				data[0] = malloc(512);

				char username[100], password[100];

				printf("username=");
				fgets(username, sizeof(username), stdin);
				username[strcspn(username, "\n")] = '\0';

				if (numara_spatii(username) != 1 && contine_spatiu(username) == 1) {
					
					printf("password=");
					fgets(password, sizeof(password), stdin);
					password[strcspn(password, "\n")] = '\0';

					if (numara_spatii(password) != 1 && contine_spatiu(password) == 1) {

						snprintf(data[0], 512, "{\"username\": \"%s\", \"password\": \"%s\"}", username, password);
						message =
							compute_post_request(SERVER, SERVER_API_REGISTER, "application/json", data, 1, NULL, 0,
												 NULL);
						send_to_server(sockfd, message);
						response = receive_from_server(sockfd);

						if (strstr(response, "400 Bad Request") && strstr(response, "201 Created") == NULL) {
							printf("EROARE : Utilizatorul este deja înregistrat!\n");
						} else {
							printf("SUCCES : Utilizator înregistrat cu succes!\n");
						}

						free(data[0]);
						free(data);
						close_connection(sockfd);
					} else {
						printf("EROARE : Parola nu poate conține spații la înregistrare!\n");
					}
				} else {
					printf("EROARE : Numele de utilizator nu poate conține spații la înregistrare!\n");
				}
			} else {
				printf("EROARE : Un utilizator este deja logat!\n");
			}
		}

		if (strcmp(command, "login\n") == 0) {
			char *message = NULL, *response = NULL;
			int sockfd;

			/* Verific daca un utilizator este deja logat */
			if (cookies[0] == NULL) {
				sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
				if (sockfd < 0) {
					printf("EROARE : Eroare la socket!\n");
				}

				char **data = malloc(sizeof (char *));
				data[0] = malloc(512);

				char username[100], password[100];

				printf("username=");
				fgets(username, sizeof(username), stdin);
				username[strcspn(username, "\n")] = '\0';

				if (numara_spatii(username) != 1 && contine_spatiu(username) == 1) {

					printf("password=");
					fgets(password, sizeof(password), stdin);
					password[strcspn(password, "\n")] = '\0';

					if (numara_spatii(password) != 1 && contine_spatiu(password) == 1) {

						snprintf(data[0], 512, "{\"username\": \"%s\", \"password\": \"%s\"}", username, password);
						message =
							compute_post_request(SERVER, SERVER_API_LOGIN, "application/json", data, 1, NULL, 0, NULL);
						send_to_server(sockfd, message);
						response = receive_from_server(sockfd);

						if (strstr(response, "400 Bad Request") && strstr(response, "200 OK") == NULL) {
							printf("EROARE : Eroare la utilizator încercând să se logheze!\n");
						} else {
							printf("SUCCES : Utilizatorul a fost logat cu succes!\n");
							cookies[0] = extract_cookie_value(response, "connect.sid");
						}

						free(data[0]);
						free(data);
						close_connection(sockfd);
					} else {
						printf("EROARE : Parola nu poate conține spații la logare!\n");
					}
				} else {
					printf("EROARE : Numele de utilizator nu poate conține spații la logare!\n");
				}
			} else {
				printf("EROARE : Un utilizator este deja logat!\n");
			}
		}

		if (strcmp(command, "enter_library\n") == 0) {
			if (cookies[0]) {
				char *message = NULL, *response;
				int sockfd;

				sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
				if (sockfd < 0) {
					printf("EROARE : Eroare la socket!\n");
				}

				message = compute_get_request(SERVER, SERVER_API_ENTER_LIBRARY, NULL, cookies, 1, NULL);
				send_to_server(sockfd, message);
				response = receive_from_server(sockfd);

				auth_token = basic_extract_json_response(response);
				auth_token = extract_token_value(auth_token);

				printf("SUCCES : Utilizatorul are acces la bibliotecă!\n");

				close_connection(sockfd);
			} else {
				printf("EROARE : Utilizatorul nu are acces la bibliotecă!\n");
			}
		}

		if (strcmp(command, "get_books\n") == 0) {
			if (auth_token != NULL) {
				char *message = NULL, *response = NULL;
				int sockfd;

				sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
				if (sockfd < 0) {
					printf("EROARE : Eroare la socket!\n");
				}

				message = compute_get_request(SERVER, SERVER_API_GET_BOOKS, NULL, cookies, 1, auth_token);
				send_to_server(sockfd, message);
				response = receive_from_server(sockfd);
				response = basic_extract_json_response_plus(response);

				parse_and_print_json(response);

				close_connection(sockfd);

			} else {
				printf("EROARE : Nu ai token pentru get_books!\n");
			}
		}

		if (strcmp(command, "get_book\n") == 0) {
			if (auth_token != NULL) {
				char *message = NULL, *response = NULL;
				int sockfd, book_id;
				char book_url[256];

				printf("id=");
				scanf("%d", &book_id);

				sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
				if (sockfd < 0) {
					printf("EROARE : Eroare la socket!\n");
				}

				sprintf(book_url, "%s%d", SERVER_API_GET_BOOK, book_id);

				message = compute_get_request(SERVER, book_url, NULL, cookies, 1, auth_token);
				send_to_server(sockfd, message);
				response = receive_from_server(sockfd);
				if (strstr(response, "404 Not Found")) {
					printf("EROARE : Id-ul pentru care efectuaţi cererea este invalid!\n");
				} else {
					response = basic_extract_json_response(response);
					response = format_json(response);
					printf("%s\n", response);

				}

				close_connection(sockfd);

			} else {
				printf("EROARE : Nu ai token pentru get_book!\n");
			}
		}

		if (strcmp(command, "add_book\n") == 0) {
			if (auth_token != NULL) {
				char *message = NULL, *response = NULL;
				int sockfd;

				char **data = malloc(sizeof(char *));
				data[0] = malloc(1000);

				char title[100], author[100], genre[100], page_count[100], publisher[100];

				printf("title=");
				fgets(title, sizeof(title), stdin);
				title[strcspn(title, "\n")] = '\0';

				if (numara_spatii(title) != 1) {

					printf("author=");
					fgets(author, sizeof(author), stdin);
					author[strcspn(author, "\n")] = '\0';

					if (numara_spatii(author) != 1) {

						if (contine_cifre(author) == 0) {

							printf("genre=");
							fgets(genre, sizeof(genre), stdin);
							genre[strcspn(genre, "\n")] = '\0';

							if (numara_spatii(genre) != 1) {

								if (contine_cifre(genre) == 0) {

									printf("publisher=");
									fgets(publisher, sizeof(publisher), stdin);
									publisher[strcspn(publisher, "\n")] = '\0';

									if (numara_spatii(publisher) != 1) {

										if (contine_cifre(publisher) == 0) {

											printf("page_count=");
											fgets(page_count, sizeof(page_count), stdin);
											page_count[strcspn(page_count, "\n")] = '\0';

											if (numara_spatii(page_count) != 1) {

												if (contine_doar_cifre(page_count) == 1) {
													snprintf(data[0], 1000, "{\n"
														"  \"title\": \"%s\",\n"
														"  \"author\": \"%s\",\n"
														"  \"genre\": \"%s\",\n"
														"  \"publisher\": \"%s\",\n"
														"  \"page_count\": %s\n"
														"}", title, author, genre, publisher, page_count);

													sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
													if (sockfd < 0) {
														printf("EROARE : Eroare la socket!\n");
													}

													message =
														compute_post_request(SERVER, SERVER_API_ADD_BOOK, "application/json",
																			data, 1, cookies, 1, auth_token);
													send_to_server(sockfd, message);
													response = receive_from_server(sockfd);

													if (strstr(response, "200 OK")) {
														printf("SUCCES : Carte adăugată cu succes!\n");
													}
													close_connection(sockfd);
												} else {
													printf("EROARE : Tip de date incorect pentru paginile cărții!\n");
												}
											} else {
												printf("EROARE : Șir gol la numărul de pagini!\n");
											}
										} else {
											printf("EROARE : Publicantul cărții nu e scris corect!\n");
										}
									} else {
										printf("EROARE : Șir gol la publicantul cărții!\n");
									}
								} else {
									printf("EROARE : Genul cărții nu e corect!\n");
								}
							} else {
								printf("EROARE : Șir gol la genul cărții!\n");
							}
						} else {
							printf("EROARE : Autorul nu are numele bun!\n");
						}
					} else {
						printf("EROARE : Șir gol la autorul cărții!\n");
					}
				} else {
					printf("EROARE : Șir gol la titlul cărții!\n");
				}
			} else {
				printf("EROARE : Nu ai token pentru add_book!\n");
			}
		}

		if (strcmp(command, "delete_book\n") == 0) {
			if (auth_token != NULL) {
				char *message = NULL, *response = NULL;
				int sockfd, book_id;
				char book_url[256];

				printf("id=");
				scanf("%d", &book_id);

				sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
				if (sockfd < 0) {
					printf("EROARE : Eroare la socket!\n");
				}

				sprintf(book_url, "%s%d", SERVER_API_DELETE_BOOK, book_id);

				message = compute_delete_request(SERVER, book_url, NULL, cookies, 1, auth_token);
				send_to_server(sockfd, message);
				response = receive_from_server(sockfd);
				if (strstr(response, "404 Not Found")) {
					printf("EROARE : Id-ul pentru care efectuaţi cererea este invalid!\n");
				} else {
					printf("SUCCES : Cartea cu id %d a fost ștearsă cu succes!\n", book_id);
				}

				close_connection(sockfd);

			} else {
				printf("EROARE : Nu ai token pentru delete_book!\n");
			}
		}

		if (strcmp(command, "logout\n") == 0) {
			if (cookies[0]) {
				char *message = NULL, *response;
				int sockfd;

				sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
				if (sockfd < 0) {
					printf("EROARE : Eroare la socket!\n");
				}

				message = compute_get_request(SERVER, SERVER_API_LOGOUT, NULL, cookies, 1, NULL);
				send_to_server(sockfd, message);
				response = receive_from_server(sockfd);
				if (!strstr(response, "200 OK")) {
					printf("EROARE : Utilizatorul nu s-a delogat cu succes!\n");
				}
           
				printf("SUCCES : Utilizatorul s-a delogat cu succes!\n");

				free(cookies[0]);
                cookies[0] = NULL;
				free(auth_token);
                auth_token = NULL;
				close_connection(sockfd);
			} else {
				printf("EROARE : Nu ați demonstrat ca sunteți autentificat!\n");
			}
		}

		if (strcmp(command, "exit\n") == 0) {
			break;
		}
	}
	return 0;
}
