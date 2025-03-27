#!/usr/bin/env python3

from datetime import datetime


now = datetime.now()
date_str = now.strftime("%Y-%m-%d")
time_str = now.strftime("%H:%M:%S")

html_body = (
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "    <meta charset='UTF-8'>\n"
    "    <title>Affichage Date et Heure</title>\n"
    "    <style>\n"
    "        body {\n"
    "            display: flex;\n"
    "            justify-content: center;\n"
    "            align-items: center;\n"
    "            height: 100vh;\n"
    "            text-align: center;\n"
    "            font-family: Arial, sans-serif;\n"
    "            background: linear-gradient(to right, #00c6ff, #007f00);\n"
    "            color: white;\n"
    "        }\n"
    "        h1 {\n"
    "            font-size: 2em;\n"
    "        }\n"
    "        p {\n"
    "            font-size: 1.5em;\n"
    "        }\n"
    "    </style>\n"
    "</head>\n"
    "<body>\n"
    "    <div>\n"
    "        <h1>Date et Heure</h1>\n"
    f"        <p>{date_str} - <span id='heure'>{time_str}</span></p>\n"
    "    </div>\n"
    "    <script>\n"
    "        function updateTime() {\n"
    "            let now = new Date();\n"
    "            let heureActuelle = now.toLocaleTimeString();\n"
    "            document.getElementById('heure').innerText = heureActuelle;\n"
    "        }\n"
    "        setInterval(updateTime, 1000);\n"
    "        updateTime();\n"
    "    </script>\n"
    "</body>\n"
    "</html>\n"
)

content_length = len(html_body.encode('utf-8'))

html_header = (
    f"HTTP/1.1 200 OK\r\n"
    f"Content-Type: text/html; charset=UTF-8\r\n"
    f"Content-Length: {content_length}\r\n\r\n"
)

print(html_header + html_body)
