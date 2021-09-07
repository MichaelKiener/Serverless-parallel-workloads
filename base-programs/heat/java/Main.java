import heatpackage.*;

public class Main {

    public static void main(String[] args) throws NumberFormatException, InterruptedException {
        Heat heat = new Heat();
        String res = heat.runHeat(args[0], Integer.parseInt(args[1]));
        System.out.println("Res: " + res);

    }
}
