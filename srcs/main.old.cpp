#include <sys/types.h>
#include <sys/socket.h>
#include <cerrno>
#include <cstdio>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <string.h>
#include <sys/epoll.h>
#include <stdbool.h>
#include <iostream> // Pour std::string, std::ostringstream, etc.
#include <fstream>  // Pour std::ifstream
#include <sstream>
#include <string> // Pour std::ostringstream

#define MAX_EVENTS 10000

void send_file_response(int client_fd, const std::string &file_path)
{
    // Ouvrir le fichier HTML
    std::ifstream file(file_path.c_str(), std::ios::binary); // Ouvrir le fichier en mode binaire
    if (!file.is_open())
    {
        // Si le fichier ne peut pas être ouvert, envoyer une réponse d'erreur 500
        const char *error_response =
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: text/plain; charset=UTF-8\r\n"
            "Connection: close\r\n\r\n"
            "Erreur serveur : Impossible d'ouvrir le fichier.\r\n";
        send(client_fd, error_response, strlen(error_response), 0);
        return; // Ne ferme pas la socket ici, la connexion reste ouverte
    }

    // Lire tout le contenu du fichier dans une string
    std::ostringstream file_stream;
    file_stream << file.rdbuf();                  // Lire le fichier dans un flux de sortie
    std::string file_content = file_stream.str(); // Convertir le flux en string
    file.close();                                 // Fermer le fichier après lecture

    // Utiliser std::stringstream pour convertir la taille du fichier en chaîne
    std::stringstream ss;
    ss << file_content.size();
    std::string content_length = ss.str(); // Convertir la taille du fichier en chaîne

    // Construire l'en-tête HTTP avec la longueur du contenu du fichier
    std::string response_header =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: " +
        content_length + "\r\n\r\n";

    // Combiner l'en-tête et le contenu du fichier
    std::string full_response = response_header + file_content;

    // Envoyer la réponse complète (en-tête + contenu du fichier) en un seul appel à send()
    send(client_fd, full_response.c_str(), full_response.size(), 0);
}

bool print_request(struct epoll_event *event)
{
    char buffer[1024];                                                            // Tampon pour stocker la requête du client
    ssize_t bytes_received = recv(event->data.fd, buffer, sizeof(buffer) - 1, 0); // Lire les données du client

    if (bytes_received <= 0)
    {
        if (bytes_received == 0)
        {
            // Le client a fermé la connexion
            printf("Client déconnecté123\n");
        }
        else
        {
            // Une erreur s'est produite
            printf("Erreur de lecture du client : %s\n", strerror(errno));
        }
        return false;
    }
    else
    {
        buffer[bytes_received] = '\0';                    // Ajouter un caractère nul à la fin de la chaîne
        printf("Requête reçue du client : %s\n", buffer); // Afficher la requête dans le terminal
    }
    return true;
}

void keep(int fd)
{
    int keepalive = 1; // Activer Keep-Alive
    int keepidle = 10; // Attendre 60s avant d'envoyer un Keep-Alive
    int keepintvl = 2; // Intervalle de 10s entre chaque Keep-Alive
    int keepcnt = 5;   // Déconnecter après 5 échecs
    setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));
    setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle));
    setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(keepintvl));
    setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt));
    // int optval;
    // socklen_t optlen = sizeof(optval);
    // if (getsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) == 0)
    // {
    //     printf("SO_KEEPALIVE: %d\n", optval);
    // }
    // else
    // {
    //     perror("getsockopt SO_KEEPALIVE");
    // }

    // if (getsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &optval, &optlen) == 0)
    // {
    //     printf("TCP_KEEPIDLE: %d\n", optval);
    // }
    // else
    // {
    //     perror("getsockopt TCP_KEEPIDLE");
    // }

    // if (getsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &optval, &optlen) == 0)
    // {
    //     printf("TCP_KEEPINTVL: %d\n", optval);
    // }
    // else
    // {
    //     perror("getsockopt TCP_KEEPINTVL");
    // }

    // if (getsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &optval, &optlen) == 0)
    // {
    //     printf("TCP_KEEPCNT: %d\n", optval);
    // }
    // else
    // {
    //     perror("getsockopt TCP_KEEPCNT");
    // }
}

int main()
{

    int serv_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_fd < 0)
    {
        printf("Erreur : %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(8080);
    int opt = 1;
    setsockopt(serv_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(serv_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Erreur bind : %s\n", strerror(errno));
        close(serv_fd);
        exit(EXIT_FAILURE);
    }
    if (listen(serv_fd, 100) < 0)
    {
        printf("Erreur : %s\n", strerror(errno));
        close(serv_fd);
        exit(EXIT_FAILURE);
    }

    fcntl(serv_fd, F_SETFL, O_NONBLOCK);

    int keepalive = 1; // Activer Keep-Alive
    int keepidle = 10; // Attendre 60s avant d'envoyer un Keep-Alive
    int keepintvl = 2; // Intervalle de 10s entre chaque Keep-Alive
    int keepcnt = 5;   // Déconnecter après 5 échecs

    setsockopt(serv_fd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));
    setsockopt(serv_fd, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle));
    setsockopt(serv_fd, IPPROTO_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(keepintvl));
    setsockopt(serv_fd, IPPROTO_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt));
    //--------------------------------------------------------------------------------------
    int epoll_fd = epoll_create(1);
    if (epoll_fd == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }
    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];
    event.events = EPOLLIN | EPOLLET | EPOLLHUP;
    event.data.fd = serv_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serv_fd, &event) == -1)
    {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        //---------------------------
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_events == -1)
        {
            printf("Erreur : %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < num_events; i++)
        {
            if (events[i].events & EPOLLHUP )
            {
                printf("Client déconnecté \n");
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                close(events[i].data.fd);
                continue;
            }
            else if (events[i].data.fd == serv_fd)
            {
                printf("DEMANDE DE CONNEXION\n");

                int client_fd = accept(serv_fd, (struct sockaddr *)&client_addr, &client_len);
                if (client_fd < 0)
                {
                    printf("Erreur : %s\n", strerror(errno));
                    continue;
                }
                keep(client_fd);
                event.events = EPOLLIN | EPOLLERR | EPOLLHUP;
                event.data.fd = client_fd;

                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1)
                {
                    perror("epoll_ctl");
                    exit(EXIT_FAILURE);
                }
            }
          

            else
            {

                print_request(&events[i]);
                send_file_response(events[i].data.fd, "HTML/acceuil.html");
            }
        }
    }
    close(serv_fd);
    return 0;
}
