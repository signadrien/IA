#include "fonctionTCP.h"
#include "ProtoQuantik.h"

#define T_BUF 20

int ReqToInt(TCoupReq *requete)
{
	int res = 0;
	switch (requete->pion.typePion)
	{
	case SPHERE:
		res += 2;
		break;
	case PAVE:
		res += 1;
		break;
	case TETRAEDRE:
		res += 3;
		break;
	case CYLINDRE:
		res += 0;
		break;
	default:
		return 1000;
	}
	switch (requete->posPion.c)
	{
	case A:
		res += 00;
		break;
	case B:
		res += 10;
		break;
	case C:
		res += 20;
		break;
	case D:
		res += 30;
		break;
	default:
		return 1000;
	}
	switch (requete->posPion.l)
	{
	case UN:
		res += 000;
		break;
	case DEUX:
		res += 100;
		break;
	case TROIS:
		res += 200;
		break;
	case QUATRE:
		res += 300;
		break;
	default:
		return 1000;
	}
	return res;
}

void IntToReq(TCoupReq *res, int requete, TPion *pion, TCase *posPion)
{
	if (requete >= 1000)
	{
		res->propCoup = GAGNE;
		requete -= 1000;
	}
	else
	{
		res->propCoup = CONT;
	}
	if (requete < 0)
	{
		res->propCoup = NUL;
		requete *= -1;
	}
	pion->typePion = (TTypePion)requete % 10;
	requete /= 10;
	posPion->c = requete % 10;
	requete /= 10;
	posPion->l = requete;
}

int main(int argc, char **argv)
{

	if (argc != 6)
	{
		printf("usage : %s adresse port port2 nomJoueur couleur\n", argv[0]);
		return -1;
	}

	int port = atoi(argv[2]);
	int sockServer = socketClient(argv[1], port);
	if (sockServer <= 0)
	{
		perror("(client) Erreur dans la connexion avec le serveur");
		return -1;
	}

	//JAVA C

	int sockJava;
	struct sockaddr_in addClient;

	int sockServ;
		sockServ = socketServeur(atoi(argv[3]));

	int sizeAddr = sizeof(struct sockaddr_in);
	sockJava = accept(sockServ, (struct sockaddr *)&addClient, (socklen_t *)&sizeAddr);
	if (sockJava < 0)
	{
		perror("(serveurTCP) erreur sur accept");
		return -3;
	}

	//END JAVA C

	int err = 0;
	TPartieReq Requete;
	short begin;
	TPartieRep Reponse;
	char* color = argv[5];
	char* nom = argv[4];
	memcpy(Requete.nomJoueur, nom, T_NOM);
	switch (color[0])
	{
	case 'n':
		Requete.coulPion = NOIR;
		begin = 1;
		err = 0;
		break;
	case 'b':
		Requete.coulPion = BLANC;
		begin = 0;
		err = 0;
		break;
	default:
		printf("La couleur n'est pas possible.\n");
		err = 1;
	}
	Requete.idReq = PARTIE;

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

	if (Reponse.validCoulPion == KO)
	{
		if (Requete.coulPion == BLANC)
		{
			Requete.coulPion = NOIR;
			begin = 1;
		}
		else
		{
			Requete.coulPion = BLANC;
			begin = 0;
		}
	}

	int nbPartie = 1;
	TPion Pion;

	Pion.coulPion = Requete.coulPion;
	int nbtour = 0;
	TCase posPion;
	TCoupReq RequeteC;
	TCoupReq RequeteAdversaire;
	RequeteC.idRequest = COUP;
	RequeteC.estBloque = false;
	TCoupRep ReponseC;
	while (nbPartie <= 2)
	{
		RequeteC.numPartie = nbPartie;

		/**** VALIDATION *****/
		switch (begin)
		{
		case 0:
			if (nbtour == 0)
			{
				int res = htonl(-1000);
				err = send(sockJava, &res, sizeof(int), 0);
				res = ntohl(res);
				nbtour++;
			}
			//ASK MOTEUR NEXT COUP

			int coup = -1;
			err = 0;
			while (err < 4)
			{
				err = recv(sockJava, &coup, sizeof(int), MSG_PEEK);
			}
			err = recv(sockJava, &coup, sizeof(int), 0);
			coup = ntohl(coup);
			IntToReq(&RequeteC, coup, &Pion, &posPion);
			RequeteC.pion = Pion;
			RequeteC.posPion = posPion;
			//fin moteur

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

			if (ReponseC.err != ERR_OK || ReponseC.validCoup != VALID || ReponseC.propCoup != CONT)
			{
				break;
			}

			//Le coup de l'adversaire est valid ?
			err = recv(sockServer, &ReponseC, sizeof(TCoupRep), 0);
			if (err <= 0)
			{
				perror("(serveurTCP) erreur dans la reception");
				shutdown(sockServer, SHUT_RDWR);
				close(sockServer);
				return -6;
			}
			if (ReponseC.err != ERR_OK || ReponseC.validCoup != VALID || ReponseC.propCoup != CONT)
			{
				break;
			}
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
			err = send(sockJava, &res, sizeof(int), 0);
			res = ntohl(res);

			break;
		case 1:
			//Le coup de l'adversaire est valid ?
			err = recv(sockServer, &ReponseC, sizeof(TCoupRep), 0);
			if (err <= 0)
			{
				perror("(serveurTCP) erreur dans la reception");
				shutdown(sockServer, SHUT_RDWR);
				close(sockServer);
				return -6;
			}
			if (ReponseC.err != ERR_OK || ReponseC.validCoup != VALID || ReponseC.propCoup != CONT)
			{
				break;
			}
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
			err = send(sockJava, &res, sizeof(int), 0);
			res = ntohl(res);
			//ASK MOTEUR NEXT COUP

			coup = -1;
			err = 0;
			while (err < 4)
			{
				err = recv(sockJava, &coup, sizeof(int), MSG_PEEK);
			}
			err = recv(sockJava, &coup, sizeof(int), 0);
			coup = ntohl(coup);
			printf("%d\n", coup);
			IntToReq(&RequeteC, coup, &Pion, &posPion);
			RequeteC.pion = Pion;
			RequeteC.posPion = posPion;
			printf("PROP :%d, colonne : %d,Ligne : %d, TYPE : %d\n", RequeteC.propCoup, RequeteC.posPion.c, RequeteC.posPion.l, RequeteC.pion.typePion);
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
			if (ReponseC.err != ERR_OK || ReponseC.validCoup != VALID || ReponseC.propCoup != CONT)
			{
				break;
			}
			break;
		default:
			printf("ERROR on Begin value");
		}

		//Fin d'une partie
		if (ReponseC.propCoup != CONT)
		{
			nbtour = 0;
			int res = htonl(1000);
			err = send(sockJava, &res, sizeof(int), 0);
			res = ntohl(res);
			if (begin == 1)
			{
				begin--;
			}
			else
			{
				begin++;
			}
			nbCoup = 0;
			nbPartie++;
		}
	}

	shutdown(sockServer, SHUT_RDWR);
	close(sockServer);
	shutdown(sockJava, SHUT_RDWR);
	close(sockJava);
	close(sockServ);
	return 0;
}
