#include <stdio.h>
#include <stdlib.h>

#include "lib/sofa/sofa.h"

int main()
{
    int YEAR, MONTH, DAY_MONTH;
    double mjd0, mjd;

    YEAR        = 2019;
    MONTH       = 8;
    DAY_MONTH   = 7;

    iauCal2jd(YEAR,MONTH,DAY_MONTH, &mjd0, &mjd);

    printf("%f",mjd);


    getchar();

    return 0;
}
