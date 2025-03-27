#!/usr/bin/env python3

from datetime import datetime

# Récupérer la date et l'heure actuelles
now = datetime.now()
date_heure_str = now.strftime("%Y-%m-%d %H:%M:%S")

# Construire la réponse HTML avec la date et l'heure, incluant le doctype
html_body = (
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "    <title>Affichage Date et Heure</title>\n"
    "</head>\n"
    "<body>\n"
    "    <h1>Date et Heure Actuelles</h1>\n"
    f"    <p>{date_heure_str}</p>\n"
    "</body>\n"
    "</html>\n"
)

# Calculer la taille du html_body (en octets)
content_length = len(html_body.encode('utf-8'))

# Construire le header HTTP avec la taille du body
html_header = (
    f"HTTP/1.1 200 OK\r\n"
    f"Content-Type: text/html; charset=UTF-8\r\n"
    f"Connection: keep-alive\r\n"
    f"Content-Length: {content_length}\r\n\r\n"
)

# Concaténer l'html_header et le html_body pour former la réponse complète
html_reponse = html_header + html_body

# Afficher la réponse complète
print(html_reponse)
