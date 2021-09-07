package gofastpackage;

import java.util.Arrays;
import java.lang.Math;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.Vector;
import java.lang.Thread;
import java.util.Random;
import java.util.concurrent.ThreadLocalRandom;
import java.util.concurrent.TimeUnit;

public class GoFast {

   public String runGoFast(String input_in, Integer threads_in) throws InterruptedException {
      String input = input_in;
      long execTime = System.nanoTime();
      System.out.println("Input: " + input);
      int N = 0;
      switch (input) {
         case "S":
            N = 2000;
            break;
         case "M":
            N = 6000;
            break;
         case "L":
            N = 22000;
            break;
         default:
            break;
      }
      double[] data = new double[N * N];

      // double trace = 0;
      int threads = threads_in;
      if (threads > Runtime.getRuntime().availableProcessors()) {
         threads = Runtime.getRuntime().availableProcessors();
      }
      System.out.println("Threads: " + threads);
      final double[] traces = new double[threads];
      Vector<Integer> count = new Vector<Integer>(0);
      ExecutorService executorService = Executors.newFixedThreadPool(threads); // number of threads
      for (int i = 0; i < threads; i++) {
         // declare variables as final which will be used in method run below
         final int lowerBound = i * (N / threads);
         final int upperBound = (i + 1) * (N / threads);
         final int offset = N;
         // final double[] traces = new double[4];
         final int index = i;
         executorService.submit(new Runnable() {
            @Override
            public void run() {
               // System.out.println("Initiating Thread: " + index + " with lower bound: " +
               // lowerBound + " and upper bound: " + upperBound);
               ThreadLocalRandom generators = ThreadLocalRandom.current();
               for (int j = lowerBound; j < upperBound; j++) {
                  for(int k = 0; k < offset; k++) {
                     data[j * offset + k] = generators.nextDouble(1);
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
      for (int i = 0; i < threads; i++) {
         // declare variables as final which will be used in method run below
         final int lowerBound = i * (N / threads);
         final int upperBound = (i + 1) * (N / threads);
         final int offset = N;
         // final double[] traces = new double[4];
         final int index = i;
         executorService.submit(new Runnable() {
            @Override
            public void run() {
               double trace_thread = 0;
               // System.out.println("Initiating Thread: " + index + " with lower bound: " +
               // lowerBound + " and upper bound: " + upperBound);
               for (int j = lowerBound; j < upperBound; j++) {
                 trace_thread += Math.tanh(data[j + j * offset]);
               }
               traces[index] = trace_thread;
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
      double tracesSum = 0;
      for (double element : traces) {
         tracesSum += element;
      }
      for (int i = 0; i < threads; i++) {
         // declare variables as final which will be used in method run below
         final int lowerBound = i * (N / threads);
         final int upperBound = (i + 1) * (N / threads);
         final double trace = tracesSum;
         final int offset = N;
         // final double[] traces = new double[4];
         executorService.submit(new Runnable() {
            @Override
            public void run() {
               // System.out.println("Initiating Thread: " + index + " with lower bound: " +
               // lowerBound + " and upper bound: " + upperBound);
               for (int j = lowerBound; j < upperBound; j++) {
                  for (int k = 0; k < offset; k++) {
                     data[j * offset + k] += trace;
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
      return "Execution time: " + (double) execTime / 1000000 + "ms, Threads: " + threads;
   }
}