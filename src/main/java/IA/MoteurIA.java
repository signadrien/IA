package IA;
import org.jpl7.*;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.lang.Integer;
import java.net.Socket;


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
		int type=0;
		try{
			String file = System.getProperty("user.dir") + "/IA.pl";
			System.out.println(file);
			JPL.init();
			Query q1 = new Query("consult('"+file+"')");
			System.out.println( "consult " + (q1.hasSolution() ? "succeeded" : "failed"));
			Socket sock = new Socket(addr,port);
			DataInputStream DIS = new DataInputStream(sock.getInputStream());
			DataOutputStream DOS = new DataOutputStream(sock.getOutputStream());
			while (nbPartie < 2) {
				int result = DIS.readInt();
				if(result!=-1000){
					if(result>=1000){
						System.out.println("Fin de partie réinitialisation");
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
					System.out.println(result);
					type = result%10;
					result/=10;
					int colonne = result%10;
					result/=10;
					int ligne =  result;
					plateau[ligne][colonne] = type;
					reserve[1][type]--;
				}
				String plat = "[" ;
				for(int i = 0; i< 4;i++ ){
					for(int j =0; j<4;j++){
						plat+=plateau[i][j];
						if(i!=3 || j!=3)
							plat+=",";
					}
				}
				plat+="]";
				System.out.println(plat);

				String reser ="[";
				for(int i =0;i<4;i++){
					for(int j =0;j<reserve[0][i];j++){
						reser+=i;
						if(i!=3 || j!=reserve[0][i]-1)
							reser+=",";
					}
				}
				reser+="]";
				System.out.println(reser);
				Query q4 =
						new Query(
								"fonction("+plat+","+reser+",Res)"
						);

				java.util.Map<String,Term> solution;

				solution = q4.oneSolution();
				System.out.println(solution.get("Res").toString());
					//ask prolog
					int reponse = 0;
					//compute response prolog

					reserve[0][type]--;

					DOS.writeInt(reponse);
			}
			sock.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}

