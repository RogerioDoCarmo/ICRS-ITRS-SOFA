#include <stdio.h>
#include <stdlib.h>

#include "lib/sofa/sofa.h"

#define LINE_COUNT 98

int main() {
    // Text-file line
    int epoch; char sat[3]; double hour; double Xt, Yt, Zt, clock;

    FILE * f_itrs_gps = NULL;
    FILE * f_gcrs_gps = NULL;

    f_itrs_gps = fopen("G10.txt", "r");
    f_gcrs_gps = fopen("G10_CELESTE.txt","w");

    if (f_itrs_gps == NULL) {
        printf("Erro ao ler o arquivo!");
        exit(EXIT_FAILURE);
    }
    if (f_gcrs_gps == NULL) {
        printf("Erro ao criar o arquivo!");
        exit(EXIT_FAILURE);
    }

    int YEAR, MONTH, DAY_MONTH;
    double mjd0, mjd, tai, utc, tt;

    YEAR       = 2018;
    MONTH      = 8;
    DAY_MONTH  = 7;

    // Conversion between Gregorian date to Julian Date
    iauCal2jd(YEAR,MONTH,DAY_MONTH, &mjd0, &mjd);

    int i = 2;
    for (    ; i < LINE_COUNT; i++) {
        fscanf(f_itrs_gps, "%d %s %lf %lf %lf %lf", &epoch, sat, hour, Xt, Yt, Zt, clock);

        utc = hour;
        tai = utc + (  37   / 3600);
        tt  = tai + (32.184 / 3600);
        tt  = tt  /  24;
        mjd = mjd + tt;

        // TO CIP
        double x, y, s;
        iauXys06a(mjd0, mjd, &x, &y, &s);

        // Q(t)
        double qt[3][3];
        iauC2ixys(x, y, s, &qt);

        // THETA (ERA)
        double theta = iauEra00(mjd0, mjd);

        // R(t)
        double rt[3][3];
        iauIr(rt);
        iauRz(theta, &rt);

        // s
        double sp = iauSp00(mjd0, mjd);

        // W(t)
        double wt[3][3], xp, yp;
        // Xp Yp from IEARS:
        xp = 200.636; //0.2012;
        yp = 398.585; //0.3987;

        xp = xp * DMAS2R;
        yp = yp * DMAS2R;

        iauPom00(xp, yp, sp, &wt);

        // Q(t) x R(t) x W(t)
        double interm[3][3], transform[3][3];

        iauRxr(wt, rt, &interm);
        iauRxr(interm, qt, &transform);

        double gcrs[3][3];
        iauTr(transform, &gcrs);

        double Xc, Yc, Zc;

        Xc = Xt * gcrs[0][1] + Yt * gcrs[0][1] + Zt * gcrs[0][2];
        Yc = Xt * gcrs[1][0] + Yt * gcrs[1][1] + Zt * gcrs[1][2];
        Zc = Xt * gcrs[2][0] + Yt * gcrs[2][1] + Zt * gcrs[2][2];

        fprintf(f_gcrs_gps, "%f %f %f %f \n", hour, Xc, Yc, Zc);
    }

    fclose(f_itrs_gps);
    fclose(f_gcrs_gps);

    printf("Pressione qualquer tecla para finalizar... ");
    getchar();

    return 0;
}
