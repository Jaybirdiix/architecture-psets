#ifndef __MAVENSCOREBOARD_H
#define __MAVENSCOREBOARD_H

#define VAU0 0
#define VAU1 1
#define VRU 2
#define VLU 3
#define VSU 4

class MavenScoreBoard
{
public:
  MavenScoreBoard();
  void step_cycle();
  void step(int delta);
  void execute(int vl, bool branch, int alu, int rs, int rt, int rd, bool b_rs, bool b_rt, bool b_rd);
  void flush();

//private:
  bool branch_chain;
  int cycle;
  int cp_cycle;
  int dep[32];
  int rport[5];
  int wport[5];
};

#endif // __MAVENSCOREBOARD_H
