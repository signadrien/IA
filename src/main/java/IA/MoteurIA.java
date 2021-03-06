package IA;

import org.jpl7.*;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.lang.Integer;
import java.net.Socket;

public class MoteurIA {

	public static void main(String[] args) {
		int plateau[][][] = { { {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1} }, { {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1} }, { {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1} }, { {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1} } };
		int reserve[][] = { { 2, 2, 2, 2 }, { 2, 2, 2, 2 } };

		int port = 0;
		String addr = "";
		int nbPartie = 0;
		if (args.length != 2) {
			System.out.println("Usage : adresse port");
		}
		addr = args[0];
		port = Integer.parseInt(args[1]);
		int type = 0;
		try {
			String file = System.getProperty("user.dir") + "/IA.pl";
			JPL.init();
			Query q1 = new Query("consult('" + file + "')");
			System.out.println("consult " + (q1.hasSolution() ? "succeeded" : "failed"));
			Socket sock = new Socket(addr, port);
			DataInputStream DIS = new DataInputStream(sock.getInputStream());
			DataOutputStream DOS = new DataOutputStream(sock.getOutputStream());
			while (nbPartie < 2) {
				int result = DIS.readInt();
				if (result != -1000) {
					if (result >= 1000) {
						nbPartie++;
						for (int i = 0; i < 4; i++) {
							for (int j = 0; j < 4; j++) {
								plateau[j][i][0] = -1;
								plateau[i][j][1] = -1;
								if (j < 2)
									reserve[j][i] = 2;
							}
						}
						continue;
					}
					type = result % 10;
					result /= 10;
					int colonne = result % 10;
					result /= 10;
					int ligne = result;
					plateau[ligne][colonne][0] = type;
					plateau[ligne][colonne][1] = 1;
					reserve[1][type]--;
				}
				String plat = "[";
				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 4; j++) {
						plat += "["+plateau[i][j][0] + ","+plateau[i][j][1]+"]";
						if (i != 3 || j != 3)
							plat += ",";
					}
				}
				plat += "]";
				String indPossible = "[";
				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 4; j++) {
						if (plateau[i][j][0] == -1) {
							indPossible += i * 4 + j;
							indPossible += ",";
						}
					}
				}
				indPossible = indPossible.substring(0, indPossible.length() - 1);
				indPossible += "]";

				String reser = "[";
				for (int i = 0; i < 4; i++) {
					if (reserve[0][i] > 0)
						reser += i + ",";
				}
				reser = reser.substring(0, reser.length() - 1);
				reser += "]";
				Query q4 = new Query(
						"fonction(" + plat + "," + reser + "," + indPossible + ",Gagne,Ligne,Colonne,Type)");

						System.out.println("fonction(" + plat + "," + reser + "," + indPossible + ",Gagne,Ligne,Colonne,Type)");
				java.util.Map<String, Term> solution;
					solution = q4.oneSolution();

					int reponse = 0;
					plateau[solution.get("Ligne").intValue()][solution.get("Colonne").intValue()][0] = solution.get("Type")
							.intValue();
					plateau[solution.get("Ligne").intValue()][solution.get("Colonne").intValue()][1] = 0;
					int nbPion = 0;
					for(int i =0;i<4;i++){
						for(int j = 0 ; j<4;j++){
							if(plateau[i][j][0]!=-1){
								nbPion++;
							}
						}
					}
					if (solution.get("Gagne").intValue() == 1) {
						reponse += 1000;
					}
					else{
						if (nbPion == 15) {
							reponse += 2000;
						}
					}
					reponse += solution.get("Ligne").intValue() * 100;
					reponse += solution.get("Colonne").intValue() * 10;
					reponse += solution.get("Type").intValue();
					reserve[0][type]--;
					System.out.println(type+ " :"+ reserve[0][type]);
					System.out.println(reponse);
					DOS.writeInt(reponse);
				}
			sock.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
