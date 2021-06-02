#ifndef AUXLIB_H
#define AUXLIB_H

// LINUX ///////////////////////////////////////////////////////////////////////

#ifdef __unix__
#endif

// WINDOWS /////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#endif

// AMBOS ///////////////////////////////////////////////////////////////////////

/* DEBUG
 * 1 para mensajes extra sobre el funcionamiento del programa
 * 0 para desactivar */
#define DEBUG 0

// PROTOTIPOS //////////////////////////////////////////////////////////////////

/* Elimina el buffer de stdin */
void clearStdIn();
/* Ejecuta un comando para limpiar la consola */
void clearConsole();
/* Pide presionar ENTER para continuar */
void pauseProgram();

#endif
