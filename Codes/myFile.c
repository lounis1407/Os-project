/**
 * @file myFile.c
 * @brief Implémentation des fonctions pour la gestion des fichiers dans une partition.
 * @authors TAGHELIT Wassim, BOUHADOUN Lounis & HARICHE Yani
 */

#include "myFile.h"
#include <string.h>
bloc_donnees allouer_bloc()
{
    bloc_donnees nouveauBloc;
    nouveauBloc.nb_octets = 0; // Aucun caractère n'est encore stocké dans le bloc
    for (int i = 0; i < max_chars_par_bloc; i++)
    {
        nouveauBloc.donnee[i] = '\0'; // Initialiser les données à vide
    }
    nouveauBloc.blocSuivant = -1; // -1 indique qu'il n'y a pas de bloc suivant

    return nouveauBloc;
}

// qui vise à trouver l'offset (position relative dans la partition) d'un bloc de données spécifique d'un fichier donné par son numéro de bloc
int offset_bloc(file *f, int blocNumber)
{
    if (f == NULL || blocNumber < 1)
    {
        return -1; // Retourne -1 en cas d'erreur ou si le numéro de bloc est invalide
    }

    // Commence par le premier bloc de données du fichier
    off_t offsetActuel = f->numEntete;
    header_bloc_fichier entete;
    lseek(f->fd, offsetActuel, SEEK_SET);
    read(f->fd, &entete, sizeof(header_bloc_fichier));

    off_t offsetBloc = entete.premierBlocDonnees;
    bloc_donnees bloc;

    // Parcourt les blocs jusqu'à atteindre le blocNumber
    for (int i = 1; i < blocNumber; i++)
    {
        // Lit le bloc actuel pour obtenir l'offset du bloc suivant
        lseek(f->fd, offsetBloc, SEEK_SET);
        if (read(f->fd, &bloc, sizeof(bloc_donnees)) < sizeof(bloc_donnees))
        {
            return -1; // Erreur de lecture, retourne -1
        }

        // Si c'est le dernier bloc et que nous cherchons encore le bloc suivant, retourne -1
        if (bloc.blocSuivant == -1)
        {
            return -1;
        }

        offsetBloc = bloc.blocSuivant; // Passe au bloc suivant
    }

    return offsetBloc; // Retourne l'offset du blocNumber spécifié
}

int count_blocs(file *f)
{
    if (f == NULL)
    {
        return -1; // Erreur, fichier invalide
    }

    header_bloc_fichier entete;
    lseek(f->fd, f->numEntete, SEEK_SET); // Positionne à l'offset du bloc d'entête
    if (read(f->fd, &entete, sizeof(header_bloc_fichier)) != sizeof(header_bloc_fichier))
    {
        return -1; // Erreur de lecture
    }

    return entete.nb_blocs; // Retourne le nombre de blocs de données du fichier
}

int get_file_header(file *f)
{
    if (f == NULL)
    {
        return -1; // Erreur, fichier invalide
    }

    header_bloc_fichier entete;
    lseek(f->fd, f->numEntete, SEEK_SET); // Positionne à l'offset du bloc d'entête
    if (read(f->fd, &entete, sizeof(header_bloc_fichier)) != sizeof(header_bloc_fichier))
    {
        return -1; // Erreur de lecture
    }

    return entete.premierBlocDonnees; // Retourne l'offset du premier bloc de données
}

int set_nb_blocs(file *f, int val)
{
    if (f == NULL)
    {
        return -1; // Erreur, fichier invalide
    }

    header_bloc_fichier entete;
    lseek(f->fd, f->numEntete, SEEK_SET); // Positionne à l'offset du bloc d'entête
    if (read(f->fd, &entete, sizeof(header_bloc_fichier)) != sizeof(header_bloc_fichier))
    {
        return -1; // Erreur de lecture
    }

    entete.nb_blocs = val;                // Met à jour le nombre de blocs de données
    lseek(f->fd, f->numEntete, SEEK_SET); // Repositionne pour écrire
    if (write(f->fd, &entete, sizeof(header_bloc_fichier)) != sizeof(header_bloc_fichier))
    {
        return -1; // Erreur d'écriture
    }

    return 0; // Succès
}

