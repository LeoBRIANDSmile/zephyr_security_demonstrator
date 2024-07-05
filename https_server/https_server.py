import http.server
import ssl

# Configuration du serveur
server_address = ('172.20.10.2', 443)  # Adresse du serveur, écoute sur le port 443
httpd = http.server.HTTPServer(server_address, http.server.SimpleHTTPRequestHandler)

# Ajout de la couche SSL
httpd.socket = ssl.wrap_socket(httpd.socket,
                               server_side=True,
                               certfile='certs/host-cert.pem',
                               keyfile='certs/host-key.pem',
                               ssl_version=ssl.PROTOCOL_TLS,
                               ciphers="DEFAULT")

print("Serving on https://localhost:443")
httpd.serve_forever()
