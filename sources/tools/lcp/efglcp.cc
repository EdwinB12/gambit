//
// $Source$
// $Date$
// $Revision$
//
// DESCRIPTION:
// Implementation of algorithm to solve extensive forms using linear
// complementarity program from sequence form
//
// This file is part of Gambit
// Copyright (c) 2002, The Gambit Project
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#include <unistd.h>
#include <iostream>
#include "libgambit/libgambit.h"

using namespace Gambit;

#include "lhtab.h"
#include "lemketab.h"


extern int g_numDecimals;

void PrintProfile(std::ostream &p_stream,
		  const std::string &p_label,
		  const MixedBehavProfile<double> &p_profile)
{
  p_stream << p_label;
  for (int i = 1; i <= p_profile.Length(); i++) {
    p_stream << "," << p_profile[i];
  }

  p_stream << std::endl;
}

void PrintProfile(std::ostream &p_stream,
		  const std::string &p_label,
		  const MixedBehavProfile<Rational> &p_profile)
{
  p_stream << p_label;
  for (int i = 1; i <= p_profile.Length(); i++) {
    p_stream.setf(std::ios::fixed);
    p_stream << "," << std::setprecision(g_numDecimals) << p_profile[i];
  }

  p_stream << std::endl;
}

template <class T> class SolveEfgLcp {
private:
  int m_stopAfter, m_maxDepth;

  int ns1,ns2,ni1,ni2;
  T maxpay,eps;
  List<BFS<T> > m_list;
  List<GameInfoset> isets1, isets2;

  void FillTableau(const BehavSupport &, Matrix<T> &, const GameNode &, T,
		   int, int, int, int);
  int AddBFS(const LTableau<T> &tab);
  int AllLemke(const BehavSupport &, int dup, LTableau<T> &B,
	       int depth, Matrix<T> &,
	       bool p_print, List<MixedBehavProfile<T> > &);
  
  void GetProfile(const BehavSupport &, const LTableau<T> &tab, 
		  MixedBehavProfile<T> &, const Vector<T> &, 
		  const GameNode &n, int,int);

public:
  SolveEfgLcp(void) : m_stopAfter(0), m_maxDepth(0) { }
  
  int StopAfter(void) const { return m_stopAfter; }
  void SetStopAfter(int p_stopAfter) { m_stopAfter = p_stopAfter; }

  int MaxDepth(void) const { return m_maxDepth; }
  void SetMaxDepth(int p_maxDepth) { m_maxDepth = p_maxDepth; }

  List<MixedBehavProfile<T> > Solve(const BehavSupport &,
				    bool p_print = true);
};


//---------------------------------------------------------------------------
//                        SolveEfgLcp: member functions
//---------------------------------------------------------------------------

//
// Sets the action probabilities at unreached information sets
// which are left undefined by the sequence form method to
// the centroid.  This helps IsNash and LiapValue work correctly.
//
template <class T>
void UndefinedToCentroid(MixedBehavProfile<T> &p_profile)
{
  Game efg = p_profile.GetGame();

  for (int pl = 1; pl <= efg->NumPlayers(); pl++) {
    GamePlayer player = efg->GetPlayer(pl);
    for (int iset = 1; iset <= player->NumInfosets(); iset++) {
      GameInfoset infoset = player->GetInfoset(iset);
      
      if (p_profile.GetInfosetProb(infoset) > (T) 0) {
	continue;
      }
	  
      T total = (T) 0;
      for (int act = 1; act <= infoset->NumActions(); act++) {
	total += p_profile.GetActionProb(infoset->GetAction(act));
      }

      if (total == (T) 0) {
	for (int act = 1; act <= infoset->NumActions(); act++) {
	  p_profile(pl, iset, act) = (T) 1.0 / (T) infoset->NumActions();
	}
      }
    }
  }
}


//
// Lemke implements the Lemke's algorithm (as refined by Eaves 
// for degenerate problems) for  Linear Complementarity
// problems, starting from the primary ray.  
//

