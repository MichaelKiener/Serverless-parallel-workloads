import mvtpackage.*;

public class Main {

        public static void main(String[] args) throws NumberFormatException, InterruptedException {
            MVT mvt = new MVT();
            String res = mvt.runMVT(args[0], Integer.parseInt(args[1]));
            System.out.println("Res: " + res);
        }
    }
