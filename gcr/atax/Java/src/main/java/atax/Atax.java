package ataxpackage;

import java.util.Arrays;
import java.lang.Math;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.Vector;
import java.lang.Thread;
import java.util.Random;
import java.util.concurrent.TimeUnit;

public class Atax {

   public String runAtax(String input_in, Integer threads_in) throws InterruptedException {
      String input = input_in;
      int N = 0;
      int M = 0;
      double fn = 5000.0;
      switch (input) {
         case "S":
            M = 2000;
            N = 2500;
            break;
         case "M":
            M = 4000;
            N = 5000;
            break;
         case "L":
            M = 20000;
            N = 22000;
            break;
         default:
            break;
      }

      double[] A = new double[N * M];
      double[] x = new double[N];
      double[] x_help = new double[N];
      /*
       * for(int i = 0; i < N; i++) { x[i] = 1.0 + (double)i / fn; } for (int i = 0; i
       * < M; i++) { for (int j = 0; j < N; j++) { A[i * N + j] = (double)((i + j) %
       * N) / (5.0 * (double)M); } }
       */
      // Arrays.fill(data, 0, data.length, generator.nextFloat());
      // float trace = 0;
      int threads = threads_in;
      if (threads > Runtime.getRuntime().availableProcessors()) {
         threads = Runtime.getRuntime().availableProcessors();
      }
      final float[] traces = new float[threads];
      Vector<Integer> count = new Vector<Integer>(0);
      ExecutorService executorService = Executors.newFixedThreadPool(threads); // number of threads
      // Init Matrix / Vector
      for (int i = 0; i < threads; i++) {
         // declare variables as final which will be used in method run below
         final int lowerBoundN = i * (N / threads);
         final int upperBoundN = (i + 1) * (N / threads);
         final int lowerBound = i * (M / threads);
         final int upperBound = (i + 1) * (M / threads);
         final int offset = N;
         final int other_offset = M;
         // final float[] traces = new float[4];
         final int index = i;
         executorService.submit(new Runnable() {
            @Override
            public void run() {
               for (int i = lowerBound; i < upperBound; i++) {
                  x[i] = 1.0 + (double) i / fn;
               }
               for (int row = lowerBound; row < upperBound; row++) {
                  for (int col = 0; col < offset; col++) {
                     A[row * offset + col] = (double) ((row + col) % offset) / (5.0 * (double) other_offset);
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

      long execTime = System.nanoTime();

      for (int i = 0; i < threads; i++) {
         // declare variables as final which will be used in method run below
         final int lowerBound = i * (M / threads);
         final int upperBound = (i + 1) * (M / threads);
         final int offset = N;
         final int index = i;
         executorService.submit(new Runnable() {
            @Override
            public void run() {
               for (int row = lowerBound; row < upperBound; row++) {
                  for (int col = 0; col < offset; col++) {
                     x_help[row] += A[row * offset + col] * x[col];
                  }
               }
               count.add(0);
               return;
            }
         });
      }
      while (count.size() < 2 * threads) {
         try {
            Thread.sleep(0);
         } catch (InterruptedException e) {
            System.out.println("Thread is interrupted");
         }
      }

      for (int i = 0; i < threads; i++) {
         // declare variables as final which will be used in method run below
         final int lowerBound = i * (M / threads);
         final int upperBound = (i + 1) * (M / threads);
         final int offset = N;
         final int index = i;
         executorService.submit(new Runnable() {
            @Override
            public void run() {
               for (int row = lowerBound; row < upperBound; row++) {
                  for (int col = 0; col < offset; col++) {
                     x[row] += A[row * offset + col] * x_help[col];
                  }
               }
               count.add(0);
               return;
            }
         });
      }

      while (count.size() < 3 * threads) {
         try {
            Thread.sleep(0);
         } catch (InterruptedException e) {
            System.out.println("Thread is interrupted");
         }
      }

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