int set_header(file *f, off_t val)
{
    if (f == NULL)
    {
        return -1; // Erreur, fichier invalide
    }

    header_bloc_fichier entete;
    lseek(f->fd, f->numEntete, SEEK_SET); // Positionne à l'offset du bloc d'entête
    if (read(f->fd, &entete, sizeof(header_bloc_fichier)) != sizeof(header_bloc_fichier))
    {
        return -1; // Erreur de lecture
    }

    entete.premierBlocDonnees = val;      // Met à jour l'offset du premier bloc de données
    lseek(f->fd, f->numEntete, SEEK_SET); // Repositionne pour écrire
    if (write(f->fd, &entete, sizeof(header_bloc_fichier)) != sizeof(header_bloc_fichier))
    {
        return -1; // Erreur d'écriture
    }

    return 0; // Succès
}

// Function to get the partition name from the actualpartitionname.txt file
char *getPartitionName()
{
    int file = open("nom_partition.txt", O_RDONLY);
    if (file != -1)
    {
        char currentPartitionName[256];
        ssize_t bytesRead = read(file, currentPartitionName, sizeof(currentPartitionName) - 1);
        if (bytesRead != -1)
        {
            currentPartitionName[bytesRead] = '\0';
            close(file);
            return strdup(currentPartitionName);
        }
        close(file);
    }
    return NULL;
}

