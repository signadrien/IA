
#include "fonctionTCP.h"
#include "ProtoQuantik.h"

int main(int argc, char **argv)
{

	if (argc != 2)
	{
		printf("usage : %s port\n", argv[0]);
		return -1;
	}
	char *joueursName[2];
	int port = atoi(argv[1]);
	int sockTrans;
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
	sockTransJ1 = accept(sockServ, (struct sockaddr *)&addClient, (socklen_t *)&sizeAddr);
	if (sockTrans < 0)
	{
		perror("(serveurTCP) erreur sur accept");
		return -5;
	}
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
		joueursName[0] = reqJoueur1.nomJoueur;
	}
	else
	{
		joueursName[1] = reqJoueur1.nomJoueur;
	}
	repJoueur1.validCoulPion = OK;

	//Joueur 2
	sockTransJ2 = accept(sockServ, (struct sockaddr *)&addClient, (socklen_t *)&sizeAddr);
	if (sockTransJ2 < 0)
	{
		perror("(serveurTCP) erreur sur accept");
		return -5;
	}
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
		repJoueur2.validCoulPion = KO;
	}
	if (reqJoueur1.coulPion == BLANC)
	{
		joueursName[1] = reqJoueur2.nomJoueur;
	}
	else
	{
		joueursName[0] = reqJoueur2.nomJoueur;
	}

	//Envoie au joueurs la réponse
	repJoueur1.nomAdvers = reqJoueur2.nomJoueur;
	repJoueur2.nomAdvers = reqJoueur1.nomJoueur;
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