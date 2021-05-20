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
#include "parson.h"

#define PORT 8080
#define HOST "34.118.48.238"
#define MAX_LEN 3000
#define BUFFLEN 4096

void successMessage(char *successM);
void errorMessage(JSON_Value *responseJSON);
void registerClient(char *username, char *password);
void login(char *username, char *password, Client *client);
void enter_library(Client *client);
void get_books(Client *client);
void get_book(char *book, Client *client);
void add_book(char *title, char *author, char *genre, int page, char *publisher, Client *client);
void delete_book();
void logout(Client *client);

void adaptString(char *string)
{
    string[strlen(string) - 1] = '\0';
}

int main(int argc, char *argv[])
{
    Client *client = (Client *)malloc(sizeof(Client));
    client->JWT_token = NULL;
    client->login = 0;
    client->password = NULL;
    client->username = NULL;
    client->cookie = NULL;

    char comanda[15];
    char username[BUFFLEN];
    char password[BUFFLEN];
    int ID;
    char page[BUFFLEN];
    char title[BUFFLEN];
    char author[BUFFLEN];
    char genre[BUFFLEN];
    char publisher[BUFFLEN];
    int page_count;

    while (1)
    {
        memset(comanda, 0, 15);
        fgets(comanda, BUFFLEN, stdin);
        if (strcmp(comanda, "register\n") == 0)
        {
            memset(username, 0, BUFFLEN);
            memset(password, 0, BUFFLEN);

            printf("%s=", "username");
            fgets(username, BUFFLEN, stdin);
            adaptString(username);

            printf("%s=", "password");
            fgets(password, BUFFLEN, stdin);
            adaptString(password);

            client->password = password;
            client->username = username;
            client->login = 0;
            registerClient(username, password);
        }
        else if (strcmp(comanda, "login\n") == 0)
        {
            memset(username, 0, BUFFLEN);
            memset(password, 0, BUFFLEN);

            printf("%s=", "username");
            fgets(username, BUFFLEN, stdin);
            adaptString(username);

            printf("%s=", "password");
            fgets(password, BUFFLEN, stdin);
            adaptString(password);

            if (client->login == 0)
            {
                login(username, password, client);
            }
        }
        else if (strcmp(comanda, "enter_library\n") == 0)
        {
            enter_library(client);
        }
        else if (strcmp(comanda, "get_books\n") == 0)
        {
            get_books(client);
        }
        else if (strcmp(comanda, "get_book\n") == 0)
        {
            printf("%s=", "id");

            fgets(page, BUFFLEN, stdin);
            adaptString(page);

            get_book(page, client);
        }
        else if (strcmp(comanda, "add_book\n") == 0)
        {
            memset(title, 0, BUFFLEN);
            memset(author, 0, BUFFLEN);
            memset(genre, 0, BUFFLEN);
            memset(publisher, 0, BUFFLEN);
            memset(page, 0, BUFFLEN);

            printf("%s=", "title");
            fgets(title, BUFFLEN, stdin);
            adaptString(title);

            printf("%s=", "author");
            fgets(author, BUFFLEN, stdin);
            adaptString(author);

            printf("%s=", "genre");
            fgets(genre, BUFFLEN, stdin);
            adaptString(genre);

            printf("%s=", "publisher");
            fgets(publisher, BUFFLEN, stdin);
            adaptString(publisher);

            printf("%s=", "page_count");
            fgets(page, BUFFLEN, stdin);
            adaptString(page);
            page_count = atoi(page);

            add_book(title, author, genre, page_count, publisher, client);
        }
        else if (strcmp(comanda, "delete_book\n") == 0)
        {
            printf("%s=", "id");
            scanf("%d\n", &ID);
            printf("%s", comanda);
        }
        else if (strcmp(comanda, "logout\n") == 0)
        {
            logout(client);
        }
        else if (strcmp(comanda, "exit\n") == 0)
        {
            return 0;
        }
        else
        {
            printf("%s\n", "error: This is an unknown command !");
        }
    }
    return 0;
}

