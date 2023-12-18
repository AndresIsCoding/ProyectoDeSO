#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tcp.h" // Se lo incluye para el manejo del cliente servidor

#define BUFFER_SIZE 8192 // Tamaño del buffer para comandos y respuestas

// Función para enviar comandos al servidor y manejar respuestas
void enviarComandos(int sockfd) {
    char buffer[BUFFER_SIZE];
    char comando[BUFFER_SIZE];

    // Enviar mensaje inicial al servidor
    strcpy(buffer, "¡Hola! ¿Listo para recibir comandos?\n");
    TCP_Write_String(sockfd, buffer);

    // Bucle para leer y enviar comandos
    while (1) {
        printf("Introduce un comando (o 'salida' para terminar): ");
        fgets(comando, BUFFER_SIZE, stdin); // Leer comando del usuario
        TCP_Write_String(sockfd, comando); // Enviar comando al servidor

        // Comprobar si el cliente quiere salir
        if (strncmp(comando, "salida", 6) == 0) {
            memset(buffer, 0, BUFFER_SIZE); // Limpiar buffer
            TCP_Read_String(sockfd, buffer, BUFFER_SIZE); // Recibir mensaje final del servidor
            printf("Respuesta del servidor: %s\n", buffer);
            break; // Salir del bucle
        }

        // Recibe y muestra la respuesta del servidor
        memset(buffer, 0, BUFFER_SIZE); // Limpia el buffer
        TCP_Read_String(sockfd, buffer, BUFFER_SIZE); // Lee la respuesta del servidor
        printf("Respuesta del servidor: %s\n", buffer);
    }
}

// Función que establece conexión y controla el flujo del programa
int main(int argc, char* argv[]) {
    int sockfd, port;

    // Verificar argumentos de línea de comandos
    if (argc != 3) {
        printf("Uso: %s <host> <puerto>\n", argv[0]);
        return 1;
    }

    port = atoi(argv[2]); // Convertir argumento de puerto a entero
    printf("Conectando a %s en el puerto %d...\n", argv[1], port);
    sockfd = TCP_Open(argv[1], port); // Establecer conexión con el servidor

    enviarComandos(sockfd); // Enviar comandos y maneja comunicación
    TCP_Close(sockfd); // Cierra la conexion 
    return 0;
}