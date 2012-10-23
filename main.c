#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000
#define STEPS 16

float m[N], x[N], y[N], z[N], vx[N], vy[N], vz[N], xnew[N], ynew[N], znew[N];

void  diff(struct timespec * difference, struct timespec start, struct timespec end)
{
  if ((end.tv_nsec-start.tv_nsec)<0) {
    difference->tv_sec = end.tv_sec-start.tv_sec-1;
    difference->tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
  } else {
    difference->tv_sec = end.tv_sec-start.tv_sec;
    difference->tv_nsec = end.tv_nsec-start.tv_nsec;
  }
}

void init(void) {
  int i;

  for(i=0; i<N; i++) { /* Foreach particle "i" ... */
    x[i] = rand();
    y[i] = rand();
    z[i] = rand();
    vx[i] = rand()/100;
    vy[i] = rand()/100;
    vz[i] = rand()/100;
    m[i] = rand();
  }
}

int main (int argc, char * argv[]) {
  int s,i,j;
  float invr, invr3, f, ax, ay, az, dx, dy, dz, dt=0.001;
  float eps=0.0000001;
  struct timespec t1, t2, d;

  init();

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);

  for (s=0; s<STEPS; s++) {
    for(i=0; i<N; i++) { /* Foreach particle "i" ... */
      ax=0.0;
      ay=0.0;
      az=0.0;
      for(j=0; j<N; j++) { /* Loop over all particles "j" */
	dx=x[j]-x[i];
	dy=y[j]-y[i];
	dz=z[j]-z[i];
	invr = 1.0/sqrt(dx*dx + dy*dy + dz*dz + eps);
	invr3 = invr*invr*invr;
	f=m[j]*invr3;
	ax += f*dx; /* accumulate the acceleration from gravitational attraction */
	ay += f*dy;
	az += f*dx;
      }
      xnew[i] = x[i] + dt*vx[i] + 0.5*dt*dt*ax; /* update position of particle "i" */
      ynew[i] = y[i] + dt*vy[i] + 0.5*dt*dt*ay;
      znew[i] = z[i] + dt*vz[i] + 0.5*dt*dt*az;
      vx[i] += dt*ax; /* update velocity of particle "i" */
      vy[i] += dt*ay;
      vz[i] += dt*az;
    }
    for(i=0;i<N;i++) { /* copy updated positions back into original arrays */
      x[i] = xnew[i];
      y[i] = ynew[i];
      z[i] = znew[i];
    }
  }
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);

  diff(&d, t1, t2);
  printf("Execution Time: %ld sec, %ld nsec\n", d.tv_sec, d.tv_nsec);
  return 0;
}
