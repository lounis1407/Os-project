# Makefile pour le projet

# Compilateur
CC = gcc

# Options du compilateur
CFLAGS = -Wall -Wextra -g -Wno-sign-compare

# Fichiers source
SRCS = main.c myFile.c

# Fichiers objets
OBJS = $(SRCS:.c=.o)

# Exécutable
TARGET = test

# Cible par défaut
all: $(TARGET)

# Compilation des fichiers source en fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Lien des fichiers objets en exécutable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Nettoyage des fichiers objets et de l'exécutable
clean:
	rm -f $(OBJS) $(TARGET)

# Règle de génération de la documentation Doxygen, veilliez à lancer la commande "make doc" pour générer la documentation, et dans le cas
# où vous n'avez pas Doxygen d'installé, veuillez l'installer en utilisant la commande "sudo apt-get install doxygen"
doc:
	doxygen -g
	doxygen Doxyfile
