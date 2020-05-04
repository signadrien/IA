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

	TTypePion J1P[4];
	TTypePion J2P[4];
	J1P[0]=SPHERE;
	J1P[1]=CYLINDRE;
	J1P[2]=TETRAEDRE;
	J1P[3]=PAVE;

	J2P[0]=SPHERE;
	J2P[1]=CYLINDRE;
	J2P[2]=TETRAEDRE;
	J2P[3]=PAVE;
	TCase J1C[4];
	TCase J2C[4];
	J1C[0].c=A;J1C[0].l=UN;
	J1C[1].c=B;J1C[1].l=DEUX;
	J1C[2].c=C;J1C[2].l=TROIS;
	J1C[3].c=D;J1C[3].l=QUATRE;

	J2C[0].c=D;J2C[0].l=DEUX;
	J2C[0].c=C;J2C[0].l=QUATRE;
	J2C[0].c=A;J2C[0].l=DEUX;
	J2C[0].c=A;J2C[0].l=TROIS;
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
				begin = 1;
				break;
			case 'b':
				Requete.coulPion = BLANC;
				begin = 0;
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
		printf("Changement de couleur :");
		if (Requete.coulPion == BLANC)
		{
			printf("Vous êtes noir\n");
			Requete.coulPion = NOIR;
			begin = 1;
		}
		else
		{
			printf("Vous êtes blanc\n");
			Requete.coulPion = BLANC;
			begin = 0;
		}
	}

	int nbPartie = 1;
	printf("Debut de la partie\n");
	int win = 0;
	int loose = 0;

	int nbCoup=0;
	while (loose < 2 && win < 2)
	{
		TCoupReq RequeteC;
		TCoupReq RequeteAdversaire;
		RequeteC.idRequest = COUP;
		RequeteC.numPartie = nbPartie;
		TPion Pion;
		Pion.coulPion = Requete.coulPion;

		TCoupRep ReponseC;

		// a modifier avec les coups
		if(Requete.coulPion==BLANC){

			Pion.typePion = J1P[nbCoup];
			RequeteC.pion = Pion;
			RequeteC.estBloque = false;
			RequeteC.posPion = J1C[nbCoup++];
			if(nbCoup == 4 ){
				RequeteC.propCoup=GAGNE;
			}
			else{
				RequeteC.propCoup = CONT;
			}

		}
		else{
			Pion.typePion = J2P[nbCoup];
			RequeteC.pion = Pion;
			RequeteC.estBloque = false;
			RequeteC.posPion = J2C[nbCoup++];
			RequeteC.propCoup = CONT;
		}
		/**** VALIDATION *****/
		switch (begin)
		{
		case 0:
			printf("Mon tour\n");
			//ASK MOTEUR NEXT COUP
			err = send(sockServer, &RequeteC, sizeof(TCoupReq), 0);
			printf("COUP ENVOYE\n");
			if (err <= 0)
			{
				perror("(client) erreur sur le send");
				shutdown(sockServer, SHUT_RDWR);
				close(sockServer);
				return -5;
			}
			printf("Validation de mon coup...\n");
			err = recv(sockServer, &ReponseC, sizeof(TCoupRep), 0);
			if (err <= 0)
			{
				perror("(serveurTCP) erreur dans la reception");
				shutdown(sockServer, SHUT_RDWR);
				close(sockServer);
				
				return -6;
			}

			if(ReponseC.err != ERR_OK || ReponseC.validCoup != VALID || ReponseC.propCoup != CONT){
				printf("COUP INVALIDE\n");
				break;
			}
			printf("COUP VALIDE.\n");
			printf("Validation du coup de l'adversaire...\n");

			//Le coup de l'adversaire est valid ?
			err = recv(sockServer, &ReponseC, sizeof(TCoupRep), 0);
			if (err <= 0)
			{
				perror("(serveurTCP) erreur dans la reception");
				shutdown(sockServer, SHUT_RDWR);
				close(sockServer);
				return -6;
			}
			if(ReponseC.err != ERR_OK || ReponseC.validCoup != VALID || ReponseC.propCoup != CONT){
				printf("COUP INVALIDE.\n");
				break;
			}
			printf("COUP VALIDE.\n");
			printf("Reception du coup de l'adversaire...\n");
			err = recv(sockServer, &RequeteAdversaire, sizeof(TCoupReq), 0);
			if (err <= 0)
			{
				perror("(serveurTCP) erreur dans la reception");
				shutdown(sockServer, SHUT_RDWR);
				close(sockServer);
				return -6;
			}
			printf("Coup reçu.");


			break;
		case 1:

			//Le coup de l'adversaire est valid ?
			printf("Validation du coup de l'adversaire...\n");
			err = recv(sockServer, &ReponseC, sizeof(TCoupRep), 0);
			if (err <= 0)
			{
				perror("(serveurTCP) erreur dans la reception");
				shutdown(sockServer, SHUT_RDWR);
				close(sockServer);
				return -6;
			}
			if(ReponseC.err != ERR_OK || ReponseC.validCoup != VALID || ReponseC.propCoup != CONT){
				printf("COUP INVALIDE.\n");
				break;
			}
			printf("COUP VALIDE.\n");
			printf("Reception du coup de l'adversaire...\n");
			err = recv(sockServer, &RequeteAdversaire, sizeof(TCoupReq), 0);
			if (err <= 0)
			{
				perror("(serveurTCP) erreur dans la reception");
				shutdown(sockServer, SHUT_RDWR);
				close(sockServer);
				return -6;
			}
			printf("Coup reçu.");
			//ASK MOTEUR NEXT COUP

			printf("Mon tour\n");
			err = send(sockServer, &RequeteC, sizeof(TCoupReq), 0);
			if (err <= 0)
			{
				perror("(client) erreur sur le send");
				shutdown(sockServer, SHUT_RDWR);
				close(sockServer);
				return -5;
			}
			printf("Validation de mon coup...\n");
			err = recv(sockServer, &ReponseC, sizeof(TCoupRep), 0);
			if (err <= 0)
			{
				perror("(serveurTCP) erreur dans la reception");
				shutdown(sockServer, SHUT_RDWR);
				close(sockServer);
				return -6;
			}
			if(ReponseC.err != ERR_OK || ReponseC.validCoup != VALID || ReponseC.propCoup != CONT){
				printf("COUP INVALIDE.\n");
				break;
			}
			printf("COUP VALIDE.\n");
			break;
			default:
				printf("ERROR on Begin value");
		}

		//Fin d'une partie
		if (ReponseC.propCoup != CONT)
		{
			if (ReponseC.propCoup == GAGNE)
			{
				win++;
			}
			else
			{
				if (ReponseC.propCoup == PERDU)
				{
					loose++;
				}
			}
			if (begin)
			{
				begin--;
				Requete.coulPion=BLANC;
			}
			else
			{
				begin++;
				Requete.coulPion=NOIR;
			}
			nbCoup =0;
			nbPartie++;
		}

		/******* COMM MOTEUR ******/

		/* err = 0;
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