void errorMessage(JSON_Value *responseJSON)
{
    char out[150];
    JSON_Object *obj = json_object(responseJSON);
    int count = json_object_get_count(obj);
    if (count > 0)
    {
        const char *name = json_object_get_name(obj, 0);
        const char *error = json_object_get_string(obj, name);

        char errorMessage[150];

        strcpy(out, "");
        for (int i = 1; i <= 60; i++)
        {
            strcat(out, "*");
        }
        printf("%s\n", out);

        memset(out, 0, 150);
        strcat(out, "**");

        memset(errorMessage, 0, 150);
        strcat(errorMessage, name);
        strcat(errorMessage, " : ");
        strcat(errorMessage, error);

        for (int i = 1; i <= (56 - strlen(errorMessage)) / 2; i++)
        {
            strcat(out, " ");
        }
        strcat(out, errorMessage);
        for (int i = 1; i <= (56 - strlen(errorMessage)) / 2; i++)
        {
            strcat(out, " ");
        }

        if (strlen(out) % 2 == 1)
        {
            strcat(out, " ");
        }

        strcat(out, "**");
        printf("%s\n", out);

        strcpy(out, "");
        for (int i = 1; i <= 60; i++)
        {
            strcat(out, "*");
        }
        printf("%s\n", out);
    }
}

void successMessage(char *successM)
{
    char out[150];
    strcpy(out, "");
    for (int i = 1; i <= 60; i++)
    {
        strcat(out, "*");
    }
    printf("%s\n", out);

    memset(out, 0, 150);
    strcat(out, "**");

    for (int i = 1; i <= (56 - strlen(successM)) / 2; i++)
    {
        strcat(out, " ");
    }
    strcat(out, successM);
    for (int i = 1; i <= (56 - strlen(successM)) / 2; i++)
    {
        strcat(out, " ");
    }

    if (strlen(out) % 2 == 1)
    {
        strcat(out, " ");
    }

    strcat(out, "**");
    printf("%s\n", out);

    strcpy(out, "");
    for (int i = 1; i <= 60; i++)
    {
        strcat(out, "*");
    }
    printf("%s\n", out);
}

void registerClient(char *username, char *password)
{
    char *message;
    char *response;
    int sockfd;

    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("[!] Connect error.");
        exit(1);
    }

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *login = NULL;
    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);
    login = json_serialize_to_string_pretty(root_value);
    json_value_free(root_value);

    char *url = "/api/v1/tema/auth/register";
    char *type = "application/json";

    message = compute_post_request(HOST, url, type, login, 1, NULL, 0, NULL);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    JSON_Value *responseJSON;
    responseJSON = json_parse_string(strchr(response, '{'));

    if (responseJSON != NULL)
    {
        errorMessage(responseJSON);
    }
    else
    {
        char successM[150];
        memset(successM, 0, 150);
        strcat(successM, "Inregistrarea a reusit!");
        successMessage(successM);
    }
    close_connection(sockfd);
}

void login(char *username, char *password, Client *client)
{
    char *message;
    char *response;
    int sockfd;

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *login = NULL;
    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);
    login = json_serialize_to_string_pretty(root_value);
    json_value_free(root_value);

    char *url = "/api/v1/tema/auth/login";
    char *type = "application/json";

    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("[!] Connect error.");
        exit(1);
    }

    message = compute_post_request(HOST, url, type, login, 1, NULL, 0, NULL);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    printf("%s", response);

    JSON_Value *responseJSON;
    responseJSON = json_parse_string(strchr(response, '{'));

    if (responseJSON != NULL)
    {
        errorMessage(responseJSON);
    }
    else
    {
        char *cookie = (char *)malloc(500 * sizeof(char));
        char *aux = strtok(response, "\n");
        char *beforeCookie = (char *)malloc(500 * sizeof(char));
        while (aux != NULL)
        {
            if (strstr(aux, "Set-Cookie") != NULL)
            {
                strcat(beforeCookie, aux + strlen("Set-Cookie ") + 1);
            }
            aux = strtok(NULL, "\n");
        }

        cookie = strtok(beforeCookie, ";");

        char successM[150];
        memset(successM, 0, 150);
        strcat(successM, "Ai reusit sa te loghezi in contul tau!");
        successMessage(successM);
        client->login = 1;
        client->cookie = cookie;
        printf("cookie:_ %ld", strlen(cookie));
    }

    close_connection(sockfd);
}

