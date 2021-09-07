#include <string>

std::string runMandelbrot(std::string input, int thread_input);

int mandelbrot_draw(int x_resolution, int y_resolution, int max_iter,
                    double view_x0, double view_x1, double view_y0, double view_y1,
                    double x_stepsize, double y_stepsize,
                    int palette_shift, unsigned char *img,
                    double power, bool no_output);
