# Programmation système et réseau - FTP client/server/slave

Dépôt GitHub: https://github.com/ozonePowered/c-ftp-server-client

Environnement de développement:
- macOS Sierra
- CLion
- CMake 3.7

## Compilation


```bash
mkdir build && cd build
cmake ..
make
```

## Exécution

Les exécutables sont placés dans le dossier bin à la racine du dossier source.

Serveur:
```bash
./bin/ftp_server
```

Serveur:
```bash
./bin/ftp_client
```

## Commandes CLI

NEED: 

Retourne: la taille et le contenu d'un fichier ou NOSUCHFILE si l'entrée de correspond à aucun chemin connu du serveur
```bash
NEED <chemin/du/fichier/à/afficher>
```
 