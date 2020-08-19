#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "buffer.h"
#include "parson.h"

#define BUFLENGTH 20
#define IP "3.8.116.10"
#define PORT 8080

void registering(int sockfd) {
	char* aux_buf = (char*) calloc(BUFLEN, sizeof(char));
	char* message;
	char* response;
	char* ret = (char*) calloc(BUFLEN, sizeof(char));

    printf("Please type an username: ");
    fgets(aux_buf, BUFLENGTH, stdin);
    aux_buf = strtok(aux_buf, "\n");
    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);
    char* serialized_string = NULL;
    json_object_set_string(root_object, "username", aux_buf);
 	
 	printf("Please type a password: ");
    fgets(aux_buf, BUFLENGTH, stdin);
    aux_buf = strtok(aux_buf, "\n");
    json_object_set_string(root_object, "password", aux_buf);
    serialized_string = json_serialize_to_string_pretty(root_value);

    sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    message = compute_post_request(IP, "/api/v1/tema/auth/register",
    "application/json", &serialized_string, 1, NULL, 0, NULL, 0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    ret = strcpy(ret, response);
    ret = strtok(ret, " ");
    ret = strtok(NULL, " ");
    int code = atoi(ret);
    if (code == 200) {
    	printf("OK\n");
    	return;
    }
    printf("%s\n", basic_extract_json_response(response));
}

char* login(int sockfd) {
	char* aux_buf = (char*) calloc(BUFLEN, sizeof(char));
	char* message;
	char* response;
	char* ret = (char*) calloc(BUFLEN, sizeof(char));

    printf("Username: ");
    fgets(aux_buf, BUFLENGTH, stdin);
    aux_buf = strtok(aux_buf, "\n");
    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);
    char* serialized_string = NULL;
    json_object_set_string(root_object, "username", aux_buf);

    printf("Password: ");
    fgets(aux_buf, BUFLENGTH, stdin);
    aux_buf = strtok(aux_buf, "\n");
    json_object_set_string(root_object, "password", aux_buf);
    serialized_string = json_serialize_to_string_pretty(root_value);

    sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    message = compute_post_request(IP, "/api/v1/tema/auth/login",
    "application/json", &serialized_string, 1, NULL, 0, NULL, 0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    ret = strcpy(ret, response);
    ret = strtok(ret, " ");
    ret = strtok(NULL, " ");
    int code = atoi(ret);
    if (code == 200) {
    	ret = strstr(response, "connect.sid");
		ret = strtok(ret, ";");
		printf("OK\n");
		return ret;
    }
    printf("%s\n", basic_extract_json_response(response));
    return NULL;
}

char* enter_library(int sockfd, char* cookie[1]) {
	sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
	char* message;
	char* response;
	char* ret = (char*) calloc(BUFLEN, sizeof(char));

    message = compute_get_request(IP, "/api/v1/tema/library/access", NULL,
    	cookie, 1, NULL, 0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    ret = strcpy(ret, response);
    ret = strtok(ret, " ");
    ret = strtok(NULL, " ");
    int code = atoi(ret);
    if (code == 200) {
    	ret = strstr(response, "token");
    	ret = strtok(ret, ":, \"");
    	ret = strtok(NULL, ":, \"");
    	printf("OK\n");
    	return ret;
    }
    printf("%s\n", basic_extract_json_response(response));
    return NULL;
}

void get_books (int sockfd, char* cookie[1], char* token[1]) {
	sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
	char* message;
	char* response;

    message = compute_get_request(IP, "/api/v1/tema/library/books", NULL,
    	cookie, 1, token, 1);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    char* ret = (char*) calloc(BUFLEN, sizeof(char));
    ret = strcpy(ret, response);
    ret = strtok(ret, " ");
    ret = strtok(NULL, " ");
    int code = atoi(ret);
    if (code == 200) {
    	if (basic_extract_json_response(response) == NULL) {
    		printf("[]\n");
    		return;
    	}
    	printf("%s\n", basic_extract_json_response(response));
    } else if (cookie[0][0] == 0) {
    	printf("You are not logged in!\n");
    } else {
    	printf("You have not entered the library!\n");
    }
}

void get_book (int sockfd, char* cookie[1], char* token[1]) {
	char* message;
	char* response;
	printf("Enter an ID: ");

	char* ID = (char*) calloc(BUFLENGTH, sizeof(char));
	fgets(ID, BUFLENGTH, stdin);
    ID = strtok(ID, "\n");

    char* path = (char*) calloc(BUFLEN, sizeof(char));
    path = strcpy(path, "/api/v1/tema/library/books/");
    path = strcat(path, ID);

    sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
	message = compute_get_request(IP, path, NULL, cookie, 1, token, 1);
	send_to_server(sockfd , message);
	response = receive_from_server(sockfd);
    char* ret = (char*) calloc(BUFLEN, sizeof(char));
    ret = strcpy(ret, response);
    ret = strtok(ret, " ");
    ret = strtok(NULL, " ");
    int code = atoi(ret);
    if (code == 200) {
    	if (basic_extract_json_response(response) == NULL) {
    		printf("[]\n");
    		return;
    	}
    	printf("%s\n", basic_extract_json_response(response));
    } else if (cookie[0][0] == 0) {
    	printf("You are not logged in!\n");
    } else {
    	printf("You have not entered the library!\n");
    }
}

void add_book(int sockfd, char* cookie[1], char* token[1]) {
	char* message;
	char* response;
	char* aux_buf = (char*) calloc(BUFLEN, sizeof(char));

	printf("Title: ");
    fgets(aux_buf, BUFLENGTH, stdin);
    aux_buf = strtok(aux_buf, "\n");
    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);
    char* serialized_string = NULL;
    json_object_set_string(root_object, "title", aux_buf);

    printf("Author: ");
    fgets(aux_buf, BUFLENGTH, stdin);
    aux_buf = strtok(aux_buf, "\n");
    json_object_set_string(root_object, "author", aux_buf);

    printf("Genre: ");
    fgets(aux_buf, BUFLENGTH, stdin);
    aux_buf = strtok(aux_buf, "\n");
    json_object_set_string(root_object, "genre", aux_buf);

    printf("Publisher: ");
    fgets(aux_buf, BUFLENGTH, stdin);
    aux_buf = strtok(aux_buf, "\n");
    json_object_set_string(root_object, "publisher", aux_buf);

    printf("Page count: ");
    fgets(aux_buf, BUFLENGTH, stdin);
    aux_buf = strtok(aux_buf, "\n");
    json_object_set_number(root_object, "page_count", atoi(aux_buf));
    
    sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    serialized_string = json_serialize_to_string_pretty(root_value);

    message = compute_post_request(IP, "/api/v1/tema/library/books",
    "application/json", &serialized_string, 1, cookie, 1, token, 1);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    char* ret = (char*) calloc(BUFLEN, sizeof(char));
    ret = strcpy(ret, response);
    ret = strtok(ret, " ");
    ret = strtok(NULL, " ");
    int code = atoi(ret);
    if (code == 200) {
    	if (basic_extract_json_response(response) == NULL) {
    		printf("OK\n");
    		return;
    	}
    	printf("%s\n", basic_extract_json_response(response));
    } else if (cookie[0][0] == 0) {
    	printf("You are not logged in!\n");
    } else {
    	printf("You have not entered the library!\n");
    }
}

