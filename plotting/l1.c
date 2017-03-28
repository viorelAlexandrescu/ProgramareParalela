#include <stdio.h>

#include <gsl/gsl_sf_bessel.h>

#define NMAX 500

int main(int argc, char ** argv) {
    if (argc != 2) {
        printf("Utilizare: %s nume_fisier_data\n", argv[0]);
        return 1;
    }
    FILE *f = fopen("f.txt", "wt");
    double dx = 20.0 / 500;
    for (double x = 0; x < 20; x += dx) {
        double r = gsl_sf_bessel_J0(x);
        fprintf(f, "%lf %lf\n", x, r);
    }
    return 0;
}