// Function to set the partition name in the actualpartitionname.txt file
int setPartitionName(char *partitionName)
{
    int file = open("nom_partition.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (file == -1)
    {
        perror("Erreur lors de l'ouverture ou de la création du fichier actualpartitionname.txt");
        return -1; // Return a custom error code
    }

    ssize_t bytesWrittenName = write(file, partitionName, strlen(partitionName));
    if (bytesWrittenName == -1)
    {
        perror("Erreur lors de l'écriture du nom de la partition dans actualpartitionname.txt");
        close(file);
        return -1; // Return a custom error code
    }

    close(file);
    return 0; // Return 0 to indicate the success of the operation
}

int myFormat(char *partitionName)
{
    // Get the current partition name
    char *currentPartitionName = getPartitionName();
    if (currentPartitionName != NULL)
    {
        // Delete the old partition file
        remove(currentPartitionName);
        free(currentPartitionName);
    }

    // Set the new partition name
    int setPartitionNameResult = setPartitionName(partitionName);
    if (setPartitionNameResult == -1)
    {
        return -1; // Return a custom error code
    }

    // Open the partition file in create/truncate mode
    int fd = open(partitionName, O_CREAT | O_TRUNC | O_RDWR, 0777);
    if (fd == -1)
    {
        perror("Erreur lors de l'ouverture ou de la création du fichier de partition");
        return -1; // Return a custom error code
    }

    // Initialize the partition infos_pdata with default values
    infos_partition infos_p;
    infos_p.first_file = -1; // -1 to indicate that no file exists yet
    infos_p.last_file = -1;  // -1 to indicate that no file exists yet

    // Write the partition infos_pdata at the beginning of the file
    ssize_t bytesWritteninfos_p = write(fd, &infos_p, sizeof(infos_partition));
    if (bytesWritteninfos_p != sizeof(infos_partition))
    {
        perror("Erreur lors de l'écriture des métadonnées de la partition");
        close(fd);
        return -1; // Return a custom error code
    }

    // Close the partition file after initialization
    close(fd);
    return 0; // Return 0 to indicate the success of the operation
}
file *myOpen(char *fileName)
{
    char *partitionName = getPartitionName();
    if (partitionName == NULL)
    {
        perror("Impossible de récupérer le nom de la partition actuelle");
        return NULL;
    }

    // Ouvre le fichier de partition en mode lecture/écriture.
    int fd = open(partitionName, O_RDWR);
    if (fd == -1)
    {
        perror("Erreur lors de l'ouverture du fichier de partition");
        free(partitionName);
        return NULL;
    }

    free(partitionName);
    // Lecture des métadonnées de la partition.
    infos_partition infos_p;
    if (read(fd, &infos_p, sizeof(infos_partition)) != sizeof(infos_partition))
    {
        perror("Erreur lors de la lecture des métadonnées de la partition");
        close(fd);
        return NULL;
    }

    off_t currentOffset = infos_p.first_file;
    header_bloc_fichier entete;
    int found = 0;

    // Parcours des fichiers existants pour vérifier si le fichier demandé existe déjà.
    while (currentOffset != -1 && !found)
    {
        lseek(fd, currentOffset, SEEK_SET); // Déplace le pointeur au début de l'entête du fichier courant
        read(fd, &entete, sizeof(header_bloc_fichier));
        if (strcmp(entete.nomFichier, fileName) == 0)
        {
            // Fichier trouvé.
            found = 1;
            break;
        }
        currentOffset = entete.fichierSuivant;
    }

    file *f = (file *)malloc(sizeof(file));
    if (f == NULL)
    {
        perror("Erreur lors de l'allocation de mémoire pour le fichier");
        close(fd);
        return NULL;
    }

    // Initialise la structure file.
    f->fd = fd;
    f->pos = 0;

    if (!found)
    {
        // Le fichier n'existe pas, créez un nouveau fichier.
        // Configure le bloc d'entête pour le nouveau fichier.
        memset(&entete, 0, sizeof(header_bloc_fichier)); // Initialisation de la structure
        strncpy(entete.nomFichier, fileName, sizeof(entete.nomFichier) - 1);
        entete.premierBlocDonnees = -1; // Aucun bloc de données au début.
        entete.nb_blocs = 0;
        entete.fichierSuivant = infos_p.first_file;  // Insère le nouveau fichier en tête de liste.
        lseek(fd, 0, SEEK_END);                      // Déplace le pointeur à la fin pour la nouvelle entête.
        infos_p.first_file = lseek(fd, 0, SEEK_CUR); // Obtient la position actuelle qui est la nouvelle entête.

        // Écriture de la nouvelle entête de fichier dans la partition.
        if (write(fd, &entete, sizeof(header_bloc_fichier)) != sizeof(header_bloc_fichier))
        {
            perror("Erreur lors de l'écriture de l'entête du nouveau fichier");
            free(f);
            close(fd);
            return NULL;
        }

        // Mise à jour des métadonnées de la partition pour inclure le nouveau fichier.
        lseek(fd, 0, SEEK_SET);
        if (write(fd, &infos_p, sizeof(infos_partition)) != sizeof(infos_partition))
        {
            perror("Erreur lors de la mise à jour des métadonnées de la partition");
            free(f);
            close(fd);
            return NULL;
        }

        f->numEntete = infos_p.first_file; // Position de l'entête du nouveau fichier.
    }
    else
    {
        // Si le fichier existe déjà, configurez simplement le numéro d'en-tête du fichier.
        f->numEntete = currentOffset;
    }

    return f; // Retourne le pointeur vers la structure file pour le fichier ouvert/créé.
}

int myClose(file *f)
{
    if (f == NULL)
    {
        return -1; // Erreur, fichier invalide
    }

    // Ferme le descripteur de fichier du fichier
    close(f->fd);
    free(f);  // Libère la mémoire allouée pour la structure file
    return 0; // Succès
}
/**
 * Écrit des données dans un fichier.
 *
 * @param f Le pointeur vers la structure file représentant le fichier.
 * @param buffer Le tampon contenant les données à écrire.
 * @param size La taille des données à écrire.
 * @return Le nombre d'octets écrits ou une erreur si la fonction échoue.
 */
int myWrite(file *f, char *buffer, int size)
{
    if (f == NULL || buffer == NULL || size <= 0)
    {
        return ERREURE_INCONNUE;
    }

    int bytesWritten = 0;
    int remaining = size;
    bloc_donnees currentBloc;

    int blocNumber = (f->pos / max_chars_par_bloc) + 1;
    off_t offsetBloc = offset_bloc(f, blocNumber);

    if (offsetBloc == -1)
    {
        currentBloc = allouer_bloc();
        offsetBloc = lseek(f->fd, 0, SEEK_END);
        if (blocNumber == 1)
        {
            set_header(f, offsetBloc);
        }
    }
    else
    {
        lseek(f->fd, offsetBloc, SEEK_SET);
        read(f->fd, &currentBloc, sizeof(bloc_donnees));
    }

    while (remaining > 0)
    {
        int spaceInBloc = max_chars_par_bloc - currentBloc.nb_octets;
        int toWrite = (remaining < spaceInBloc) ? remaining : spaceInBloc;

        memcpy(currentBloc.donnee + currentBloc.nb_octets, buffer + bytesWritten, toWrite);
        currentBloc.nb_octets += toWrite;
        bytesWritten += toWrite;
        remaining -= toWrite;

        lseek(f->fd, offsetBloc, SEEK_SET);
        mySeek(f, 0, SEEK_SET);
        write(f->fd, &currentBloc, sizeof(bloc_donnees));

        if (remaining > 0)
        {
            currentBloc = allouer_bloc();
            off_t newBlocOffset = lseek(f->fd, 0, SEEK_END);
            currentBloc.blocSuivant = newBlocOffset;
            lseek(f->fd, offsetBloc, SEEK_SET);
            write(f->fd, &currentBloc, sizeof(bloc_donnees));
            offsetBloc = newBlocOffset;
            blocNumber++;
        }
    }

    f->pos += bytesWritten;

    int nb_blocs = count_blocs(f);
    if (blocNumber > nb_blocs)
    {
        set_nb_blocs(f, blocNumber);
    }

    return bytesWritten;
}

void mySeek(file *f, int offset, int base)
{
    if (f == NULL)
    {
        return; // Gestion simplifiée de l'erreur
    }

    switch (base)
    {
    case SEEK_SET:
        f->pos = offset;
        break;
    case SEEK_CUR:
        f->pos += offset;
        break;
    case SEEK_END:
    {
        int fileSize = getSizeReelFile(f);
        f->pos = fileSize + offset;
    }
    break;
    default:
        // Gestion des valeurs de base invalides
        printf("Erreur: valeur de 'base' invalide dans mySeek.\n");
        break;
    }
}

int mySize(char *fileName)

{

    char *partitionName = getPartitionName(); // Assume getPartitionName() retourne correctement le nom de la partition.

    if (partitionName == NULL)

    {

        perror("Impossible de récupérer le nom de la partition actuelle");

        return -1;
    }

    int fd = open(partitionName, O_RDONLY); // Ouvre la partition en lecture seule.

    if (fd == -1)

    {

        perror("Erreur lors de l'ouverture du fichier de partition");

        free(partitionName);

        return -1;
    }

    free(partitionName);

    infos_partition infos_p;

    if (lseek(fd, 0, SEEK_SET) == -1 || read(fd, &infos_p, sizeof(infos_partition)) != sizeof(infos_partition))

    {

        perror("Erreur lors du positionnement ou de la lecture des métadonnées de la partition");

        close(fd);

        return -1;
    }

    off_t currentOffset = infos_p.first_file;

    header_bloc_fichier entete;

    while (currentOffset != 0)

    {

        if (lseek(fd, currentOffset, SEEK_SET) == -1 || read(fd, &entete, sizeof(header_bloc_fichier)) != sizeof(header_bloc_fichier))

        {

            perror("Erreur lors du positionnement ou de la lecture d'un bloc d'entête");

            close(fd);

            return -1;
        }

        if (strcmp(entete.nomFichier, fileName) == 0)

        {

            // Fichier trouvé, calculer et retourner sa taille

            int fileSize = 0;

            off_t dataBlockOffset = entete.premierBlocDonnees;

            bloc_donnees dataBlock;

            while (dataBlockOffset != 0)

            {

                if (lseek(fd, dataBlockOffset, SEEK_SET) == -1 || read(fd, &dataBlock, sizeof(bloc_donnees)) != sizeof(bloc_donnees))

                {

                    perror("Erreur lors de la lecture d'un bloc de données");

                    close(fd);

                    return -1;
                }

                fileSize += dataBlock.nb_octets;

                dataBlockOffset = dataBlock.blocSuivant;
            }

            close(fd);

            return fileSize; // Taille du fichier trouvée et calculée avec succès
        }

        currentOffset = entete.fichierSuivant;
    }

    close(fd);

    printf("Le fichier '%s' n'existe pas.\n", fileName);

    return -1; // Le fichier n'existe pas
}

int getSizeReelFile(file *f)
{
    if (f == NULL)
    {
        return ERREURE_INCONNUE; // Gestion simplifiée de l'erreur
    }

    int totalSize = 0;

    off_t currentBlocOffset = get_file_header(f);
    // bloc entete pour lire le premier bloc de données
    header_bloc_fichier entete;
    lseek(f->fd, currentBlocOffset, SEEK_SET);
    read(f->fd, &entete, sizeof(header_bloc_fichier));
    currentBlocOffset = entete.premierBlocDonnees;

    while (currentBlocOffset != -1)
    {
        bloc_donnees bloc;
        lseek(f->fd, currentBlocOffset, SEEK_SET);
        if (read(f->fd, &bloc, sizeof(bloc_donnees)) != sizeof(bloc_donnees))
        {
            // Gestion d'erreur de lecture
            return ERROR_READ;
        }
        totalSize += bloc.nb_octets;
        currentBlocOffset = bloc.blocSuivant;
    }

    return totalSize; // Retourne la taille totale du fichier en termes de caractères
}

int myRead(file *f, void *buffer, int nBytes)
{
    if (f == NULL || buffer == NULL || nBytes <= 0)
    {
        return ERREURE_INCONNUE; // Paramètres invalides.
    }

    char *buf = (char *)buffer; // Cast de buffer pour faciliter la manipulation des données.
    int bytesRead = 0;          // Nombre total d'octets lus.
    int remaining = nBytes;     // Nombre d'octets restants à lire.

    // Assure que la position actuelle est dans une plage valide.
    if (f->pos < 0)
    {
        return ERREURE_INCONNUE; // Position invalide.
    }

    // Calculer le numéro de bloc initial et le décalage au sein de ce bloc basé sur la position actuelle.
    int blocNumber = f->pos / max_chars_par_bloc;
    int offsetInBloc = f->pos % max_chars_par_bloc;

    // Déterminer l'offset du bloc initial dans la partition.
    off_t offsetBloc = offset_bloc(f, blocNumber);
    if (offsetBloc == -1)
    {
        // Gérer le cas où le fichier est vide ou la position est en dehors des données existantes.
        return bytesRead; // Aucune donnée à lire.
    }

    bloc_donnees currentBloc;

    while (remaining > 0 && offsetBloc != -1)
    {
        // Positionner le curseur sur le bloc à lire.
        lseek(f->fd, offsetBloc, SEEK_SET);
        if (read(f->fd, &currentBloc, sizeof(bloc_donnees)) != sizeof(bloc_donnees))
        {
            break; // Échec de la lecture du bloc actuel, arrête la lecture.
        }

        // Calculer le nombre d'octets à lire dans ce bloc.
        int bytesToRead = (remaining < currentBloc.nb_octets - offsetInBloc) ? remaining : currentBloc.nb_octets - offsetInBloc;

        // Copier les données du bloc actuel vers le buffer de l'utilisateur.
        memcpy(buf + bytesRead, currentBloc.donnee + offsetInBloc, bytesToRead);
        bytesRead += bytesToRead;
        remaining -= bytesToRead;

        // Mise à jour de la position dans le fichier.
        f->pos += bytesToRead;

        // Si on a lu tout ce qu'il y avait dans ce bloc, passer au bloc suivant.
        if (offsetInBloc + bytesToRead == currentBloc.nb_octets)
        {
            offsetBloc = currentBloc.blocSuivant; // Passer au bloc suivant.
            offsetInBloc = 0;                     // Réinitialiser le décalage dans le nouveau bloc.
        }
        else
        {
            offsetInBloc += bytesToRead; // Mettre à jour le décalage dans le bloc actuel.
        }
    }

    return bytesRead; // Retourne le nombre total d'octets lus.
}

void visualisation(char *partitionName)
{
    int fd = open(partitionName, O_RDONLY);
    if (fd == -1)
    {
        perror("Erreur lors de l'ouverture de la partition");
        return;
    }

    printf("------------------------------------------\n");
    printf("| Bloc Type | Nom/Info          | Suivant |\n");
    printf("------------------------------------------\n");

    infos_partition mp;
    if (read(fd, &mp, sizeof(mp)) != sizeof(mp))
    {
        perror("Erreur de lecture des métadonnées de la partition");
        close(fd);
        return;
    }

    off_t currentOffset = mp.first_file;
    header_bloc_fichier entete;
    while (currentOffset != 0)
    {
        if (lseek(fd, currentOffset, SEEK_SET) == -1 || read(fd, &entete, sizeof(entete)) != sizeof(entete))
        {
            perror("Erreur de lecture de l'entête du fichier");
            break;
        }

        printf("\033[31m| ENTETE    | %-18s| %-8lld|\033[0m\n", entete.nomFichier, (long long)entete.premierBlocDonnees);
        printf("------------------------------------------\n");

        off_t dataOffset = entete.premierBlocDonnees;
        while (dataOffset != 0)
        {
            bloc_donnees data;
            if (lseek(fd, dataOffset, SEEK_SET) == -1 || read(fd, &data, sizeof(data)) != sizeof(data))
            {
                break;
            }

            printf("\033[37m| DATA      | %-18s| %-8lld|\033[0m\n", data.donnee, (long long)data.blocSuivant);
            printf("------------------------------------------\n");

            dataOffset = data.blocSuivant;
        }

        currentOffset = entete.fichierSuivant;
    }

    close(fd);
}
