#include "fonctionTCP.h"

#define TAIL_BUF 20

int main(int argc, char** argv) {

  /*
   * verification des arguments
   */
  if (argc != 2) {
    printf ("usage : %s port\n", argv[0]);
    return -1;
  }
  
  int port  = atoi(argv[1]);
  int  sockTrans,       /* descripteur socket transmission */
       sizeAddr,        /* taille de l'adresse d'une socket */
       err;	        /* code d'erreur */
  
  char buffer[TAIL_BUF]; /* buffer de reception */
  struct sockaddr_in addServ;	/* adresse socket connex serveur */
  struct sockaddr_in addClient;	/* adresse de la socket client connectee */

  int sockServ = socketServeur(port);
  /*
   * attente de connexion
   */
  sizeAddr = sizeof(struct sockaddr_in);
  sockTrans = accept(sockServ, 
		     (struct sockaddr *)&addClient, 
		     (socklen_t *)&sizeAddr);
  if (sockTrans < 0) {
    perror("(serveurTCP) erreur sur accept");
    return -5;
  }
  
  /*
   * reception et affichage du message en provenance du client
   */
  err = recv(sockTrans, buffer, TAIL_BUF, 0);
  if (err <= 0) {
    perror("(serveurTCP) erreur dans la reception");
    shutdown(sockTrans, SHUT_RDWR); close(sockTrans);
    return -6;
  }
  printf("(serveurTCP) voila le message recu : %s\n", buffer);

  /* 
   * arret de la connexion et fermeture
   */
  shutdown(sockTrans, SHUT_RDWR); close(sockTrans);
  close(sockServ);

  return 0;
}
