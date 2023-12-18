#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <netinet/tcp.h> 
#include <stdlib.h>
#include <string.h> 
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h> 
#include "tcp.h"

int TCP_Open(char *hostname, t_port port) {
  return TCP_Open_By_IP(Get_IP(hostname),port);
}

// Esta es la funcion que usa un cliente para conectarse a un 
// servidor.
int TCP_Open_By_IP(char *hostip, t_port port) {
	int sockfd; 
	struct sockaddr_in servaddr; 

	// socket create and varification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr(hostip); 
	servaddr.sin_port = htons(port); 

	// connect the client socket to server socket 
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n"); 
		exit(0); 
	} 
	else
		printf("connected to the server..\n"); 

	return sockfd;
}

// Esta funcion es usada para crear un servidor de socket.
// El usuario pasa el numero de puerto donde desea que este servidor
// sea accedido.
int TCP_Server_Open(t_port port) {
	int sockfd; 
	struct sockaddr_in servaddr; 

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(port); 

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n"); 

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 5)) != 0) { 
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server listening..\n"); 

        return sockfd;
}

// Esta funcion espera por conexiones de red desde un cliente
// Devuelve un socket que es el canal de comunicacion entre el 
// servidor y el cliente que solicita la conexion
int TCP_Accept(t_socket socket) {
	struct sockaddr_in servaddr, cli; 
	int len = sizeof(cli); 
	int connfd;

	printf("Server accepting connections..\n");
	// Accept the data packet from client and verification 
	connfd = accept(socket, (SA*)&cli, &len); 
	if (connfd < 0) { 
		printf("server acccept failed...\n"); 
		exit(0); 
	} 
	else
		printf("server acccept the client...\n"); 

        return connfd;
}

int TCP_Write_String(t_socket socket, char* string) {
	int len = strlen(string)*sizeof(char);
	int written;
	int flag = 1;
	written = write(socket,string, len);
	if (len != written) {
		printf("No se enviaron todos los datos %d/%d\n",written,len);
	}
	return written;
}

int TCP_Read_String(t_socket socket, char* string,int maxstring) {
	int _read;
	_read = read(socket,string,maxstring);

	return _read;
}

// Codigo tomado de 
// https://idiotdeveloper.com/file-transfer-using-tcp-socket-in-c/
int TCP_Send_File(t_socket socket, char* filename) {
	int n;
	char buffer[BUFSIZ] = {0};
	FILE *fp;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "Fallo en abrir archivo (TCP_Send_File) %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	while (fgets(buffer, BUFSIZ, fp) != NULL) {
		int n;
		if ((n = send(socket, buffer, sizeof(buffer), 0)) == -1) {
			fprintf(stderr, "Fallo en enviar datos archivo (TCP_Send_File) %s\n",strerror(errno));
			exit(EXIT_FAILURE);
		}
		bzero(buffer,BUFSIZ);
	}
	strcpy(buffer,TCP_FIN_FILE);
	send(socket,buffer,sizeof(buffer),0);
	fclose(fp);
	
	return 0;
}

// Codigo tomado de 
// https://idiotdeveloper.com/file-transfer-using-tcp-socket-in-c/
int TCP_Recv_File(t_socket socket, char* filename) {
	FILE *fp;
	int n;
	char buffer[BUFSIZ];

	fp = fopen(filename, "w");
	if (fp == NULL) {
		fprintf(stderr, "Fallo en abrir archivo (TCP_Recv_File) %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	while (1) {
		n = recv(socket, buffer, BUFSIZ, 0);
		if (n <= 0) break;
		if (!strcmp(TCP_FIN_FILE,buffer)) break;
		fprintf(fp,"%s",buffer);
	}
	fflush(fp); fclose(fp);

	return 0;
}

int TCP_Close(t_socket sck) {
	close(sck);
}

char *Get_IP(char *hostname) {
	struct hostent *host_entry;

	host_entry = gethostbyname(hostname);
	if (host_entry == NULL) {
		printf("host_entry failed..");
		return NULL;	
	}
	return inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));
}

int Send_ACK(t_socket socket) {
  TCP_Write_String(socket,TCP_ACK);
}

int Recv_ACK(t_socket socket) {
  char ack[MAX_TCP_ACK] = {0};
  TCP_Read_String(socket,ack,MAX_TCP_ACK);
  return !strcmp(ack,TCP_ACK);
}