template <class T> List<MixedBehavProfile<T> > 
SolveEfgLcp<T>::Solve(const BehavSupport &p_support, bool p_print /*= true*/)
{
  BFS<T> cbfs((T) 0);
  int i, j;

  isets1 = p_support.ReachableInfosets(p_support.GetGame()->GetPlayer(1));
  isets2 = p_support.ReachableInfosets(p_support.GetGame()->GetPlayer(2));

  m_list = List<BFS<T> >();

  int ntot;
  ns1 = p_support.NumSequences(1);
  ns2 = p_support.NumSequences(2);
  ni1 = p_support.GetGame()->GetPlayer(1)->NumInfosets()+1;
  ni2 = p_support.GetGame()->GetPlayer(2)->NumInfosets()+1;

  ntot = ns1+ns2+ni1+ni2;

  Matrix<T> A(1,ntot,0,ntot);
  Vector<T> b(1,ntot);

  maxpay = p_support.GetGame()->GetMaxPayoff() + Rational(1);

  T prob = (T)1;
  for (i = A.MinRow(); i <= A.MaxRow(); i++) {
    b[i] = (T) 0;
    for (j = A.MinCol(); j <= A.MaxCol(); j++) {
      A(i,j) = (T) 0; 
    }
  }

  FillTableau(p_support, A, p_support.GetGame()->GetRoot(), prob, 1, 1, 0, 0);
  for (i = A.MinRow(); i <= A.MaxRow(); i++) { 
    A(i,0) = -(T) 1;
  }
  A(1,ns1+ns2+1) = (T)1;
  A(ns1+ns2+1,1) = -(T)1;
  A(ns1+1,ns1+ns2+ni1+1) = (T)1;
  A(ns1+ns2+ni1+1,ns1+1) = -(T)1;
  b[ns1+ns2+1] = -(T)1;
  b[ns1+ns2+ni1+1] = -(T)1;

  LTableau<T> tab(A,b);
  eps = tab.Epsilon();
  
  MixedBehavProfile<T> profile(p_support);
  Vector<T> sol(tab.MinRow(),tab.MaxRow());
  List<MixedBehavProfile<T> > solutions;
  
  try {
    if (m_stopAfter != 1) {
      AllLemke(p_support, ns1+ns2+1, tab, 0, A, p_print, solutions);
    }
    else {
      tab.Pivot(ns1+ns2+1,0);
      tab.SF_LCPPath(ns1+ns2+1);
      
      AddBFS(tab);
      tab.BasisVector(sol);
      GetProfile(p_support, tab, 
		 profile,sol,p_support.GetGame()->GetRoot(),1,1);
      UndefinedToCentroid(profile);

      PrintProfile(std::cout, "NE", profile);
    }
  }
  catch (...) {
    // catch exception; return solutions computed (if any)
  }

  return solutions;
}

template <class T> int SolveEfgLcp<T>::AddBFS(const LTableau<T> &tableau)
{
  BFS<T> cbfs((T) 0);
  Vector<T> v(tableau.MinRow(), tableau.MaxRow());
  tableau.BasisVector(v);

  for (int i = tableau.MinCol(); i <= tableau.MaxCol(); i++)
    if (tableau.Member(i)) {
      cbfs.Define(i, v[tableau.Find(i)]);
    }

  if (m_list.Contains(cbfs))  return 0;
  m_list.Append(cbfs);
  return 1;
}

