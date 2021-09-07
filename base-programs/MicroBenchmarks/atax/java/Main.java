import ataxpackage.*;

public class Main {

        public static void main(String[] args) throws NumberFormatException, InterruptedException {
            Atax atax = new Atax();
            String res = atax.runAtax(args[0], Integer.parseInt(args[1]));
            System.out.println("Res: " + res);
        }
    }
