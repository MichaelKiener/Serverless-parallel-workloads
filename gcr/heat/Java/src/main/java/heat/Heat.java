package heatpackage;

import java.util.Arrays;
import java.lang.Math;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.Vector;
import java.lang.Thread;
import java.util.Random;
import java.util.concurrent.TimeUnit;

public class Heat {

   public class HeatSrc {
      public double posX;
      public double posY;
      public double range;
      public double temp;

      public HeatSrc(double x, double y, double r, double t) {
         this.posX = x;
         this.posY = y;
         this.range = r;
         this.temp = t;
      }
   }

   public String runHeat(String input_in, Integer threads_in) throws InterruptedException {
      String input = input_in;

      System.out.println("Input: " + input);
      int N = 0;
      int iterations = 10;
      switch (input) {
         case "S":
            N = 200;
            break;
         case "M":
            N = 5200;
            break;
         case "L":
            N = 15200;
            break;
         default:
            break;
      }
      int threads = threads_in;
      if (threads > Runtime.getRuntime().availableProcessors()) {
         threads = Runtime.getRuntime().availableProcessors();
      }

      // double[] A_help = new double [N * N];

      // Initiate Heatsrces
      HeatSrc[] heatSrces = new HeatSrc[2];
      heatSrces[0] = new HeatSrc(0.0, 0.0, 1.0, 1.0);
      heatSrces[1] = new HeatSrc(1.0, 1.0, 1.0, 0.5);

      int np = N + 2;
      double[] A = new double[np * np];
      double[] A_help = new double[np * np];
      // double dist;

      ExecutorService executorService = Executors.newFixedThreadPool(threads);
      Vector<Integer> countInit = new Vector<Integer>(0);
      for (int t = 0; t < threads; t++) {
         // declare variables as final which will be used in method run below
         final int lowerBound = t * (np / threads);
         final int upperBound = (t + 1) * (np / threads);
         final int offset = np;
         // final float[] traces = new float[4];
         final int index = t;
         final double[] A_ref = A;
         final double[] A_ref_help = A_help;
         executorService.submit(new Runnable() {
            @Override
            public void run() {
               double dist;
               for (int i = 0; i < heatSrces.length; i++) {
                  // Top Row
                  for (int j = lowerBound; j < upperBound; j++) {
                     dist = Math.sqrt(Math.pow((double) j / (double) (np - 1) - heatSrces[i].posX, 2)
                           + Math.pow(heatSrces[i].posY, 2));
                     if (dist <= heatSrces[i].range) {
                        A_ref[j] += (heatSrces[i].range - dist) / heatSrces[i].range * heatSrces[i].temp;
                     }
                  }

                  /* bottom row */
                  for (int j = lowerBound; j < upperBound; j++) {
                     dist = Math.sqrt(Math.pow((double) j / (double) (np - 1) - heatSrces[i].posX, 2)
                           + Math.pow(1 - heatSrces[i].posY, 2));

                     if (dist <= heatSrces[i].range) {
                        A_ref[(np - 1) * np + j] += (heatSrces[i].range - dist) / heatSrces[i].range * heatSrces[i].temp;
                     }
                  }

                  /* leftmost column */
                  for (int j = lowerBound + 1; j < upperBound - 1; j++) {
                     dist = Math.sqrt(Math.pow(heatSrces[i].posX, 2)
                           + Math.pow((double) j / (double) (np - 1) - heatSrces[i].posY, 2));

                     if (dist <= heatSrces[i].range) {
                        A_ref[j * np] += (heatSrces[i].range - dist) / heatSrces[i].range * heatSrces[i].temp;
                     }
                  }

                  /* rightmost column */
                  for (int j = lowerBound + 1; j < upperBound - 1; j++) {
                     dist = Math.sqrt(Math.pow(1 - heatSrces[i].posX, 2)
                           + Math.pow((double) j / (double) (np - 1) - heatSrces[i].posY, 2));

                     if (dist <= heatSrces[i].range) {
                        A_ref[j * np + (np - 1)] += (heatSrces[i].range - dist) / heatSrces[i].range * heatSrces[i].temp;
                     }
                  }
               }

               for (int copy = 0; copy < A_ref.length; copy++) {
                  A_ref_help[copy] = A_ref[copy];
               }
               countInit.add(0);
               return;
            }
         });
      }
      while (countInit.size() < threads) {
         try {
            Thread.sleep(0);
         } catch (InterruptedException e) {
            System.out.println("Thread is interrupted");
         }
      }
      // double[] A_help = A.clone();
      long execTime = System.nanoTime();
      /*
       * for(double a:A) { System.out.print(" " + a + " "); }
       */

      // Arrays.fill(data, 0, data.length, generator.nextFloat());

      // float trace = 0;

      final double[] residuals = new double[threads];
      double residualSum = 0.0;
      // number of threads
      for (int iter = 0; iter < iterations; iter++) {
         Vector<Integer> count = new Vector<Integer>(0);
         for (int i = 0; i < threads; i++) {
            // declare variables as final which will be used in method run below
            final int lowerBound = i * (N / threads) + 1;
            final int upperBound = (i + 1) * (N / threads) + 1;
            final int offset = np;
            // final float[] traces = new float[4];
            final int index = i;
            final double[] A_ref = A;
            final double[] A_ref_help = A_help;
            executorService.submit(new Runnable() {
               @Override
               public void run() {
                  double diff, val;
                  double resiudal_thread = 0;
                  for (int row = lowerBound; row < upperBound; row++) {
                     for (int col = 1; col < offset - 1; col++) {
                        // x_help[row] += A[row * offset + col] * x[col];
                        val = 0.25 * (A_ref[row * offset + col - 1] + A_ref[row * offset + col + 1]
                              + A_ref[(row - 1) * offset + col] + A_ref[(row + 1) * offset + col]);
                        diff = val - A_ref[row * offset + col];
                        A_ref_help[row * offset + col] = val;
                        resiudal_thread += diff * diff;
                     }
                  }
                  residuals[index] = resiudal_thread;
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
         // Add residuals
         residualSum = 0.0;
         for (int res = 0; res < residuals.length; res++) {
            residualSum += residuals[res];
            residuals[res] = 0.0;
         }
         double[] temp = A;
         A = A_help;
         A_help = temp;
      }
      // System.out.println(residualSum);
      executorService.shutdown();
      execTime = System.nanoTime() - execTime;
      try {
         if (!executorService.awaitTermination(5000, TimeUnit.MILLISECONDS)) {
            executorService.shutdownNow();
         }
      } catch (InterruptedException e) {
         executorService.shutdownNow();
      }
      return "Execution time: " + (double) execTime / 1000000 + "ms, Threads: " + threads;
   }
}