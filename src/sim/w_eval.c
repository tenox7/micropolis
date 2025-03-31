#include "sim.h"


char *cityClassStr[6] = {
  "VILLAGE", "TOWN", "CITY", "CAPITAL", "METROPOLIS", "MEGALOPOLIS"
};

char *cityLevelStr[3] = {
  "Easy", "Medium", "Hard"
};

char *probStr[10] = {
  "CRIME", "POLLUTION", "HOUSING COSTS", "TAXES",
  "TRAFFIC", "UNEMPLOYMENT", "FIRES"
};


/* comefrom: DoSubUpDate scoreDoer */
doScoreCard(void)
{
  char title[256],
  goodyes[32], goodno[32],
  prob0[32], prob1[32], prob2[32], prob3[32],
  pop[32], delta[32], assessed[32], assessed_dollars[32],
  score[32], changed[32];

  /* send /EvaluationTitle /SetValue [ (titleStr year) ] */
  /* send /GoodJobPercents /SetValue [ (CityYes%) (CityNo%) ] */
  /* send /WorstProblemPercents /SetValue [ (ProblemVotes[0]%) ... ] */
  /* send /WorstProblemNames /SetValue [ (probStr[ProblemOrder[0]) ... ] */
  /*	put ^chars around first problem name to make it bold */
  /* send /Statistics /SetValue
	  [ (CityPop) (deltaCityPop) () (CityAssValue)
	    (cityClassStr[CityClass]) (cityLevelStr[GameLevel]) ] */
  /* send /CurrentScore /SetValue [ (CityScore) ] */
  /* send /AnnualChange /SetValue [ (deltaCityScore) ] */

  sprintf(title, "City Evaluation  %d", CurrentYear());
  sprintf(goodyes, "%d%%", CityYes);
  sprintf(goodno, "%d%%", CityNo);
  sprintf(prob0, "%d%%", ProblemVotes[ProblemOrder[0]]);
  sprintf(prob1, "%d%%", ProblemVotes[ProblemOrder[1]]);
  sprintf(prob2, "%d%%", ProblemVotes[ProblemOrder[2]]);
  sprintf(prob3, "%d%%", ProblemVotes[ProblemOrder[3]]);
  sprintf(pop, "%d", CityPop);
  sprintf(delta, "%d", deltaCityPop);
  sprintf(assessed, "%d", CityAssValue);
  makeDollarDecimalStr(assessed, assessed_dollars);

  sprintf(score, "%d", CityScore);
  sprintf(changed, "%d", deltaCityScore);

  SetEvaluation(changed, score,
		ProblemVotes[ProblemOrder[0]] ? probStr[ProblemOrder[0]] : " ",
		ProblemVotes[ProblemOrder[1]] ? probStr[ProblemOrder[1]] : " ",
		ProblemVotes[ProblemOrder[2]] ? probStr[ProblemOrder[2]] : " ",
		ProblemVotes[ProblemOrder[3]] ? probStr[ProblemOrder[3]] : " ",
		ProblemVotes[ProblemOrder[0]] ? prob0 : " ",
		ProblemVotes[ProblemOrder[1]] ? prob1 : " ",
		ProblemVotes[ProblemOrder[2]] ? prob2 : " ",
		ProblemVotes[ProblemOrder[3]] ? prob3 : " ",
		pop, delta, assessed_dollars,
		cityClassStr[CityClass], cityLevelStr[GameLevel],
		goodyes, goodno, title);
}


ChangeEval()
{
  EvalChanged = 1;
}


scoreDoer(void)
{
  if (EvalChanged) {
    doScoreCard();
    EvalChanged = 0;
  }
}


SetEvaluation(char *changed, char *score,
	      char *ps0, char *ps1, char *ps2, char *ps3,
	      char *pv0, char *pv1, char *pv2, char *pv3,
	      char *pop, char *delta, char *assessed_dollars,
	      char *cityclass, char *citylevel,
	      char *goodyes, char *goodno, char *title)
{
  char buf[2048];

  sprintf(buf, "UISetEvaluation {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s}",
	  changed, score,
	  ps0, ps1, ps2, ps3,
	  pv0, pv1, pv2, pv3,
	  pop, delta, assessed_dollars,
	  cityclass, citylevel,
	  goodyes, goodno, title);
  Eval(buf);
}


