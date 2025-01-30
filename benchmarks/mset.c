/* TEST mandelbrot
* DESCRIPTION mandelbrot set loop calculating z=z*z+c in complex numbers
* CATEGORY integer
* DEFINE TYPE={signed char,short,int,long,long long,unsigned char}
* DEFINE TYPE={unsigned short, unsigned int, unsigned long, unsigned long long}
* CATEGORY fp
* DEFINE TYPE={float,double,long double}
* END
*/
#include <bench.h>
static long maxiter = WORKLOAD;
typedef TYPE number_t;
static int
mand_calc (number_t cre,
	number_t cim,
	number_t pre,
	number_t pim)
{
	number_t rp = 0, ip = 0;
	unsigned long iter = maxiter;
	number_t zre, zim;
	zre = cre;
	zim = cim;
	while ((iter) && (rp + ip < 4))
	{
		ip = (zim * zim);
		zim = (zim * zre) * 2 + pim;
		rp = (zre * zre);
		zre = rp - ip + pre;
		iter--;
	}
	iter = maxiter - iter;
	return (iter);
}


#include <time.h>
float       	time0,time1;
void mandelbrot_integer(void)
{
	time0 = (float) clock() / CLOCKS_PER_SEC;
	int i;
	int loops = 0;
	number_t z1=ZERO;
	number_t z2=ZERO;
	number_t z3=ZERO;
	number_t z4=ZERO;
	i += mand_calc(z1, z2, z3, z4);
	USE (i);
	time1 = (float) clock() / CLOCKS_PER_SEC;
    printf ("\n ELAPSED TIME: %fs\n", time1-time0);
}

SINGLE_BENCHMARK(mandelbrot_integer())
