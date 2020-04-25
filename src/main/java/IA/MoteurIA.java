package IA;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Scanner;


public class MoteurIA {

	int tab[][] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};

	void placement(int l, int c, int p, int t){
		switch(p){
			case 1 :
				tab[l][c] = t;
			break;
			case 2 :
				tab[l][c] = -t;
			break;
			default:
		}
	}


	public static void main (String[] args) {



		for(int i =0 ; i < 4; i++){
			for(int j = 0; j < 4;j++){
				
			}
		}

















/*
		int port = 0;
		String addr = "";
		int arret = 0;
		if(args.length != 2) {
			System.out.println("Usage : adresse port");
		}
		addr = args[0];
		port = Integer.parseInt(args[1]);
		try{
			Socket sock = new Socket(addr,port);
			DataInputStream DIS = new DataInputStream(sock.getInputStream());
			DataOutputStream DOS = new DataOutputStream(sock.getOutputStream());
			int op1 = -1;
			int op2 = -1;
			int op3 = -1;
			char operande = 'o';
			Scanner sc = new Scanner(System.in);
			while (arret <= 1) {
				System.out.println("Premier operande");
				op1 = sc.nextInt();
				System.out.println("Operateur");
				operande = sc.next().charAt(0);
				System.out.println("Seconde operande");
				op2 = sc.nextInt();
				int result = -1;
				DOS.writeInt(op1);
				op3 = (int) operande;
				DOS.writeInt(op3);
				DOS.writeInt(op2);
				result = DIS.readInt();
				System.out.println("Réception du calcul");
				System.out.println(op1 + " " + operande + " " + op2 + " = " + result);
				System.out.println("Continuer les calculs ? (0/1)");
				arret = sc.nextInt();
				DOS.writeInt(arret);
				if (arret > 0) {		
					sock.close();
					break;
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}*/
}