void delete_book(int sockfd, char* cookie[1], char* token[1]) {
	char* message;
	char* response;
    char* ID = (char*) calloc(BUFLENGTH, sizeof(char));

    printf("Enter an ID: ");
	fgets(ID, BUFLENGTH, stdin);
    ID = strtok(ID, "\n");

    char* path = (char*) calloc(BUFLEN, sizeof(char));
    path = strcpy(path, "/api/v1/tema/library/books/");
    path = strcat(path, ID);

    sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    message = compute_delete_request(IP, path, cookie, 1, token, 1);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    char* ret = (char*) calloc(BUFLEN, sizeof(char));
    ret = strcpy(ret, response);
    ret = strtok(ret, " ");
    ret = strtok(NULL, " ");
    int code = atoi(ret);
    if (code == 200) {
    	if (basic_extract_json_response(response) == NULL) {
    		printf("OK\n");
    		return;
    	}
    	printf("%s\n", basic_extract_json_response(response));
    } else if (cookie[0][0] == 0) {
    	printf("You are not logged in!\n");
    } else {
    	printf("You have not entered the library!\n");
    }
}

char* logout(int sockfd, char* cookie[1]) {
	sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
	char* message;
	char* response;

    message = compute_get_request(IP, "/api/v1/tema/auth/logout", NULL,	cookie,
    	1, NULL, 0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    char* ret = (char*) calloc(BUFLEN, sizeof(char));
    ret = strcpy(ret, response);
    ret = strtok(ret, " ");
    ret = strtok(NULL, " ");
    int code = atoi(ret);
    if (code == 200) {
    	if (basic_extract_json_response(response) == NULL) {
    		printf("OK\n");
    		char* x = (char*) calloc(BUFLEN, sizeof(char));
    		return x;
    	}
    	printf("%s\n", basic_extract_json_response(response));
    } else if (cookie[0][0] == 0) {
    	printf("You are not logged in!\n");
    }
    return cookie[0];
}

int main(int argc, char *argv[])
{
    int sockfd;
    char* buffer = (char*) calloc(BUFLENGTH, sizeof(char));

    char* cookie[1];
    cookie[0] = (char*) calloc(BUFLENGTH, sizeof(char));
    char* token[1];
    token[0] = (char*) calloc(BUFLENGTH, sizeof(char));

    // connection opened
    sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);

    // Ex 1.1: GET dummy from main server

    while(1) {
    	char* aux = (char*) calloc(BUFLENGTH, sizeof(char));
    	fgets(aux, BUFLENGTH, stdin);
    	buffer = strcpy(buffer, aux);
    	if (buffer == NULL) {
    		continue;
    	}
    	if (strncmp(buffer, "register", 8) == 0) {
    		registering(sockfd);
    	} else if (strncmp(buffer, "login", 5) == 0) {
    		cookie[0] = login(sockfd);
    	} else if (strncmp(buffer, "enter_library", 13) == 0) {
    		token[0] = enter_library(sockfd, cookie);
    	} else if (strncmp(buffer, "get_books", 9) == 0) {
    		get_books(sockfd, cookie, token);
    	} else if (strncmp(buffer, "get_book", 8) == 0) {
    		get_book(sockfd, cookie, token);
    	} else if (strncmp(buffer, "add_book", 8) == 0) {
    		add_book(sockfd, cookie, token);
    	} else if (strncmp(buffer, "delete_book", 11) == 0) {
    		delete_book(sockfd, cookie, token);
    	} else if (strncmp(buffer, "logout", 6) == 0) {
    		cookie[0] = logout(sockfd, cookie);
    	} else if (strncmp(buffer, "exit", 4) == 0) {
    		sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
    		close_connection(sockfd);
    		break;
    	} else {
    		printf("Invalid command!\n");
    	}
    }

    return 0;
}
