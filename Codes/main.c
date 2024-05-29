/**
 * @file main.c
 * @brief Programme principal pour la gestion de fichiers.
 * @authors Wassim & Lounis
 */

#include "myFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Fonction principale du programme.
 * @return 0 en cas de succès.
 */
int main()
{
    file *f = NULL;          /**< Pointeur vers le fichier ouvert */
    char partitionName[256]; /**< Nom de la partition */
    char fileName[256];      /**< Nom du fichier */
    char buffer[1000];       /**< Tampon de données */
    int choice;              /**< Choix de l'utilisateur */
    int nBytes;              /**< Nombre d'octets */
    int offset;              /**< Décalage */
    int seekBase;            /**< Base de déplacement */
    strcpy(partitionName, getPartitionName());
    while (1)
    {
        printf("\nMenu :\n");
        printf("1. Formater la partition\n");
        printf("2. Ouvrir un fichier\n");
        printf("3. Écrire dans le fichier\n");
        printf("4. Lire le fichier\n");
        printf("5. Déplacer le curseur de position fichier\n");
        printf("6. Taille du fichier\n");
        printf("7. Fermer le fichier\n");
        printf("8. Visualiser la partition\n");
        printf("9. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choice);
        getchar(); // Consume newline character left in input buffer

        switch (choice)
        {
        case 1: // Formater
            printf("Nom de la partition : ");
            scanf("%s", partitionName);
            if (myFormat(partitionName) == 0)
            {
                printf("Partition formatée avec succès.\n");
            }
            else
            {
                printf("Erreur lors du formatage de la partition.\n");
            }
            break;

        case 2: // Ouvrir
            printf("Nom du fichier : ");
            scanf("%s", fileName);
            f = myOpen(fileName);
            if (f)
            {
                printf("Fichier '%s' ouvert avec succès.\n", fileName);
            }
            else
            {
                printf("Erreur lors de l'ouverture du fichier '%s'.\n", fileName);
            }
            break;

        case 3: // Écrire
            printf("Données à écrire :");

            if (f)
            {
                fgets(buffer, sizeof(buffer), stdin);
                buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing newline character
                printf("Taille des données introduites : %ld\n", strlen(buffer));
                nBytes = myWrite(f, buffer, strlen(buffer));
                if (nBytes >= 0)
                {
                    printf("%d octets écrits dans le fichier.\n", nBytes);
                    // Déplacer la position de lecture au début du fichier

                    // Consume newline character
                }
                else
                {
                    printf("Erreur lors de l'écriture dans le fichier.\n");
                }
            }
            else
            {
                printf("Aucun fichier ouvert.\n");
            }
            break;

        case 4: // Lire
            if (f)
            {
                printf("Nombre d'octets à lire : ");
                scanf("%d", &nBytes);

                // Consume newline character
                // Réinitialiser le tampon de données
                int nByes = myRead(f, buffer, nBytes);
                if (nByes >= 0)
                {
                    printf("Données lues (%d octets) : \"%s\"\n", nBytes, buffer);
                    for (int i = 0; i < nByes; i++)
                    {
                        printf("%c", buffer[i]);
                    }
                }
                else
                {
                    printf("Erreur lors de la lecture du fichier.\n");
                }
            }
            else
            {
                printf("Aucun fichier ouvert.\n");
            }
            break;

        case 5: // Se positionner dans le fichier (Seek)
            if (f)
            {
                printf("Offset : ");
                scanf("%d", &offset);
                printf("Base (0: début du fichier, 1: position courrante, 2: fin du fichier) : ");
                scanf("%d", &seekBase);
                switch (seekBase)
                {
                case 0:
                    seekBase = SEEK_SET;
                    break;
                case 1:
                    seekBase = SEEK_CUR;
                    break;
                case 2:
                    seekBase = SEEK_END;
                    break;
                default:
                    printf("Base invalide.\n");
                    continue;
                }
                mySeek(f, offset, seekBase);
                printf("Position de lecture déplacée avec succès %d. \n", f->pos);
            }
            else
            {
                printf("Aucun fichier ouvert.\n");
            }
            break;

        case 6: // taille du fichier
            if (f)
            {
                printf("La taille du fichier est : %d\n", mySize(fileName));
            }
            else
                printf("Aucun fichier ouvert.\n");
            break;
        case 7: // Fermer
            if (f)
            {
                if (myClose(f) == 0)
                {
                    printf("Fichier fermé avec succès.\n");
                }
                else
                {
                    printf("Erreur lors de la fermeture du fichier.\n");
                }
                f = NULL; // on remet l'instance du fichier a null
            }
            else
            {
                printf("Aucun fichier ouvert.\n");
            }
            break;

        case 8: // visialiser la partition
            visualisation(partitionName);
            break;

        case 9: // Quitter
            printf("Fin du programme.\n");
            // Si un fichier est encore ouvert, le fermer avant de quitter
            if (f)
            {
                myClose(f);
            }
            return 0;

        default:
            printf("Choix non valide.\n");
        }
    }

    return 0;
}