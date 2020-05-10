
#include "fonctionTCP.h"
#include "validation.h"
#include "sys/time.h"

int main(int argc, char **argv)
{

	if (argc != 2)
	{
		printf("usage : %s port\n", argv[0]);
		return -1;
	}

	fd_set readSet;		  //Descripteurs pour select
	struct timeval delai; //timer de 5 sec
	delai.tv_sec = 5;
	delai.tv_usec = 0;

	char *joueursName[2]; //Tableau contenant les pseudos des deux joueurs
	int port = atoi(argv[1]);
	int sockTransJ1; //Socket de transmission du joueur 1
	int sockTransJ2; //Socket de transmission du joueur 2
	int sizeAddr;
	int err; // Sert à check si des erreurs se sont produites
	struct sockaddr_in addClient;
	int sockServ = socketServeur(port); // Initialisation de la socket de Connexion
	sizeAddr = sizeof(struct sockaddr_in);

	TPartieReq reqJoueur1; //Requête du joueur 1
	TPartieRep repJoueur1; //Réponse pour le joueur 1
	TPartieReq reqJoueur2; //Requête du joueur 2
	TPartieRep repJoueur2; //Réponse pour le joueur 2

	int white; //socket du joueur blanc
	int black; //socket du joueur noir
	printf("Attente de la connexion du premier joueur...\n");
	//Accepte la demande de connexion du premier joueur
	sockTransJ1 = accept(sockServ, (struct sockaddr *)&addClient, (socklen_t *)&sizeAddr);
	if (sockTransJ1 < 0)
	{
		perror("(serveurTCP) erreur sur accept");
		return -5;
	}
	//Attente de sa requête TPartieReq
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
	// La validation de couleur du joueur 1 est toujours OK
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
	//Réception de la requête
	err = recv(sockTransJ2, &reqJoueur2, sizeof(TPartieReq), 0);
	if (err <= 0)
	{
		perror("(serveurTCP) erreur dans la reception");
		shutdown(sockTransJ2, SHUT_RDWR);
		close(sockTransJ2);
		return -6;
	}
	//Si la couleur est déjà choisie
	if (reqJoueur1.coulPion == reqJoueur2.coulPion)
	{
		printf("Joueur 1 a déjà choisie cette couleur, donc Joueur 2 a ");
		//Code KO
		repJoueur2.validCoulPion = KO;
	}
	//Sinon
	else
	{
		printf("Joueur 2 a ");
		repJoueur2.validCoulPion = OK;
	}
	//Si la couleur est modifié, alors
	if (repJoueur2.validCoulPion == KO)
	{
		//Si le joueur 1 à la couleur blanche
		if (reqJoueur1.coulPion == BLANC)
		{
			//Le joueur deux à la couleur noire
			black = sockTransJ2;
			//On rempli la structure de réponse
			reqJoueur2.coulPion = NOIR;
			printf("la couleur noire\n");
			//On rempli le tableau de pseudo
			joueursName[1] = reqJoueur2.nomJoueur;
		}
		//Si il a la couleur noire
		else
		{
			//Le joueur deux à la couleur blanche
			white = sockTransJ2;
			//On remplie la structure de réponse
			reqJoueur2.coulPion = BLANC;
			printf("la couleur blanche\n");
			joueursName[0] = reqJoueur2.nomJoueur;
		}
	}
	//Sinon si la validation de couleur est OK, on rempli la structure de réponse et le tableau des pseudos en fonction
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

	int nfsd = 6;

	//Envoie au joueurs la réponse de leur requête, indiquant quelle couleurs ils ont
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

	bool fin = false;		//Booléen pour savoir si la manche est finie
	TCoupReq reqCoup;		//Variable stockants les requêtes des 2 joueurs
	TCoupRep repCoupJ1;		//Réponse pour le joueur 1
	TCoupRep repCoupJ2;		//Réponse pour le joueur 2
	TPropCoup arbitrage;	// Propriété du coup donné par un joueur après l'arbitrage
	bool isValid;			// Booléen si le coup donné est valide
	int J1Win = 0;			// Nombre de victoire du joueur 1
	int J2Win = 0;			// Nombre de victoire du joueur 2
	char *tempoName;		//Variable temporaire lors du changement entre la partie
	int temporaireWin;		// De même
	int compteurPartie = 1; // Nombre de partie
	bool inversion = false; //Inversion des deux joueurs initialisé à faux
	//Jeu
	while (compteurPartie <= 2)
	{
		FD_ZERO(&readSet);
		printf("Partie numéro %d\n", compteurPartie);
		initialiserPartie();
		//Le joueur 1 change en fonction de la manche
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
		}
		while (!fin)
		{
			//Attente d'un coup de joueur 1
			printf("Attente du coup de joueur 1 %s\n", joueursName[0]);
			//On redéfini le timer qui est réinitialiser entre chaque select (si j'ai bien compris)
			delai.tv_sec = 5;
			delai.tv_usec = 0;
			//On ajoute la socket du joueur 1 dans le descripteur
			FD_SET(sockTransJ1, &readSet);
			err = select(nfsd, &readSet, NULL, NULL, &delai);
			if (err < 0)
			{
				perror("Erreur dans le select\n");
				shutdown(sockTransJ1, SHUT_RDWR);
				close(sockTransJ1);

				shutdown(sockTransJ2, SHUT_RDWR);
				close(sockTransJ2);
				return -5;
			}
			//Si le joueur 1 a envoyé avant 5 sec son coup
			if (FD_ISSET(sockTransJ1, &readSet) != 0)
			{
				err = recv(sockTransJ1, &reqCoup, sizeof(TCoupReq), 0);
				if (err < 0)
				{
					perror("Erreur recv coup joueur 1\n");
					shutdown(sockTransJ1, SHUT_RDWR);
					close(sockTransJ1);
				}
				//On stock la validité du coup
				isValid = validationCoup(1, reqCoup, &arbitrage);
				//On rempli les réponse avec l'arbitrage
				repCoupJ1.propCoup = arbitrage;
				repCoupJ2.propCoup = arbitrage;
				//Si le coup est valide
				if (isValid)
				{
					//On rempli d'abord la validité du coup dans les deux réponses des joueurs
					repCoupJ1.validCoup = VALID;
					repCoupJ2.validCoup = VALID;
					//Si le coup est gagnant
					if (arbitrage == GAGNE)
					{
						printf("Joueur 1 gagne la manche\n");
						repCoupJ1.propCoup = GAGNE;
						repCoupJ2.propCoup = PERDU;
						//Fin de la manche
						fin = true;
						J1Win++;
					}
					//Si le coup est perdant
					if (arbitrage == PERDU)
					{
						printf("Joueur 2 gagne la manche\n");
						repCoupJ1.propCoup = PERDU;
						repCoupJ2.propCoup = GAGNE;
						J2Win++;
						fin = true;
					}
					//Si le coup donne match nul
					if (arbitrage == NUL)
					{
						printf("Match nul\n");
						repCoupJ1.propCoup = NUL;
						repCoupJ2.propCoup = NUL;
						fin = true;
					}
				}
				//Si le coup est invalide ou que l'arbitrage n'est pas d'accord avec la propriété donné par le joueur
				if (!isValid || arbitrage != reqCoup.propCoup)
				{
					//Le joueur est compté comme tricheur et perd la manche
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
			}
			//Si pas de réponse dans les 5 secondes
			else
			{
				printf("Rien reçu\n");
				repCoupJ1.validCoup = TIMEOUT;
				repCoupJ1.propCoup = PERDU;
				repCoupJ2.validCoup = TIMEOUT;
				repCoupJ2.propCoup = GAGNE;
				repCoupJ1.err = ERR_OK;
				repCoupJ2.err = ERR_OK;
				J2Win++;
				fin = true;
			}
			//On retire la socket du joueur 1 et on ajoute celle du joueur 2 dans le descripteur
			FD_CLR(sockTransJ1, &readSet);
			FD_SET(sockTransJ2, &readSet);
			printf("Envoie au joueur 1 la réponse à sa requête\n");
			//Envoie des réponses aux deux joueurs
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
			//Si la partie est finie on casse la boucle
			if (fin)
			{
				printf("La partie est finie\n");
				continue;
			}
			//Sinon on envoie la requête au joueur suivant du joueur qui a joué
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

			//Tour du joueur 2(même chose que tour 1 mais en inversé)
			delai.tv_sec = 5;
			delai.tv_usec = 0;
			printf("Attente du coup de joueur 2 %s\n", joueursName[1]);
			err = select(nfsd, &readSet, NULL, NULL, &delai);
			if (err < 0)
			{
				perror("Erreur dans le select\n");
				shutdown(sockTransJ1, SHUT_RDWR);
				close(sockTransJ1);

				shutdown(sockTransJ2, SHUT_RDWR);
				close(sockTransJ2);
				return -5;
			}
			if (FD_ISSET(sockTransJ2, &readSet) != 0)
			{
				err = recv(sockTransJ2, &reqCoup, sizeof(TCoupReq), 0);
				if (err < 0)
				{
					perror("Erreur recv coup joueur 1\n");
					shutdown(sockTransJ2, SHUT_RDWR);
					close(sockTransJ2);
				}
				isValid = validationCoup(2, reqCoup, &arbitrage);
				repCoupJ1.propCoup = arbitrage;
				repCoupJ2.propCoup = arbitrage;
				if (isValid)
				{
					repCoupJ1.validCoup = VALID;
					repCoupJ2.validCoup = VALID;
					if (arbitrage == GAGNE)
					{
						printf("Joueur 2 gagne la manche\n");
						repCoupJ1.propCoup = PERDU;
						repCoupJ2.propCoup = GAGNE;
						fin = true;
						J1Win++;
					}
					if (arbitrage == PERDU)
					{
						printf("Joueur 1 gagne la manche\n");
						repCoupJ1.propCoup = GAGNE;
						repCoupJ2.propCoup = PERDU;
						J2Win++;
						fin = true;
					}
					if (arbitrage == NUL)
					{
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
					repCoupJ1.propCoup = GAGNE;
					J1Win++;
					repCoupJ2.validCoup = TRICHE;
					repCoupJ2.propCoup = PERDU;
					fin = true;
				}
				repCoupJ1.err = ERR_OK;
				repCoupJ2.err = ERR_OK;
			}
			else
			{
				printf("Rien reçu\n");
				repCoupJ1.validCoup = TIMEOUT;
				repCoupJ1.propCoup = PERDU;
				repCoupJ2.validCoup = TIMEOUT;
				repCoupJ2.propCoup = GAGNE;
				repCoupJ1.err = ERR_OK;
				repCoupJ2.err = ERR_OK;
				J2Win++;
				fin = true;
			}

			FD_CLR(sockTransJ2, &readSet);
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
		//Fin de la manche, on remet fin à false pour la partie d'après, on augmente le compteur de partie, et on inverse les joueurs
		fin = false;
		compteurPartie++;
		inversion = !inversion;
		printf("Score J1 : %d\n", J1Win);
		printf("Score J2 : %d\n", J2Win);
	}
	//Fermeture des sockets
	shutdown(sockTransJ1, SHUT_RDWR);
	close(sockTransJ1);
	shutdown(sockTransJ2, SHUT_RDWR);
	close(sockTransJ2);
}
