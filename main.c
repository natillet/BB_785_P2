#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <arm_neon.h>

#define N 1000
#define STEPS 16

typedef signed int int32_t;
extern void enable_runfast();

float m[N], x[N], y[N], z[N], vx[N], vy[N], vz[N], xnew[N], ynew[N], znew[N];

static inline unsigned int get_cyclecount (void)
{
  unsigned int value;
  // Read CCNT Register
  asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(value));  
  return value;
}

static inline void init_perfcounters (int32_t do_reset, int32_t enable_divider)
{
  // in general enable all counters (including cycle counter)
  int32_t value = 1;

  // peform reset:  
  if (do_reset)
  {
    value |= 2;     // reset all counters to zero.
    value |= 4;     // reset cycle counter to zero.
  } 

  if (enable_divider)
    value |= 8;     // enable "by 64" divider for CCNT.

  value |= 16;

  // program the performance-counter control-register:
  asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(value));  

  // enable all counters:  
  asm volatile ("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r"(0x8000000f));  

  // clear overflows:
  asm volatile ("MCR p15, 0, %0, c9, c12, 3\t\n" :: "r"(0x8000000f));
}


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
  float invr[N], f, ax, ay, az, dx[N], dy[N], dz[N], dt=0.001;
  float eps=0.0000001;
  struct timespec t1, t2, d;
  FILE *fp;
  char *outputFilename = "results.txt";
  float in_sqrt[N];
  //float *p_in_sqrt = &in_sqrt[0];
  //float *p_invr = &invr[0];
  float32x4_t vec_invr;
  unsigned long long cycles;

  enable_runfast();
  init();

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
  init_perfcounters(1, 1);
  cycles = get_cyclecount();

  for (s=0; s<STEPS; s++) {
    for(i=0; i<N; i++) { /* Foreach particle "i" ... */
      ax=0.0f;
      ay=0.0f;
      az=0.0f;
      for(j=0; j<N; j++) { /* Loop over all particles "j" */
	      dx[j]=x[j]-x[i];
	      dy[j]=y[j]-y[i];
	      dz[j]=z[j]-z[i];
	      in_sqrt[j] = dx[j]*dx[j] + dy[j]*dy[j] + dz[j]*dz[j] + eps;
      }
	    for(j=0; j<N; j+=4) { /* Loop over all particles "j" */
              //invr[j] = 1.0f/sqrtf(in_sqrt[j]);
              vec_invr = vld1q_f32(&in_sqrt[j]);
              vec_invr = vrsqrteq_f32(vec_invr);
              vst1q_f32(&invr[j], vec_invr);
              //p_in_sqrt += 4;
              //p_invr += 4;
	    }
	    for(j=0; j<N; j++) { /* Loop over all particles "j" */
	      f=m[j]*invr[j]*invr[j]*invr[j];
	      ax += f*dx[j]; /* accumulate the acceleration from gravitational attraction */
	      ay += f*dy[j];
	      az += f*dx[j];
      }
      xnew[i] = x[i] + dt*vx[i] + 0.5f*dt*dt*ax; /* update position of particle "i" */
      ynew[i] = y[i] + dt*vy[i] + 0.5f*dt*dt*ay;
      znew[i] = z[i] + dt*vz[i] + 0.5f*dt*dt*az;
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
  cycles = get_cyclecount() - cycles;
  printf("Execution duration in cycles (rounded to the nearest 64 cycles): %llu\n", cycles*64);

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);

  // Print results to file so we can compare to ensure optimizations do not alter functionality
  fp = fopen(outputFilename, "w");
  if (fp == NULL) {
    fprintf(stderr, "Can't open output file %s!\n", outputFilename);
    exit(1);
  }
  for (i=0; i<N; i++) {
	fprintf(fp, "%f %f %f\n", x[i], y[i], z[i]);
  }
  fclose(fp);

  diff(&d, t1, t2);
//  printf("Execution Time: %ld sec, %ld nsec\n", d.tv_sec, d.tv_nsec);
  return 0;
}