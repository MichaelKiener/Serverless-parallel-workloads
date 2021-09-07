package mandelbrotpackage;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.awt.image.*;
import java.awt.*;
import java.io.*;
import javax.imageio.*;
import java.io.OutputStream;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.Vector;
import java.lang.Thread;
import java.util.concurrent.TimeUnit;

import mandelbrotpackage.Complex;

public class Mandelbrot {
	int[][] colors = { { 15, 14, 29 }, { 30, 29, 59 }, { 45, 44, 89 }, { 60, 59, 119 }, { 75, 74, 148 },
			{ 90, 89, 178 }, { 105, 104, 208 }, { 120, 119, 238 }, { 108, 105, 211 }, { 96, 91, 184 }, { 84, 77, 158 },
			{ 72, 63, 131 }, { 60, 49, 104 }, { 48, 35, 78 }, { 36, 21, 51 }, { 24, 7, 25 }, { 45, 14, 25 },
			{ 67, 21, 25 }, { 88, 29, 26 }, { 110, 36, 26 }, { 132, 43, 26 }, { 153, 51, 27 }, { 175, 58, 27 },
			{ 197, 66, 28 }, { 176, 60, 25 }, { 155, 54, 23 }, { 134, 48, 21 }, { 113, 42, 19 }, { 92, 36, 17 },
			{ 71, 30, 15 }, { 50, 24, 13 }, { 29, 18, 11 }, { 42, 21, 18 }, { 55, 25, 26 }, { 68, 28, 33 },
			{ 82, 32, 41 }, { 95, 35, 48 }, { 108, 39, 56 }, { 121, 42, 63 }, { 135, 46, 71 }, { 121, 43, 63 },
			{ 107, 41, 56 }, { 93, 38, 49 }, { 79, 36, 42 }, { 65, 34, 34 }, { 51, 31, 27 }, { 37, 29, 20 },
			{ 24, 27, 13 }, { 51, 52, 27 }, { 78, 77, 41 }, { 105, 103, 56 }, { 132, 128, 70 }, { 159, 153, 84 },
			{ 186, 179, 99 }, { 213, 204, 113 }, { 241, 230, 128 }, { 213, 205, 115 }, { 185, 180, 102 },
			{ 157, 155, 89 }, { 129, 130, 76 }, { 101, 105, 63 }, { 73, 80, 50 }, { 45, 55, 37 }, { 17, 31, 24 },
			{ 44, 47, 38 }, { 72, 63, 52 }, { 100, 80, 67 }, { 128, 96, 81 }, { 156, 112, 95 }, { 184, 129, 110 },
			{ 212, 145, 124 }, { 240, 162, 139 }, { 211, 142, 125 }, { 182, 122, 111 }, { 154, 102, 98 },
			{ 125, 83, 84 }, { 96, 63, 70 }, { 68, 43, 57 }, { 39, 23, 43 }, { 11, 4, 30 }, { 22, 14, 49 },
			{ 34, 24, 69 }, { 46, 35, 89 }, { 58, 45, 109 }, { 70, 55, 129 }, { 82, 66, 149 }, { 94, 76, 169 },
			{ 106, 87, 189 }, { 96, 78, 167 }, { 86, 70, 145 }, { 77, 62, 123 }, { 67, 54, 101 }, { 57, 45, 79 },
			{ 48, 37, 57 }, { 38, 29, 35 }, { 29, 21, 14 }, { 26, 35, 27 }, { 24, 50, 40 }, { 22, 65, 53 },
			{ 20, 80, 66 }, { 18, 95, 79 }, { 16, 110, 92 }, { 14, 125, 105 }, { 12, 140, 118 }, { 11, 123, 106 },
			{ 11, 106, 95 }, { 11, 89, 84 }, { 11, 73, 73 }, { 10, 56, 62 }, { 10, 39, 51 }, { 10, 22, 40 },
			{ 10, 6, 29 }, { 15, 23, 35 }, { 20, 40, 41 }, { 25, 57, 47 }, { 30, 75, 53 }, { 35, 92, 59 },
			{ 40, 109, 65 }, { 45, 126, 71 }, { 50, 144, 77 }, { 46, 126, 70 }, { 43, 108, 63 }, { 39, 90, 57 },
			{ 36, 72, 50 }, { 32, 54, 43 }, { 29, 36, 37 }, { 25, 18, 30 }, { 22, 0, 24 }, { 37, 23, 51 },
			{ 53, 47, 78 }, { 69, 70, 106 }, { 85, 94, 133 }, { 100, 117, 160 }, { 116, 141, 188 }, { 132, 164, 215 },
			{ 148, 188, 243 }, { 130, 168, 213 }, { 112, 149, 184 }, { 94, 129, 154 }, { 76, 110, 125 }, { 58, 90, 95 },
			{ 40, 71, 66 }, { 22, 51, 36 }, { 4, 32, 7 }, { 32, 46, 7 }, { 60, 60, 8 }, { 89, 74, 9 }, { 117, 89, 10 },
			{ 145, 103, 11 }, { 174, 117, 12 }, { 202, 131, 13 }, { 231, 146, 14 }, { 203, 129, 14 }, { 175, 112, 15 },
			{ 148, 96, 16 }, { 120, 79, 17 }, { 92, 62, 17 }, { 65, 46, 18 }, { 37, 29, 19 }, { 10, 13, 20 },
			{ 31, 29, 26 }, { 53, 46, 32 }, { 75, 63, 38 }, { 97, 80, 44 }, { 118, 96, 50 }, { 140, 113, 56 },
			{ 162, 130, 62 }, { 184, 147, 68 }, { 162, 132, 59 }, { 141, 117, 51 }, { 119, 102, 43 }, { 98, 87, 35 },
			{ 77, 72, 27 }, { 55, 57, 19 }, { 34, 42, 11 }, { 13, 28, 3 }, { 32, 55, 21 }, { 52, 83, 40 },
			{ 71, 110, 58 }, { 91, 138, 77 }, { 110, 165, 96 }, { 130, 193, 114 }, { 149, 220, 133 }, { 169, 248, 152 },
			{ 148, 217, 137 }, { 127, 186, 122 }, { 107, 155, 107 }, { 86, 124, 93 }, { 65, 93, 78 }, { 45, 62, 63 },
			{ 24, 31, 48 }, { 4, 0, 34 }, { 11, 10, 35 }, { 18, 20, 37 }, { 25, 31, 39 }, { 33, 41, 41 },
			{ 40, 51, 42 }, { 47, 62, 44 }, { 54, 72, 46 }, { 62, 83, 48 }, { 55, 75, 44 }, { 48, 67, 41 },
			{ 41, 59, 38 }, { 34, 52, 35 }, { 27, 44, 31 }, { 20, 36, 28 }, { 13, 28, 25 }, { 7, 21, 22 },
			{ 25, 30, 42 }, { 43, 40, 62 }, { 61, 49, 82 }, { 79, 59, 103 }, { 97, 68, 123 }, { 115, 78, 143 },
			{ 133, 87, 163 }, { 152, 97, 184 }, { 134, 85, 162 }, { 116, 73, 141 }, { 98, 61, 119 }, { 80, 50, 98 },
			{ 62, 38, 76 }, { 44, 26, 55 }, { 26, 14, 33 } };

