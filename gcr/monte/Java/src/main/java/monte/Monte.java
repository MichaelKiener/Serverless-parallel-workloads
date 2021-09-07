package montepackage;

import java.util.Arrays;
import java.lang.Math;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.Vector;
import java.lang.Thread;
import java.util.Random;
import java.util.concurrent.ThreadLocalRandom;
import java.util.concurrent.TimeUnit;
import java.lang.Math;

public class Monte {

   public String runMonte(String input_in, Integer threads_in) throws InterruptedException {
      String input = input_in;
      int iterations = 0;
      switch (input) {
         case "S":
            iterations = 10000000;
            break;
         case "M":
            iterations = 100000000;
            break;
         case "L":
            iterations = 1000000000;
            break;
         default:
            break;
      }

      long execTime = System.nanoTime();
      // float trace = 0;
      int threads = threads_in;
      if (threads > Runtime.getRuntime().availableProcessors()) {
         threads = Runtime.getRuntime().availableProcessors();
      }
      final int[] counts = new int[threads];
      /* final double[] x = new double[threads];
      final double[] y = new double[threads];
      final double[] z = new double[threads]; */
      double countSum = 0.0;
      ThreadLocalRandom[] generator = new ThreadLocalRandom[threads];
      ExecutorService executorService = Executors.newFixedThreadPool(threads); // number of threads
         Vector<Integer> count = new Vector<Integer>(0);
         for (int i = 0; i < threads; i++) {
            // declare variables as final which will be used in method run below
            final int lowerBound = i * (iterations / threads);
            final int upperBound = (i + 1) * (iterations / threads);
            // final float[] traces = new float[4];
            final int index = i;
            executorService.submit(new Runnable() {
               @Override
               public void run() {
                  generator[index] = ThreadLocalRandom.current();
                  for (int j = lowerBound; j < upperBound; j++) {
                     double x = generator[index].nextDouble(1);
                     double y = generator[index].nextDouble(1);
                    // x[index] = generator[index].nextDouble(1);
                    // y[index] = generator[index].nextDouble(1);
                    double z = Math.pow(x, 2.0) + Math.pow(y, 2.0);
                    //z[index] = Math.pow(x[index], 2) + Math.pow(y[index], 2);
                    if(z <= 1) {
                        counts[index]++;
                    }
                  }
                  count.add(0);
                  return;
               }
            });
         }
         while (count.size() < threads) {
            try {
               Thread.sleep(0);
            } catch (InterruptedException e) {
               System.out.println("Thread is interrupted");
            }
         }
         // Add counts
         countSum = 0;
         for (int res = 0; res < counts.length; res++) {
            countSum += counts[res];
         }

      double pi = 4 * countSum / iterations;

      System.out.println(pi);

      executorService.shutdown();
      execTime = System.nanoTime() - execTime;
      try {
         if (!executorService.awaitTermination(5000, TimeUnit.MILLISECONDS)) {
             executorService.shutdownNow();
         }
     } catch (InterruptedException e) {
         executorService.shutdownNow();
     }
      return "Execution Time: " + (double) execTime / 1000000 + "ms, Threads: " + threads;
   }
}