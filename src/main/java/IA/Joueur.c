#include "fonctionTCP.h"
#include "ProtoQuantik.h"

#define T_BUF 20


int ReqToInt(TCoupReq *requete){
	int res = 0;
	switch(requete->pion.typePion){
		case SPHERE :
			res+=2;
		break;
		case PAVE :
			res+=1;
		break;
		case TETRAEDRE :
			res+=3;
		break;
		case CYLINDRE :
			res+=0;
		break;
		default :
			return 1000;
	}
	switch(requete->posPion.c){
		case A :
			res+=00;
		break;
		case B :
			res+=10;
		break;
		case C :
			res+=20;
		break;
		case D :
			res+=30;
		break;
		default : 
			return 1000;
	}
	switch(requete->posPion.l){
		case UN :
			res+=000;
		break;
		case DEUX :
			res+=100;
		break;
		case TROIS :
			res+=200;
		break;
		case QUATRE :
			res+=300;
		break;
		default : 
			return 1000;
	}
	return res;
}


void IntToReq(TCoupReq *res, int requete){
	if(requete>1000){
		res->propCoup=GAGNE;
	}
	else{
		res->propCoup = CONT;
	}
	if(requete<0){
		res->propCoup = NUL;
		requete*=-1;
	}
	res->pion.typePion = (TTypePion)requete%10;
	requete/=10;
	res->posPion.c  = requete%10;
	requete/=10;
	res->posPion.l =  requete;
}

int main(int argc, char **argv)
{

	if (argc != 2)
	{
		printf("usage : %s port\n", argv[0]);
		return -1;
	}

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
	J1C[1].c=A;J1C[1].l=DEUX;
	J1C[2].c=A;J1C[2].l=TROIS;
	J1C[3].c=A;J1C[3].l=QUATRE;

	J2C[0].c=D;J2C[0].l=QUATRE;
	J2C[1].c=D;J2C[1].l=TROIS;
	J2C[2].c=D;J2C[2].l=DEUX;
	J2C[3].c=D;J2C[3].l=UN;
	int port = atoi(argv[1]);
	int sockServer = socketClient("127.0.0.1", port);
	if (sockServer <= 0)
	{
		perror("(client) Erreur dans la connexion avec le serveur");
		return -1;
	}


//JAVA C

	int	sockJava;
	struct sockaddr_in addClient;
	
	int sockServ;
	do {
		sockServ = socketServeur(++port);
	} while(sockServ < 0);

	int sizeAddr = sizeof(struct sockaddr_in);
	sockJava = accept(sockServ, (struct sockaddr *)&addClient, (socklen_t *)&sizeAddr);
	if (sockJava < 0) {
		perror("(serveurTCP) erreur sur accept");
		return -3;
	}

//END JAVA C

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
				err =0;
				break;
			case 'b':
				Requete.coulPion = BLANC;
				begin = 0;
				err =0;
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

	int nbCoup=0;
	TPion Pion;

			Pion.coulPion = Requete.coulPion;
	int nbtour =0;
	TCoupReq RequeteTest;
	while (nbPartie <= 2)
	{
		TCoupReq RequeteC;
		TCoupReq RequeteAdversaire;
		RequeteC.idRequest = COUP;
		RequeteC.numPartie = nbPartie;
		

		TCoupRep ReponseC;

		// a modifier avec les coups
		if(begin==0){

			Pion.typePion = J1P[nbCoup];
			
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
			
			RequeteC.estBloque = false;
			RequeteC.posPion = J2C[nbCoup++];
			RequeteC.propCoup = CONT;
		}
			RequeteC.pion = Pion;
		/**** VALIDATION *****/
		switch (begin)
		{
		case 0:
			printf("Mon tour\n");
			if(nbtour==0){
				int res = htonl(-1000);
				err = send(sockJava,&res,sizeof(int),0);
				res = ntohl(res);
				nbtour++;
			}
			//ASK MOTEUR NEXT COUP

			int coup =-1;
			err = 0;
			while(err<4) {
				err = recv(sockJava, &coup, sizeof(int), MSG_PEEK);
			}
			err = recv(sockJava, &coup, sizeof(int), 0);

			IntToReq(&RequeteTest,coup);

			//fin moteur

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
			int coupTest = ReqToInt(&RequeteAdversaire);
				int res = htonl(coupTest);
				err = send(sockJava,&res,sizeof(int),0);
				res = ntohl(res);

			printf("Coup reçu. Code : %d\n",coupTest);


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
			printf("ERROK ? %d\nVALID ? %d\nPROPCOUP ? %d\n",ReponseC.err,ReponseC.validCoup,ReponseC.propCoup);
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

				coupTest = ReqToInt(&RequeteAdversaire);
				res = htonl(coupTest);
				err = send(sockJava,&res,sizeof(int),0);
				res = ntohl(res);
			printf("Coup reçu.\n");
			//ASK MOTEUR NEXT COUP

			coup =-1;
			err = 0;
			while(err<4) {
				err = recv(sockJava, &coup, sizeof(int), MSG_PEEK);
			}
			err = recv(sockJava, &coup, sizeof(int), 0);

			IntToReq(&RequeteTest,coup);

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
		printf("%d",ReponseC.propCoup);
		if (ReponseC.propCoup != CONT)
		{
			nbtour=0;
			int res = htonl(1000);
			err = send(sockJava,&res,sizeof(int),0);
			res = ntohl(res);
			printf("FIN DE PARTIE \n\n");
			if (begin==1)
			{
				begin--;
			}
			else
			{
				begin++;
			}
			nbCoup =0;
			nbPartie++;
		}
	}

	/**** MOTEUR IA ***/

	shutdown(sockJava, SHUT_RDWR); 
  	close(sockJava);
	close(sockServ);
	return 0;

}
