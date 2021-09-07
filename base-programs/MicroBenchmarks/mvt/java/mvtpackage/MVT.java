package mvtpackage;

import java.util.Arrays;
import java.lang.Math;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.Vector;
import java.lang.Thread;
import java.util.Random;
import java.util.concurrent.TimeUnit;

public class MVT {

   public String runMVT(String input_in, Integer threads_in) throws InterruptedException {
      String input = input_in;
      int N = 0;
      switch (input) {
         case "S":
            N = 2000;
            break;
         case "M":
            N = 5500;
            break;
         case "L":
            N = 18000;
            break;
         default:
            break;
      }
      double[] A = new double[N * N];
      double[] x1 = new double[N];
      double[] x2 = new double[N];
      double[] y1 = new double[N];
      double[] y2 = new double[N];
      /* for (int i = 0; i < N; i++) {
         x1[i] = (double) (i % N) / (double) N;
         x2[i] = (double) ((i + 1) % N) / (double) N;
         y1[i] = (double) ((i + 3) % N) / (double) N;
         y2[i] = (double) ((i + 4) % N) / (double) N;
         for (int j = 0; j < N; j++) {
            A[i * N + j] = (double)((i * j) % N) / ((double)N);
         }
      } */
      // Arrays.fill(data, 0, data.length, generator.nextFloat());
      // float trace = 0;
      int threads = threads_in;
      if (threads > Runtime.getRuntime().availableProcessors()) {
         threads = Runtime.getRuntime().availableProcessors();
      }
      Vector<Integer> count = new Vector<Integer>(0);
      ExecutorService executorService = Executors.newFixedThreadPool(threads); // number of threads

      for (int i = 0; i < threads; i++) {
         // declare variables as final which will be used in method run below
         final int lowerBound = i * (N / threads);
         final int upperBound = (i + 1) * (N / threads);
         final int offset = N;
         // final float[] traces = new float[4];
         final int index = i;
         executorService.submit(new Runnable() {
            @Override
            public void run() {
               for (int row = lowerBound; row < upperBound; row++) {
                  x1[row] = (double) (row % offset) / (double) offset;
                  x2[row] = (double) ((row + 1) % offset) / (double) offset;
                  y1[row] = (double) ((row + 3) % offset) / (double) offset;
                  y2[row] = (double) ((row + 4) % offset) / (double) offset;
                  for (int col = 0; col < offset; col++) {
                     A[row * offset + col] = (double)((row * col) % offset) / ((double) offset);
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
         final int lowerBound = i * (N / threads);
         final int upperBound = (i + 1) * (N / threads);
         final int offset = N;
         // final float[] traces = new float[4];
         final int index = i;
         executorService.submit(new Runnable() {
            @Override
            public void run() {
               // System.out.println("Initiating Thread: " + index + " with lower bound: " +
               // lowerBound + " and upper bound: " + upperBound);
               for (int row = lowerBound; row < upperBound; row++) {
                  for (int col = 0; col < offset; col++) {
                     x1[row] += A[row * offset + col] * y1[col];
                     x2[row] += A[col * offset + row] * y2[col];
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