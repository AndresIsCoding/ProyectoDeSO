#ifndef __TCP_h__
#define __TCP_h__
#define SA struct sockaddr 
#define t_socket int
#define t_port int
#define buffer_size int
// Crea un socket dado un nombre de maquina y un numero de puerto
t_socket TCP_Open(char*,t_port);
// Crea un socket dado un numero IP y un numero de puerto
t_socket TCP_Open_By_IP(char*,t_port);
// Crea un socket servidor dado un num. de puerto
t_socket TCP_Server_Open(t_port);
// Espera por conexiones sobre el socket indiciado 
t_socket TCP_Accept(t_socket);
// Escribe una cadena dada sobre el socket indicado
int TCP_Write_String(t_socket, char*);
// Lee una cadena del socket indicado en el buffer indicado 
int TCP_Read_String(t_socket, char*,buffer_size);
// Envia un archivo indicado por el socket suministrado
int TCP_Send_File(t_socket, char*);
// Recibe un archivo indicado por el socket suministrado
int TCP_Recv_File(t_socket, char*);
// Cierra conexion TCP por el socket indicado
int TCP_Close(t_socket);
// Retorna un IP dado un nombre de maquina
char *Get_IP(char *);
// Envia un ack por el socket indicado
int Send_ACK(t_socket);
// Espera un ack por el socket indicado
int Recv_ACK(t_socket);
#define TCP_ACK "ACK\x0"
#define MAX_TCP_ACK 4
#define TCP_FIN "__FIN__\x0"
#define TCP_FIN_FILE "__FIN__FILE__\x0"
#endif