void enter_library(Client *client)
{
    char *message;
    char *response;
    int sockfd;

    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("[!] Connect error.");
        exit(1);
    }

    char *url = "/api/v1/tema/library/access";

    message = compute_get_request(HOST, url, NULL, &(client->cookie), 1, NULL);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    printf("%s", response);

    JSON_Value *responseJSON;
    responseJSON = json_parse_string(strchr(response, '{'));

    JSON_Object *obj = json_object(responseJSON);

    const char *name = json_object_get_name(obj, 0);

    if (strcmp(name, "token") == 0)
    {
        const char *token = json_object_get_string(obj, name);

        printf("%ld", strlen(token));

        client->JWT_token = (char *)token;

        char successM[150];
        memset(successM, 0, 150);
        strcat(successM, "Ai reusit sa obtii acces in biblioteca!");
        successMessage(successM);
    }
    else
    {
        errorMessage(responseJSON);
    }

    close_connection(sockfd);
}

void get_books(Client *client)
{
    char *message;
    char *response;
    int sockfd;

    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("[!] Connect error.");
        exit(1);
    }

    char *url = "/api/v1/tema/library/books";

    message = compute_get_request(HOST, url, NULL, &(client->cookie), 1, client->JWT_token);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    printf("%s", response);

    close_connection(sockfd);
}

void get_book(char *book, Client *client)
{
    char *message;
    char *response;
    int sockfd;

    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("[!] Connect error.");
        exit(1);
    }

    char url[100];
    strcpy(url, "/api/v1/tema/library/books/");
    strcat(url, book);

    message = compute_get_request(HOST, url, NULL, &(client->cookie), 1, client->JWT_token);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    printf("%s", response);
}

void add_book(char *title, char *author, char *genre, int page, char *publisher, Client *client)
{
    char *message;
    char *response;
    int sockfd;

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *login = NULL;

    json_object_set_string(root_object, "title", title);
    json_object_set_string(root_object, "author", author);
    json_object_set_string(root_object, "genre", genre);
    json_object_set_number(root_object, "page_count", page);
    json_object_set_string(root_object, "publisher", publisher);
    login = json_serialize_to_string_pretty(root_value);
    //json_value_free(root_value);

    char *url = "/api/v1/tema/library/books";
    char *type = "application/json";

    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("[!] Connect error.");
        exit(1);
    }

    printf("%ld", strlen(client->JWT_token));
    printf("%ld", strlen(client->cookie));

    message = compute_post_request(HOST, url, type, login, 1, &(client->cookie), 1, client->JWT_token);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    printf("%s", response);

    close_connection(sockfd);
}

void logout(Client *client)
{
    char *message;
    char *response;
    int sockfd;

    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("[!] Connect error.");
        exit(1);
    }

    char *url = "/api/v1/tema/auth/logout";

    message = compute_get_request(HOST, url, NULL, &(client->cookie), 1, NULL);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    JSON_Value *responseJSON;
    responseJSON = json_parse_string(strchr(response, '{'));

    if (responseJSON != NULL)
    {
        errorMessage(responseJSON);
    }
    else
    {
        char successM[150];
        memset(successM, 0, 150);
        strcat(successM, "Deconectarea a reusit!");
        successMessage(successM);
    }
    close_connection(sockfd);
}