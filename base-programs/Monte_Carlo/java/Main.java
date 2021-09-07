import montepackage.*;

public class Main {

        public static void main(String[] args) throws NumberFormatException, InterruptedException {
            Monte monte = new Monte();
            String res = monte.runMonte(args[0], Integer.parseInt(args[1]));
            System.out.println("Res: " + res);
        }
    }
