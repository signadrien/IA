package IA;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Scanner;


public class MoteurIA {

	public static void main (String[] args) {
		int plateau[][] = {{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1}};
		int reserve[][]= {{2,2,2,2},{2,2,2,2}};

		int port = 0;
		String addr = "";
		int nbPartie = 0;
		if(args.length != 2) {
			System.out.println("Usage : adresse port");
		}
		addr = args[0];
		port = Integer.parseInt(args[1]);
		try{
			Socket sock = new Socket(addr,port);
			DataInputStream DIS = new DataInputStream(sock.getInputStream());
			DataOutputStream DOS = new DataOutputStream(sock.getOutputStream());
			while (nbPartie < 2) {
				result = DIS.readInt();
				if(result>1000){
					nbPartie++;
					for(int i = 0; i< 4;i++ ){
						for(int j =0; j<4;j++){
							plateau[j][i] = -1;
							if(j<2)
								reserve[j][i] = 2;
						}
					}
					continue;
				}
				int type = result%10;
				result/=10;
				int colonne = result%10;
				result/=10;
				int ligne =  result;

					//ask prolog

					plateau[ligne][colonne] = type;
					reserve[1][type]--;

					int reponse = 0;
					//compute response prolog

					reserve[0][type]--;

					DOS.writeInt(reponse);

					int verification = DIS.readInt();
					if(verification == 0){
						nbPartie++;
						for(int i = 0; i< 4;i++ ){
							for(int j =0; j<4;j++){
								plateau[j][i] = -1;
								if(j<2)
									reserve[j][i] = 2;
							}
						}
					}
			}
			sock.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}

