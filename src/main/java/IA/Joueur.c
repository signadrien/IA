#include "fonctionTCP.h"
#include "ProtoQuantik.h"

#define T_BUF 20

int main(int argc, char **argv)
{

	if (argc != 2)
	{
		printf("usage : %s port\n", argv[0]);
		return -1;
	}

	/*
	*
	* SERVER avec MOTEURIA
	*
	*/

	/*
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
	*/

	/*
	*
	* CLIENT avec VALIDATION
	*
	*/
	int port = atoi(argv[1]);
	char chaine[T_BUF];
	int sockServer = socketClient("127.0.0.1", port);
	if (sockServer <= 0)
	{
		perror("(client) Erreur dans la connexion avec le serveur");
		return -1;
	}
	int continuer = 1;
	char stop[T_BUF];
	int err = 0;

	TPartieReq Requete;
	short begin;
	TPartieRep Reponse;
	char color[T_BUF];

	do
	{
		do
		{
			printf("Saisir votre Pseudo (taille <= 20) :\n");
			scanf("%s", Requete.nomJoueur);
			printf("Saisir la couleur souhaitée : (n/b)\n");
			scanf("%s", color);
			switch (color[0])
			{
			case 'n':
				Requete.coulPion = NOIR;
				break;
			case 'b':
				Requete.coulPion = BLANC;
				break;
			default:
				printf("La couleur n'est pas possible.\n");
				err = 1;
			}
			Requete.idReq = PARTIE;

		} while (err);

		err = send(sockServer, &Requete, sizeof(TPartieReq), 0);
		if (err <= 0)
		{
			perror("(client) erreur sur le send");
			shutdown(sockServer, SHUT_RDWR);
			close(sockServer);
			return -5;
		}

		err = recv(sockServer, &Reponse, sizeof(TPartieRep), 0);
		if (err <= 0)
		{
			perror("(serveurTCP) erreur dans la reception");
			shutdown(sockServer, SHUT_RDWR);
			close(sockServer);
			return -6;
		}

	} while (Reponse.err != ERR_OK);

	if (Reponse.validCoulPion == KO)
	{
		if (Requete.coulPion == BLANC)
		{
			Requete.coulPion = NOIR;
			begin = 0;
		}
		else
		{
			Requete.coulPion = BLANC;
			begin = 1;
		}
	}

	int end = 0;

	while (end != 2)
	{
		TCoupReq RequeteC;

		TCoupRep ReponseC;

		/**** VALIDATION *****/
		if (begin)
		{
			//ASK MOTEUR NEXT COUP
			err = send(sockServer, &RequeteC, sizeof(TCoupReq), 0);
			if (err <= 0)
			{
				perror("(client) erreur sur le send");
				shutdown(sockServer, SHUT_RDWR);
				close(sockServer);
				return -5;
			}
			err = recv(sockServer, &ReponseC, sizeof(TCoupRep), 0);
			if (err <= 0)
			{
				perror("(serveurTCP) erreur dans la reception");
				shutdown(sockServer, SHUT_RDWR);
				close(sockServer);
				return -6;
			}
		}
		else {
			err = recv(sockServer, &ReponseC, sizeof(TCoupRep), 0);
			if (err <= 0)
			{
				perror("(serveurTCP) erreur dans la reception");
				shutdown(sockServer, SHUT_RDWR);
				close(sockServer);
				return -6;
			}
			//ASK MOTEUR NEXT COUP
			err = send(sockServer, &RequeteC, sizeof(TCoupReq), 0);
			if (err <= 0)
			{
				perror("(client) erreur sur le send");
				shutdown(sockServer, SHUT_RDWR);
				close(sockServer);
				return -5;
			}
		}

		if (ReponseC.propCoup != CONT)
		{
			if (begin)
			{
				begin--;
			}
			else
			{
				begin++;
			}
			end++;
		}

		/******* COMM MOTEUR ******/

		/*
		
		/*err = 0;
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
			return -4;*/
		}
		*/
	}

	/**** MOTEUR ***/
	/*
	shutdown(sockTrans, SHUT_RDWR); 
  close(sockTrans);
	close(sockServ);
	return 0;
	*/
}
