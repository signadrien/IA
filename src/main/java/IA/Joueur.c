#include "fonctionTCP.h"

int main(int argc, char** argv) {
	
	if (argc != 2) {
		printf ("usage : %s port\n", argv[0]);
		return -1;
	}
	
	int	sockTrans;
	int	port = atoi(argv[1]);
	int	sizeAddr;
	int	err;
	int	fin = 0;
	int	entier1;
	int	entier2;
	int	res;
	int	op;
	char ope;
	struct sockaddr_in addClient;
	
	int sockServ = socketServeur(port);
	if (sockServ < 0){
		printf("Erreur sur socket serveur\n");
		return -2;
	}

	sizeAddr = sizeof(struct sockaddr_in);
	sockTrans = accept(sockServ, (struct sockaddr *)&addClient, (socklen_t *)&sizeAddr);
	if (sockTrans < 0) {
		perror("(serveurTCP) erreur sur accept");
		return -3;
	}
	
	while(!fin){
		
		err = 0;
		while(err<4) {
			err = recv(sockTrans, &entier1, sizeof(int), MSG_PEEK);
		}
		err = recv(sockTrans, &entier1, sizeof(int), 0);
		
		if (err <= 0) {
			perror("(serveurTCP) erreur dans la reception");
			shutdown(sockTrans, SHUT_RDWR); close(sockTrans);
			return -4;
		}
		entier1 = ntohl(entier1);

		err = 0;
		while(err<4) {
			err = recv(sockTrans, &op, sizeof(int), MSG_PEEK);
		}
		err = recv(sockTrans, &op, sizeof(int), 0);
		
		if (err <= 0) {
			perror("(serveurTCP) erreur dans la reception");
			shutdown(sockTrans, SHUT_RDWR); close(sockTrans);
			return -4;
		}
		
		op = ntohl(op);
		ope = (char) op;

		err = 0;
		while(err<4) {
		 err = recv(sockTrans, &entier2, sizeof(int), MSG_PEEK);
		}
		err = recv(sockTrans, &entier2, sizeof(int), 0);
		
		if (err <= 0) {
			perror("(serveurTCP) erreur dans la reception");
			shutdown(sockTrans, SHUT_RDWR); close(sockTrans);
			return -4;
		}
		
		entier2 = ntohl(entier2);

		switch (ope) {
			case '+':
				res = entier1 + entier2;
				printf("%d + %d = %d\n",entier1,entier2,res);
				break;
			case '-':
				res = entier1 - entier2;
				printf("%d - %d = %d\n",entier1,entier2,res);
				break;
			case '*':
				res = entier1*entier2;
				printf("%d * %d = %d\n",entier1,entier2,res);
				break;
			case '/':
				res = (int)(entier1/entier2);
				printf("%d / %d = %d\n",entier1,entier2,res);
				break;
						      
			default:
				res = -1;
				break;
		}
		res = htonl(res);
		err = send(sockTrans,&res,sizeof(int),0);
		res = ntohl(res);
		
		err = 0;
		while(err<4) {
			err = recv(sockTrans, &fin, sizeof(int), MSG_PEEK);
		}
		err = recv(sockTrans, &fin, sizeof(int), 0);
		fin = ntohl(fin);
		if (err <= 0) {
			perror("(serveurTCP) erreur dans la reception");
			shutdown(sockTrans, SHUT_RDWR); close(sockTrans);
			return -4;
		}
	}
 
	shutdown(sockTrans, SHUT_RDWR); 
  close(sockTrans);
	close(sockServ);
	return 0;
}