//
// All_Lemke finds all accessible Nash equilibria by recursively 
// calling itself.  List maintains the list of basic variables 
// for the equilibria that have already been found.  
// From each new accessible equilibrium, it follows
// all possible paths, adding any new equilibria to the List.  
//
template <class T> int 
SolveEfgLcp<T>::AllLemke(const BehavSupport &p_support,
			 int j, LTableau<T> &B, int depth,
			 Matrix<T> &A,
			 bool p_print,
			 List<MixedBehavProfile<T> > &p_solutions)
{
  if (m_maxDepth != 0 && depth > m_maxDepth) {
    return 1;
  }

  int i,newsol,missing;
  T small_num = (T)1/(T)1000;

  Vector<T> sol(B.MinRow(),B.MaxRow());
  MixedBehavProfile<T> profile(p_support);

  newsol =0;
  for (i = B.MinRow(); 
       i <= B.MaxRow()  && newsol == 0;
       i++) {
    if (i != j)  {
      LTableau<T> BCopy(B);
      A(i,0) = -small_num;
      BCopy.Refactor();

      if (depth == 0) {
	BCopy.Pivot(j, 0);
	missing = -j;
      }
      else {
	missing = BCopy.SF_PivotIn(0);
      }

      newsol = 0;

      if (BCopy.SF_LCPPath(-missing) == 1) {
	newsol = AddBFS(BCopy);
	BCopy.BasisVector(sol);
	GetProfile(p_support, BCopy, profile, sol,
		   p_support.GetGame()->GetRoot(), 1, 1);
	UndefinedToCentroid(profile);
	if (newsol) {
	  if (p_print) {
	    PrintProfile(std::cout, "NE", profile);
	  }
	  p_solutions.Append(profile);
	}
      }
      else {
	// gout << ": Dead End";
      }
      
      A(i,0) = (T) -1;
      if (newsol) {
	BCopy.Refactor();
	AllLemke(p_support, i, BCopy, depth+1, A, p_print, p_solutions);
      }
    }
  }
  
  return 1;
}

template <class T>
void SolveEfgLcp<T>::FillTableau(const BehavSupport &p_support, Matrix<T> &A,
				 const GameNode &n, T prob,
				 int s1, int s2, int i1, int i2)
{
  int snew;
  GameOutcome outcome = n->GetOutcome();
  if (outcome) {
    A(s1,ns1+s2) = Rational(A(s1,ns1+s2)) +
      Rational(prob) * (outcome->GetPayoff<Rational>(1) - Rational(maxpay));
    A(ns1+s2,s1) = Rational(A(ns1+s2,s1)) +
      Rational(prob) * (outcome->GetPayoff<Rational>(2) - Rational(maxpay));
  }
  if (n->GetInfoset()) {
    if (n->GetPlayer()->IsChance()) {
      GameInfoset infoset = n->GetInfoset();
      for (int i = 1; i <= n->NumChildren(); i++) {
	FillTableau(p_support, A, n->GetChild(i),
		    Rational(prob) * infoset->GetActionProb<Rational>(i),
		    s1,s2,i1,i2);
      }
    }
    int pl = n->GetPlayer()->GetNumber();
    if (pl==1) {
      i1=isets1.Find(n->GetInfoset());
      snew=1;
      for (int i = 1; i < i1; i++) {
	snew+=p_support.NumActions(isets1[i]->GetPlayer()->GetNumber(),
				   isets1[i]->GetNumber());
      }
      A(s1,ns1+ns2+i1+1) = -(T)1;
      A(ns1+ns2+i1+1,s1) = (T)1;
      for (int i = 1; i <= p_support.NumActions(n->GetInfoset()->GetPlayer()->GetNumber(), n->GetInfoset()->GetNumber()); i++) {
	A(snew+i,ns1+ns2+i1+1) = (T)1;
	A(ns1+ns2+i1+1,snew+i) = -(T)1;
	FillTableau(p_support, A, n->GetChild(p_support.GetAction(n->GetInfoset()->GetPlayer()->GetNumber(), n->GetInfoset()->GetNumber(), i)->GetNumber()),prob,snew+i,s2,i1,i2);
      }
    }
    if(pl==2) {
      i2=isets2.Find(n->GetInfoset());
      snew=1;
      for (int i = 1; i < i2; i++) {
	snew+=p_support.NumActions(isets2[i]->GetPlayer()->GetNumber(),
				   isets2[i]->GetNumber());
      }
      A(ns1+s2,ns1+ns2+ni1+i2+1) = -(T)1;
      A(ns1+ns2+ni1+i2+1,ns1+s2) = (T)1;
      for (int i = 1; i <= p_support.NumActions(n->GetInfoset()->GetPlayer()->GetNumber(), n->GetInfoset()->GetNumber()); i++) {
	A(ns1+snew+i,ns1+ns2+ni1+i2+1) = (T)1;
	A(ns1+ns2+ni1+i2+1,ns1+snew+i) = -(T)1;
	FillTableau(p_support, A, n->GetChild(p_support.GetAction(n->GetInfoset()->GetPlayer()->GetNumber(), n->GetInfoset()->GetNumber(), i)->GetNumber()),prob,s1,snew+i,i1,i2);
      }
    }
    
  }
}


