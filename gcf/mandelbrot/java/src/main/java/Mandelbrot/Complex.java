package mandelbrotpackage;

public class Complex {
    public double real;
    public double imaginary;

    public Complex(double r, double i) {
        this.real = r;
        this.imaginary = i;
    }

    public Complex plus(Complex b) {
        Complex a = this;             // invoking object
        double re = a.real + b.real;
        double im = a.imaginary + b.imaginary;
        return new Complex(re, im);
    }

    public Complex times(Complex b) {
        Complex a = this;
        double re = a.real * b.real - a.imaginary * b.imaginary;
        double im = a.real * b.imaginary + a.imaginary * b.real;
        return new Complex(re, im);
    }

    public double abs() {
        return Math.hypot(this.real, this.imaginary);
    }

    public Complex pow(double x) {
        double modulus = Math.sqrt(this.real*this.real + this.imaginary*this.imaginary);
        double arg = Math.atan2(this.imaginary,this.real);
        double log_re = Math.log(modulus);
        double log_im = arg;
        double x_log_re = x * log_re;
        double x_log_im = x * log_im;
        double modulus_ans = Math.exp(x_log_re);
        return new Complex(modulus_ans*Math.cos(x_log_im), modulus_ans*Math.sin(x_log_im));
    }

}
