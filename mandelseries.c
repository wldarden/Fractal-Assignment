#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Part 1: Multiple Processes
// Now, write a new program mandelseries that runs mandel 50 times, using what you
// learned in your shell program. Keep the -x and -y values the same as in your chosen
// image above, but allow -s to vary from an initial value of 2 all the way down to your
// target value. The end result should be 50 images named mandel1.bmp, mandel2.bmp,
// etc.
// Generating all those frames will take some time. We can speed up the process
// significantly by using multiple processes simultaneously.

// To do this, make mandelseries accept a single argument: the number of processes to run
// simultaneously. So, mandelseries 3 should start three mandel processes at once, then
// wait for one to complete. As soon as one completes, start the next, and keep going until
// all the work is complete. mandelseries should work correctly for any arbitrary
// number of processes given on the command line.

int main (int argc, char *argv[]) {
  int nProcs = 3; // input arg of how many proccesses to have
  int cProcs = 0; // current running mandel procs
  double nMandels = 10; // how many mandel pictures to make
  int cMandel = 0; // current index of Mandel picture
  char cBuffer[200]; // holds mandel command string
  char command[] = "./mandel";
  char outfile[30];
  int w = 2000;
  int h = 2000;
  int m = 1075;
  long double x = .330658;
  long double y = .42685;
  long double maxS = 2; // least zoom
  long double minS = .00000005; // farthest zoom
  long double cS = maxS; // current s
  char sw[25], sh[25], sm[25], sx[25], sy[25], ss[25]; // string version of arguments
  long double step = (maxS - minS) / (nMandels-1); // s change size
  printf("step: %1.15Lf, %1.15Lf, %1.15Lf\n", step, (maxS - minS), minS);
  int status;
  //create proccesses
  while (cMandel < nMandels) {
    if (cProcs >= nProcs) { //max procs. wait for any one to finish
      wait(&status);
      cProcs--;
    }
    cProcs++;
    pid_t pid = fork();
    if (pid == 0) {
      //in child. execute mandel process
      // generate command
      sprintf(outfile, "-o mandel-%i.bmp", cMandel);
      sprintf(sw, "-W %i", w);
      sprintf(sh, "-H %i", h);
      sprintf(sx, "-x %1.15Lf", x);
      sprintf(sy, "-y %1.15Lf", y);
      sprintf(sm, "-m %i", m);
      sprintf(ss, "-s %1.15Lf", cS);
      execl("./mandel", command, sw, sh, sx, sy, sm, ss, outfile, (char *) NULL);
      printf( "Error exec failed: Mandel %i\n", cMandel);
      exit(1);
    } else if (pid == -1) {
      printf("Error forking process: Mandel %i\n", cMandel);
    }
    cMandel++;
    cS -= step;
  }



}
