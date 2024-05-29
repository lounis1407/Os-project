/**
 * @file myFile.h
 * @brief Ce fichier contient les définitions et les déclarations des structures et des fonctions utilisées dans le projet OS.
 * @authors TAGHELIT Wassim, BOUHADOUN Lounis & HARICHE Yani
 */

#ifndef HEADER_OS_H_INCLUDED
#define HEADER_OS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define NB_FILES_MAX 180
#define ERREURE_INCONNUE -1
#define ERROR_OPEN -2
#define ERROR_READ -3
#define ERROR_WRITE -4
#define ERROR_LSEEK -5
#define max_chars_par_bloc 100

/**
 * @struct file
 * @brief Structure contenant les informations sur un fichier.
 */
typedef struct file
{
    int fd;        /**< Descripteur de fichier = descripteur de la partition */
    int pos;       /**< Pointeur de lecture/écriture */
    int numEntete; /**< Offset vers le bloc d'entête du fichier depuis le début de la partition */
} file;

/**
 * @struct infos_partition
 * @brief Structure contenant les informations sur la partition.
 */
typedef struct
{
    int first_file; /**< Offset du premier bloc d'entête de fichier dans la partition */
    int last_file;  /**< Offset du dernier bloc d'entête de fichier ajouté dans la partition */
} infos_partition;

/**
 * @struct header_bloc_fichier
 * @brief Structure contenant les informations sur un bloc d'entête de fichier.
 */
typedef struct
{
    char nomFichier[20];    /**< Nom du fichier */
    int nb_blocs;           /**< Nombre total de blocs de données pour ce fichier */
    int premierBlocDonnees; /**< Offset vers le premier bloc de données du fichier */
    int fichierSuivant;     /**< Offset vers le bloc d'entête du fichier suivant dans la chaîne */
} header_bloc_fichier;

/**
 * @struct bloc_donnees
 * @brief Structure contenant les données d'un bloc de données.
 */
typedef struct
{
    char donnee[max_chars_par_bloc]; /**< Données du fichier (limitées à `max_chars_par_bloc`) */
    int nb_octets;                   /**< Nombre de caractères (octets) actuellement stockés dans le bloc */
    int blocSuivant;                 /**< Offset vers le prochain bloc de données du fichier */
} bloc_donnees;

/**
 * @brief Alloue un bloc de données vide.
 * @return Le bloc de données alloué.
 * @author Lounis
 */
bloc_donnees allouer_bloc();

/**
 * @brief Obtient le nombre de blocs de données d'un fichier.
 * @param f Le fichier.
 * @return Le nombre de blocs de données.
 * @author Lounis
 */
int count_blocs(file *f);

/**
 * @brief Obtient l'offset du premier bloc de données d'un fichier.
 * @param f Le fichier.
 * @return L'offset du premier bloc de données.
 * @author Lounis
 */
int get_file_header(file *f);

/**
 * @brief Trouve l'offset d'un bloc de données spécifique.
 * @param f Le fichier.
 * @param blocNumber Le numéro du bloc.
 * @return L'offset du bloc de données.
 * @author Lounis
 */
int offset_bloc(file *f, int blocNumber);

/**
 * @brief Définit le nombre de blocs de données d'un fichier.
 * @param f Le fichier.
 * @param val La valeur à définir.
 * @return 0 en cas de succès, une valeur d'erreur sinon.
 * @author Wassim
 */
int set_nb_blocs(file *f, int val);

/**
 * @brief Définit l'offset du premier bloc de données d'un fichier.
 * @param f Le fichier.
 * @param val La valeur à définir.
 * @return 0 en cas de succès, une valeur d'erreur sinon.
 * @author Yani
 */
int set_header(file *f, off_t val);

/**
 * @brief Obtient la taille réelle du fichier en termes de données (nombre de caractères).
 * @param f Le fichier.
 * @return La taille réelle en octets.
 * @author Wassim
 */
int getSizeReelFile(file *f);

/**
 * @brief Définit le nom de la partition.
 * @param partitionName Le nom de la partition.
 * @return 0 en cas de succès, une valeur d'erreur sinon.
 * @author Wassim
 */
int setPartitionName(char *partitionName);

/**
 * @brief Obtient le nom de la partition.
 * @return Le nom de la partition.
 * @author Wassim
 */
char *getPartitionName();

/**
 * @brief Formate ou initialise la partition.
 * @param partitionName Le nom de la partition.
 * @return 0 en cas de succès, une valeur d'erreur sinon.
 * @author Wassim
 */
int myFormat(char *partitionName);

/**
 * @brief Ouvre ou crée un fichier.
 * @param fileName Le nom du fichier.
 * @return Le descripteur de fichier en cas de succès, une valeur d'erreur sinon.
 * @author Wassim
 */
file *myOpen(char *fileName);

/**
 * @brief Écrit des données dans un fichier.
 * @param f Le fichier.
 * @param buffer Le buffer contenant les données à écrire.
 * @param size La taille des données à écrire.
 * @return Le nombre d'octets écrits en cas de succès, une valeur d'erreur sinon.
 * @author Wassim
 */
int myWrite(file *f, char *buffer, int size);

/**
 * @brief Lit des données d'un fichier.
 * @param f Le fichier.
 * @param buffer Le buffer de destination pour les données lues.
 * @param nBytes Le nombre d'octets à lire.
 * @return Le nombre d'octets lus en cas de succès, une valeur d'erreur sinon.
 * @author Yani
 */
int myRead(file *f, void *buffer, int nBytes);

/**
 * @brief Modifie la position de lecture/écriture dans un fichier.
 * @param f Le fichier.
 * @param offset Le décalage à appliquer.
 * @param base La base de décalage (SEEK_SET, SEEK_CUR, SEEK_END).
 * @author Lounis
 */
void mySeek(file *f, int offset, int base);

/**
 * @brief Ferme un fichier.
 * @param f Le fichier.
 * @return 0 en cas de succès, une valeur d'erreur sinon.
 * @author Lounis
 */
int myClose(file *f);

/**
 * @brief Affiche les informations sur la partition.
 * @param partitionName Le nom de la partition.
 * @author Wassim
 */
void visualisation(char *partitionName);

/**
 * @brief calcule la taille d'un fichier.
 * @param fileName Le nom du fichier.
 * @return -1 en cas d'erreur de lecture des blocs de données ou bien d'un fichier qui n'existe pas
 * et retourne la taille du fichier sinon.
 * @author Wassim

*/
int mySize(char *fileName);

#endif // HEADER_OS_H_INCLUDED