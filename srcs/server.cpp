#include "server.hpp"
//-------------------------------------------
#include <stdio.h>
#include <fstream>
#include <sstream>

//------------------------------------------------------------
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
//-------------------------------------------------------------

server::server()
{
    _socket.init_listener_socket();
}

server::~server()
{
}

int server::get_serv_fd() const
{
    return (_socket.get_fd());
}

void server::start()
{
    _epollMgr.init(get_serv_fd());

    while (1)
    {
        _epollMgr.waitingForEvents();

        for (int i = 0; i < _epollMgr.num_events; i++)
        {

            if (_epollMgr.events[i].events & EPOLLHUP)
            {
                _epollMgr.del_fd(_epollMgr.events[i].data.fd);
                // close la socket;
                continue;
            }
            else if (_epollMgr.events[i].data.fd == get_serv_fd())
            {
                sock socket_client;
                try
                {

                    socket_client.accept_client(get_serv_fd());
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << std::endl;
                    continue;
                }
                try
                {

                    _epollMgr.add_fd(socket_client.get_fd());
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << std::endl;
                }
            }
            else
            {

                print_request(&_epollMgr.events[i]);
                send_file_response(_epollMgr.events[i].data.fd, "HTML/acceuil.html");
            }
        }
    }
}