import gofastpackage.GoFast;

public class Main {

        public static void main(String[] args) throws NumberFormatException, InterruptedException {
            GoFast gf = new GoFast();
            String res = gf.runGoFast(args[0], Integer.parseInt(args[1]));
            System.out.println("Res: " + res);
        }
    }
