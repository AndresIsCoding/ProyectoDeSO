#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tcp.h"

#define PUERTO 12345
#define BUFFER_SIZE 8192

// Ejecuta un comando y almacena la salida en un buffer
void ejecutarComando(const char *comando, char *salida, int maxSalida) {
    FILE *fp;
    char buffer[128];

    salida[0] = '\0'; // Inicializa la cadena de salida

    // Ejecuta el comando y abre un flujo de lectura
    fp = popen(comando, "r");
    if (fp == NULL) {
        snprintf(salida, maxSalida, "Error al ejecutar comando\n");
        return;
    }

    // Lee la salida del comando y la almacena en 'salida'
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (strlen(salida) + strlen(buffer) < maxSalida) {
            strcat(salida, buffer);
        } else {
            break;
        }
    }

    // Verifica si el comando no produjo ninguna salida
    if (strlen(salida) == 0) {
        snprintf(salida, maxSalida, "Comando no reconocido\n");
    }

    pclose(fp); // Cierra el flujo de lectura del comando.
}

// Procesa los comandos recibidos del cliente
void procesarComandos(int cliente_socket) {
    char buffer[BUFFER_SIZE];
    char salida[BUFFER_SIZE];
    int primerMensaje = 1; // Indica si es el primer mensaje.

    while (1) {
        memset(buffer, 0, BUFFER_SIZE); // Limpiar buffer antes de leer
        TCP_Read_String(cliente_socket, buffer, BUFFER_SIZE);

        // Verifica si es el primer mensaje para enviar el mensaje inicial
        if (primerMensaje) {
            printf("Mensaje inicial del cliente: %s\n", buffer);
            primerMensaje = 0; // Desactivar la bandera después del primer mensaje
            continue; // Continuar con el siguiente ciclo sin ejecutar este mensaje
        }

        // Cierra la conexión si recibe el comando 'salida'
        if (strcmp(buffer, "salida\n") == 0) {
            printf("Cerrando conexión con el cliente.\n");
            char mensajeDespedida[] = "Cerrando conexión. ¡Hasta luego!\n";
            TCP_Write_String(cliente_socket, mensajeDespedida);
            break;
        }

        // Ejecuta el comando recibido y envía la salida al cliente
        printf("Comando recibido: %s\n", buffer);
        ejecutarComando(buffer, salida, BUFFER_SIZE);
        TCP_Write_String(cliente_socket, salida); // Enviar la salida antes de limpiar el buffer
    }
}

// Función main
int main() {
    t_socket servidor_socket, cliente_socket;

    // Abre un socket para el servidor
    servidor_socket = TCP_Server_Open(PUERTO);
    printf("Servidor iniciado en el puerto %d\n", PUERTO);

    while (1) {
        cliente_socket = TCP_Accept(servidor_socket);
        printf("Cliente conectado.\n");

        // Procesa los comandos del cliente
        procesarComandos(cliente_socket);

        TCP_Close(cliente_socket);
    }

    // Cierra el socket del servidor
    TCP_Close(servidor_socket);
    return 0;
}


