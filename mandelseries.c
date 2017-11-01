#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>

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
// s =
// 2 = 0^2 + 0 + 2
// y = a(x-h)^2 + k
// h = nMandels
// k = minS
// 2 = a(0 - nMandels)^2 + minS
// 2 = a(nMandels^2) + minS
// (2 - minS)/nMandels^2 = a

long double getS(int x,long double a, int nMandels, long double minS) {
  return (a * ((x - nMandels) * (x - nMandels))) + minS;
}

int main (int argc, char *argv[]) {
  char c;
  int nProcs = 3; // default arg of how many proccesses to have
  int t = 3; // default threads
  double nMandels = 50; // how many mandel pictures to make
  int outfileOffset = 0; // for names
  if (argc > 5){
    printf("Usage: mandelseries <Process Count (def=3)> <thread count (def=3)> ");
    printf("<# Mandels (def=50)> <out index (def=0)>\n");
    exit(1);
  } else if (argc >= 2) {
    nProcs = atoi(argv[1]);
    if (argc >= 3) {
      t = atoi(argv[2]);
      if (argc >= 4) {
        nMandels = atoi(argv[3]);
        if (argc >= 5) {
          outfileOffset = atoi(argv[4]);
        }
      }
    }
  }

  int cProcs = 0; // current running mandel procs

  int cMandel = 0; // current index of Mandel picture
  char cBuffer[200]; // holds mandel command string
  char command[] = "./mandel";
  char outfile[30];
  int w = 1000;
  int h = 1000;
  int m = 5000;

  long double x = .32408; // .330658
  long double y = .40397; // .42685
  long double maxS = 2; // least zoom
  long double minS = .00000001; // farthest zoom //.000000005
  long double cS = maxS; // current s
  char sw[25], sh[25], sm[25], sx[25], sy[25], ss[25], st[25]; // string version of arguments
  // long double step = (maxS - minS) / (nMandels-1); // s change size
  int status;
  long double a = (maxS - minS) / ((nMandels - 1) * (nMandels - 1)); // calculate quadratic step consant
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
      sprintf(outfile, "-o mandel-%i.bmp", cMandel + outfileOffset);
      sprintf(sw, "-W %i", w);
      sprintf(sh, "-H %i", h);
      sprintf(sx, "-x %1.15Lf", x);
      sprintf(sy, "-y %1.15Lf", y);
      sprintf(sm, "-m %i", m);
      sprintf(ss, "-s %1.15Lf", cS);
      sprintf(st, "-n %i", t);
      execl("./mandel", command, sw, sh, sx, sy, sm, ss, st, outfile, (char *) NULL);
      printf( "Error exec failed: Mandel %i\n", cMandel);
      exit(1);
    } else if (pid == -1) {
      printf("Error forking process: Mandel %i\n", cMandel);
    }
    cMandel++;
    cS = getS(cMandel, a, nMandels-1, minS);
  }
  while (cProcs > 0) { //max procs. wait for any one to finish
    wait(&status);
    cProcs--;
  }


}
