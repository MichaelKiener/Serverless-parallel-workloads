import java.util.concurrent.ForkJoinPool.ManagedBlocker;

import mandelbrotpackage.*;

public class Main {

    public static void main(String[] args) throws NumberFormatException, InterruptedException {
        Mandelbrot m = new Mandelbrot();
        String res = m.runMandelbrot(args[0], Integer.parseInt(args[1]));
        System.out.println("Res: " + res);
    }
}