template <class T>
void SolveEfgLcp<T>::GetProfile(const BehavSupport &p_support,
				const LTableau<T> &tab, 
				MixedBehavProfile<T> &v, 
				const Vector<T> &sol,
				const GameNode &n, int s1,int s2)
{
  if (n->GetInfoset()) {
    int pl = n->GetPlayer()->GetNumber();
    int iset = n->GetInfoset()->GetNumber();

    if (n->GetPlayer()->IsChance()) {
      for (int i = 1; i <= n->NumChildren(); i++) {
	GetProfile(p_support, tab, v, sol, n->GetChild(i), s1, s2);
      }
    }
    else if (pl == 1) {
      int inf = isets1.Find(n->GetInfoset());
      int snew = 1;
      for (int i = 1; i < inf; i++) {
	snew += p_support.NumActions(1, isets1[i]->GetNumber()); 
      }
      
      for (int i = 1; i <= p_support.NumActions(pl, iset); i++) {
	v(pl,inf,i) = (T) 0;
	if (tab.Member(s1)) {
	  int ind = tab.Find(s1);
	  if (sol[ind] > eps) {
	    if (tab.Member(snew+i)) {
	      int ind2 = tab.Find(snew+i);
	      if (sol[ind2] > eps) {
		v(pl,inf,i) = sol[ind2] / sol[ind];
	      }
	    }
	  } 
	} 
	GetProfile(p_support, tab, v, sol,
		   n->GetChild(p_support.GetAction(pl, iset, i)->GetNumber()),
		   snew+i, s2);
      }
    }
    else if (pl == 2) { 
      int inf = isets2.Find(n->GetInfoset());
      int snew = 1;
      for (int i = 1; i < inf; i++) {
	snew += p_support.NumActions(2, isets2[i]->GetNumber()); 
      }

      for (int i = 1; i<= p_support.NumActions(pl, iset); i++) {
	v(pl,inf,i) = (T) 0;
	if (tab.Member(ns1+s2)) {
	  int ind = tab.Find(ns1+s2);
	  if (sol[ind] > eps) {
	    if (tab.Member(ns1+snew+i)) {
	      int ind2 = tab.Find(ns1+snew+i);
	      if (sol[ind2] > eps) {
		v(pl,inf,i) = sol[ind2] / sol[ind];
	      }
	    }
	  } 
	} 
	GetProfile(p_support, tab, v, sol,
		   n->GetChild(p_support.GetAction(pl, iset, i)->GetNumber()),
		   s1, snew+i);
      }
    }
  }
}

template <class T>
List<MixedBehavProfile<T> > SolveExtensive(const BehavSupport &p_support)
{
  SolveEfgLcp<T> algorithm;
  return algorithm.Solve(p_support);
}

template List<MixedBehavProfile<double> > 
SolveExtensive(const BehavSupport &);
template List<MixedBehavProfile<Rational> > 
SolveExtensive(const BehavSupport &);


template <class T>
List<MixedBehavProfile<T> > SolveExtensiveSilent(const BehavSupport &p_support)
{
  SolveEfgLcp<T> algorithm;
  return algorithm.Solve(p_support, false);
}

template List<MixedBehavProfile<double> > 
SolveExtensiveSilent(const BehavSupport &);
template List<MixedBehavProfile<Rational> > 
SolveExtensiveSilent(const BehavSupport &);