	public String runMandelbrot(String input, int threads) {

		if (threads > Runtime.getRuntime().availableProcessors()) {
			threads = Runtime.getRuntime().availableProcessors();
		}

		int x_resolution = 0;
		int y_resolution = 0;
		int max_iter = 50;
		double view_x0 = -2.0;
		double view_x1 = 2.0;
		double view_y0 = -2.0;
		double view_y1 = 2.0;
		double power = 2.0;
		String file_name = "mandelbrot.png";
		switch (input) {
			case "S":
				x_resolution = 256;
				y_resolution = 256;
				break;
			case "M":
				x_resolution = 1024;
				y_resolution = 1024;
				break;
			case "L":
				x_resolution = 3072;
				y_resolution = 3072;
				break;
			default:
				break;
		}
		double execTime = System.nanoTime();
		double x_stepsize = (view_x1 - view_x0) / (1.0 * (double) x_resolution);
		double y_stepsize = (view_y1 - view_y0) / (1.0 * (double) y_resolution);

		BufferedImage mandelbrotImage = new BufferedImage(x_resolution, y_resolution, BufferedImage.TYPE_INT_RGB);

		int[] pointsInSetCountArr = new int[threads];
		double[] x = new double[threads];
		double[] y = new double[threads];
		int[] step = new int[threads];
		Complex[] Z = new Complex[threads];
		Complex[] C = new Complex[threads];
		ExecutorService executorService = Executors.newFixedThreadPool(threads); // number of threads
		Vector<Integer> count = new Vector<Integer>(0);
		for (int i = 0; i < threads; i++) {
			final int lowerBound = i * (y_resolution / threads);
			final int upperBound = (i + 1) * (y_resolution / threads);
			final int x_res = x_resolution;
			x[i] = 0.0;
			y[i] = 0.0;
			final int index = i;
			final int max_it = max_iter;
			Z[index] = new Complex(0, 0);
			C[index] = new Complex(0, 0);
			step[i] = 0;
			executorService.submit(new Runnable() {
				@Override
				public void run() {
					for (int j = lowerBound; j < upperBound; j++) {
						for (int k = 0; k < x_res; k++) {
							y[index] = view_y1 - (double) j * y_stepsize;
							x[index] = view_x0 + (double) k * x_stepsize;

							Z[index] = new Complex(0, 0);
							C[index] = new Complex(x[index], 0).plus(new Complex(y[index], 0).times(new Complex(0, 1)));

							step[index] = 0;
							// Do while loop
							do {
								Z[index] = Z[index].pow(2).plus(C[index]);
								step[index]++;
							} while (Z[index].abs() < 2.0 && step[index] < max_it);

							if (step[index] == max_it) {
								pointsInSetCountArr[index]++;
								mandelbrotImage.setRGB(k, j, new Color(0, 0, 0).getRGB());
							} else {
								int ind = step[index] % colors.length / colors[0].length;
								int[] co = colors[ind];
								mandelbrotImage.setRGB(k, j, new Color(co[0], co[1], co[2]).getRGB());
							}
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

		int pointsInSetSum = 0;
		for (int i = 0; i < pointsInSetCountArr.length; i++) {
			pointsInSetSum += pointsInSetCountArr[i];
		}
		File outputfile = new File("/tmp/mandelbrot.png");
		try {
			ImageIO.write(mandelbrotImage, "png", outputfile);
		} catch (IOException e) {
			System.out.println(e.getMessage());
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
