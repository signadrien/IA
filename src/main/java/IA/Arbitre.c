
#include "fonctionTCP.h"
#include "validation.h"


int main(int argc, char **argv)
{

	if (argc != 2)
	{
		printf("usage : %s port\n", argv[0]);
		return -1;
	}
	char *joueursName[2];
	int port = atoi(argv[1]);
	int sockTransJ1;
	int sockTransJ2;
	int sizeAddr;
	int err;
	struct sockaddr_in addClient;
	printf("Attente de la connexion du premier joueur...\n");
	int sockServ = socketServeur(port);
	sizeAddr = sizeof(struct sockaddr_in);
	TPartieReq reqJoueur1;
	TPartieRep repJoueur1;
	TPartieReq reqJoueur2;
	TPartieRep repJoueur2;
	//Joueur 1
	int white; //socket du joueur blanc
	int black; //socket du joueur noir
	sockTransJ1 = accept(sockServ, (struct sockaddr *)&addClient, (socklen_t *)&sizeAddr);
	if (sockTransJ1 < 0)
	{
		perror("(serveurTCP) erreur sur accept");
		return -5;
	}
	printf("Réception requête joueur 1\n");
	err = recv(sockTransJ1, &reqJoueur1, sizeof(TPartieReq), 0);
	if (err <= 0)
	{
		perror("(serveurTCP) erreur dans la reception");
		shutdown(sockTransJ1, SHUT_RDWR);
		close(sockTransJ1);
		return -6;
	}
	if (reqJoueur1.coulPion == BLANC)
	{
		white = sockTransJ1;
		printf("Joueur 1 a la couleur blanche\n");
		joueursName[0] = reqJoueur1.nomJoueur;
	}
	else
	{
		black = sockTransJ1;
		printf("Joueur 1 a la couleur noire\n");
		joueursName[1] = reqJoueur1.nomJoueur;
	}

	repJoueur1.validCoulPion = OK;

	//Joueur 2
	printf("Attente de connexion du joueur 2\n");
	sockTransJ2 = accept(sockServ, (struct sockaddr *)&addClient, (socklen_t *)&sizeAddr);
	if (sockTransJ2 < 0)
	{
		perror("(serveurTCP) erreur sur accept");
		return -5;
	}
	printf("Réception de la requête du joueur 2\n");
	err = recv(sockTransJ2, &reqJoueur2, sizeof(TPartieReq), 0);
	if (err <= 0)
	{
		perror("(serveurTCP) erreur dans la reception");
		shutdown(sockTransJ2, SHUT_RDWR);
		close(sockTransJ2);
		return -6;
	}
	if (reqJoueur1.coulPion == reqJoueur2.coulPion)
	{
		printf("Joueur 1 a déjà choisie cette couleur, donc Joueur 2 a ");
		repJoueur2.validCoulPion = KO;
	}
	else
	{
		printf("Joueur 2 a ");
		repJoueur2.validCoulPion = OK;
	}
	if (repJoueur2.validCoulPion == KO)
	{
		if (reqJoueur1.coulPion == BLANC)
		{
			black = sockTransJ2;
			reqJoueur2.coulPion = NOIR;
			printf("la couleur noire\n");
			joueursName[1] = reqJoueur2.nomJoueur;
		}
		else
		{
			white = sockTransJ2;
			reqJoueur2.coulPion = BLANC;
			printf("la couleur blanche\n");
			joueursName[0] = reqJoueur2.nomJoueur;
		}
	}
	else
	{
		if (reqJoueur2.coulPion == BLANC)
		{
			white = sockTransJ2;
			printf("la couleur blanche\n");
			joueursName[0] = reqJoueur2.nomJoueur;
		}
		else
		{
			printf("la couleur noire\n");
			black = sockTransJ2;
			joueursName[1] = reqJoueur2.nomJoueur;
		}
	}

	//Envoie au joueurs la réponse
	memcpy(repJoueur1.nomAdvers, reqJoueur2.nomJoueur, T_NOM);
	memcpy(repJoueur2.nomAdvers, reqJoueur1.nomJoueur, T_NOM);
	repJoueur1.err = ERR_OK;
	repJoueur2.err = ERR_OK;

	printf("Envoie au joueur 1 de la réponse \n");
	err = send(sockTransJ1, &repJoueur1, sizeof(TPartieRep), 0);
	if (err <= 0)
	{
		perror("(client) erreur sur le send");
		shutdown(sockTransJ1, SHUT_RDWR);
		close(sockTransJ1);
		return -5;
	}
	printf("Envoie au joueur 2 de la réponse \n");
	err = send(sockTransJ2, &repJoueur2, sizeof(TPartieRep), 0);
	if (err <= 0)
	{
		perror("(client) erreur sur le send");
		shutdown(sockTransJ2, SHUT_RDWR);
		close(sockTransJ2);
		return -5;
	}

	printf("La partie oppose %s à %s\n", joueursName[0], joueursName[1]);

	printf("%s commence la partie.\n", joueursName[0]);

	bool fin = false;
	TCoupReq reqCoup;
	TCoupRep repCoupJ1;
	TCoupRep repCoupJ2;
	TPropCoup arbitrage;
	bool isValid;
	int J1Win = 0;
	int J2Win = 0;
	char *tempoName;
	int temporaireWin;
	int compteurPartie = 1;
	bool inversion = false;
	//Jeu
	while (compteurPartie <= 2)
	{
		printf("Partie numéro %d\n", compteurPartie);
		initialiserPartie();
		if (!inversion)
		{
			printf("Les blancs commencent\n");
			sockTransJ1 = white;
			sockTransJ2 = black;
			if (compteurPartie != 1)
			{
				temporaireWin = J1Win;
				J1Win = J2Win;
				J2Win = temporaireWin;
				tempoName = joueursName[0];
				joueursName[0] = joueursName[1];
				joueursName[1] = tempoName;
			}
		}
		else
		{

			tempoName = joueursName[0];
			joueursName[0] = joueursName[1];
			joueursName[1] = tempoName;
			temporaireWin = J1Win;
			J1Win = J2Win;
			J2Win = temporaireWin;
			sockTransJ1 = black;
			sockTransJ2 = white;
			printf("Les noirs commencent\n");
			printf("%d\n", fin);
		}
		while (!fin)
		{
			//Attente d'un coup de joueur 1
			printf("Attente du coup de joueur 1 %s\n", joueursName[0]);
			err = recv(sockTransJ1, &reqCoup, sizeof(TCoupReq), 0);
			isValid = validationCoup(1, reqCoup, &arbitrage);
			repCoupJ1.propCoup = arbitrage;
			repCoupJ2.propCoup = arbitrage;
			if (isValid)
			{
				repCoupJ1.validCoup = VALID;
				repCoupJ2.validCoup = VALID;
				if(arbitrage == GAGNE){
					printf("Joueur 1 gagne la manche\n");
					repCoupJ1.propCoup = GAGNE;
					repCoupJ2.propCoup = PERDU;
					fin=true;
					J1Win++;
				}
				if(arbitrage == PERDU){
					printf("Joueur 2 gagne la manche\n");
					repCoupJ1.propCoup = PERDU;
					repCoupJ2.propCoup = GAGNE;
					J2Win++;
					fin=true;
				}
				if(arbitrage == NUL){
					printf("Match nul\n");
					repCoupJ1.propCoup = NUL;
					repCoupJ2.propCoup = NUL;
					fin = true;
				}
				
			}
			if (!isValid || arbitrage != reqCoup.propCoup)
			{
				printf("Triche");
				repCoupJ1.validCoup = TRICHE;
				repCoupJ1.propCoup = PERDU;
				repCoupJ2.validCoup = TRICHE;
				repCoupJ2.propCoup = GAGNE;
				J2Win++;
				fin = true;
			}
			repCoupJ1.err = ERR_OK;
			repCoupJ2.err = ERR_OK;

			printf("Envoie au joueur 1 la réponse à sa requête\n");

			err = send(sockTransJ1, &repCoupJ1, sizeof(TCoupRep), 0);
			if (err <= 0)
			{
				perror("(client) erreur sur le send");
				shutdown(sockTransJ1, SHUT_RDWR);
				close(sockTransJ1);
				return -5;
			}
			printf("Envoie au joueur 2 la réponse à la requête du joueur 1\n");
			err = send(sockTransJ2, &repCoupJ2, sizeof(TCoupRep), 0);
			if (err <= 0)
			{
				perror("(client) erreur sur le send");
				shutdown(sockTransJ2, SHUT_RDWR);
				close(sockTransJ2);
				return -5;
			}

			if (fin)
			{
				printf("La partie est finie\n");
				continue;
			}
			else
			{
				printf("La partie continue, envoie au joueur 2 la requête du joueur 1\n");
				err = send(sockTransJ2, &reqCoup, sizeof(TCoupReq), 0);
				if (err <= 0)
				{
					perror("(client) erreur sur le send");
					shutdown(sockTransJ2, SHUT_RDWR);
					close(sockTransJ2);
					return -5;
				}
			}
			printf("%s a joué, au tour de %s\n", joueursName[0], joueursName[1]);

			//Tour du joueur 2

			printf("Attente du coup de joueur 2 %s\n", joueursName[1]);
			err = recv(sockTransJ2, &reqCoup, sizeof(TCoupReq), 0);
			
			isValid = validationCoup(2, reqCoup, &arbitrage);
			repCoupJ1.propCoup = arbitrage;
			repCoupJ2.propCoup = arbitrage;
			if (isValid)
			{
				repCoupJ1.validCoup = VALID;
				repCoupJ2.validCoup = VALID;
			}
			if (!isValid || arbitrage != reqCoup.propCoup)
			{
				printf("Triche");
				repCoupJ1.validCoup = TRICHE;
				repCoupJ1.propCoup = GAGNE;
				J1Win++;
				repCoupJ2.validCoup = TRICHE;
				repCoupJ2.propCoup = PERDU;
				fin = true;
			}
			repCoupJ1.err = ERR_OK;
			repCoupJ2.err = ERR_OK;

			printf("Envoie au joueur 1 la réponse à la requête du joueur 2\n");
			err = send(sockTransJ1, &repCoupJ1, sizeof(TCoupRep), 0);
			if (err <= 0)
			{
				perror("(client) erreur sur le send");
				shutdown(sockTransJ1, SHUT_RDWR);
				close(sockTransJ1);
				return -5;
			}
			printf("Envoie au joueur 2 la réponse à sa requête\n");
			err = send(sockTransJ2, &repCoupJ2, sizeof(TCoupRep), 0);
			if (err <= 0)
			{
				perror("(client) erreur sur le send");
				shutdown(sockTransJ2, SHUT_RDWR);
				close(sockTransJ2);
				return -5;
			}

			if (fin)
			{
				printf("Fin de la partie\n");
				continue;
			}
			else
			{
				printf("La partie continue, envoie au joueur 1 la requête du joueur 2\n");
				err = send(sockTransJ1, &reqCoup, sizeof(TCoupReq), 0);
				if (err <= 0)
				{
					perror("(client) erreur sur le send");
					shutdown(sockTransJ1, SHUT_RDWR);
					close(sockTransJ1);
					return -5;
				}
			}
		}
		printf("Partie suivante!\n");
		fin = false;
		compteurPartie++;
		inversion = !inversion;
		printf("Score J1 : %d\n", J1Win);
		printf("Score J2 : %d\n", J2Win);
	}
}
