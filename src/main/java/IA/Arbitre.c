
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
	int sockServ = socketServeur(port);
	sizeAddr = sizeof(struct sockaddr_in);
	TPartieReq reqJoueur1;
	TPartieRep repJoueur1;
	TPartieReq reqJoueur2;
	TPartieRep repJoueur2;
	//Joueur 1
	printf("Attente de la connexion du premier joueur");
	sockTransJ1 = accept(sockServ, (struct sockaddr *)&addClient, (socklen_t *)&sizeAddr);
	if (sockTransJ1 < 0)
	{
		perror("(serveurTCP) erreur sur accept");
		return -5;
	}
	printf("Réception requête joueur 1");
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
		printf("Joueur 1 a la couleur blanche");
		joueursName[0] = reqJoueur1.nomJoueur;
	}
	else
	{	
		printf("Joueur 1 a la couleur noire");
		joueursName[1] = reqJoueur1.nomJoueur;
	}

	repJoueur1.validCoulPion = OK;

	//Joueur 2
	printf("Attente de connexion du joueur 2");
	sockTransJ2 = accept(sockServ, (struct sockaddr *)&addClient, (socklen_t *)&sizeAddr);
	if (sockTransJ2 < 0)
	{
		perror("(serveurTCP) erreur sur accept");
		return -5;
	}
	printf("Réception de la requête du joueur 2");
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
	else{
		printf("Joueur 2 a ");
		repJoueur2.validCoulPion = OK;
	}
	if (repJoueur2.validCoulPion == KO)
    {
        if (reqJoueur1.coulPion == BLANC)
        {
            reqJoueur2.coulPion = NOIR;
			printf("la couleur noire");
			joueursName[1] = reqJoueur2.nomJoueur;
        }
        else
        {
            reqJoueur2.coulPion = BLANC;
			printf("la couleur blanche");
			joueursName[0] = reqJoueur2.nomJoueur;
        }
    }
	else{
		if (reqJoueur2.coulPion == BLANC)
        {
			printf("la couleur blanche");
			joueursName[0] = reqJoueur2.nomJoueur;
        }
        else
        {
			printf("la couleur noire");
			joueursName[1] = reqJoueur2.nomJoueur;
        }
	}

	//Envoie au joueurs la réponse
	memcpy(repJoueur1.nomAdvers,reqJoueur2.nomJoueur,T_NOM);
	memcpy(repJoueur2.nomAdvers,reqJoueur1.nomJoueur,T_NOM);
	repJoueur1.err = ERR_OK;
	repJoueur2.err = ERR_OK;

	err = send(sockTransJ1, &repJoueur1, sizeof(TCodeRep), 0);
	if (err <= 0) {
		perror("(client) erreur sur le send");
		shutdown(sockTransJ1, SHUT_RDWR); close(sockTransJ1);
		return -5;
	}
	err = send(sockTransJ2, &repJoueur2, sizeof(TCodeRep), 0);
	if (err <= 0) {
		perror("(client) erreur sur le send");
		shutdown(sockTransJ2, SHUT_RDWR); close(sockTransJ2);
		return -5;
	}

	/*
  int op1;
  int op2;
  int port = atoi(argv[1]);
  int sockTrans;
  int sizeAddr;
  int err;
  int stop = 0;
  Oper op;
  Reponse rep;
  struct sockaddr_in addClient;
  int sockServ = socketServeur(port);
  int skip = 0;
  sizeAddr = sizeof(struct sockaddr_in);
  
  sockTrans = accept(sockServ,(struct sockaddr *)&addClient,(socklen_t *)&sizeAddr);
	if (sockTrans < 0) {
	  perror("(serveurTCP) erreur sur accept");
	  return -5;
	}
  
  do {
  
		err = recv(sockTrans, &op, sizeof(Oper), 0);
		if (err <= 0) {
		  perror("(serveurTCP) erreur dans la reception");
		  shutdown(sockTrans, SHUT_RDWR); close(sockTrans);
		  return -6;
		}
		
		int result;
		op1 = op.op1;
		op2 = op.op2;
		
		switch(op.operande) {
			case ADD:
				skip = 0;
				result = op1 + op2;
				printf("Calcul : %d + %d = %d\n", op1, op2, result);
				rep.ret = OK;
				break;
			case SUB:
				skip = 0;
				result = op1 - op2;
				printf("Calcul : %d - %d = %d\n", op1, op2, result);
				rep.ret = OK;
				break;
			case MULT:
				skip = 0;
				result = op1 * op2;
				printf("Calcul : %d * %d = %d\n", op1, op2, result);
				rep.ret = OK;
				break;
			case DIV:
				skip = 0;
				if (op2 == 0) {
					result = -1;
					rep.ret = DIVO;
				} else {
					result = op1 / op2;
					printf("Calcul : %d / %d = %d\n", op1, op2, result);
					rep.ret = OK;
				}
				break;
			case STOP:
				sockTrans = accept(sockServ,(struct sockaddr *)&addClient,(socklen_t *)&sizeAddr);
				if (sockTrans < 0) {
					perror("(serveurTCP) erreur sur accept");
					return -5;
				}
				skip = 1;
				break;
			default:
				skip = 0;
				result = -1;
				rep.ret = ERR;
				break;
		}

		if (skip == 0) {
			rep.resultat = result;		
			err = send(sockTrans, &rep, sizeof(Reponse), 0);
			if (err <= 0) {
				perror("(client) erreur sur le send");
				shutdown(sockTrans, SHUT_RDWR); close(sockTrans);
				return -5;
			}
		}

	}while(stop == 0);

  shutdown(sockTrans, SHUT_RDWR); close(sockTrans);
  close(sockServ);
  return 0;
  */
}
