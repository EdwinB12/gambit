//
// FILE: gsmoper.cc -- implementations for GSM operator functions
//                     companion to GSM
//
// $Id$
//

#include <stdlib.h>
#include <ctype.h>

#include "gmisc.h"
#include "gsm.h"
#include "portion.h"
#include "gsmfunc.h"

#include "rational.h"
#include "gblock.h"

#include "mixedsol.h"
#include "behavsol.h"

#include "nfg.h"
#include "efg.h"

#include "system.h"
#include "gstack.h"
#include "gstring.h"


#include "gcmdline.h"


extern GSM* _gsm;




//---------------------------------------
//           IsNull
//---------------------------------------

Portion* GSM_IsNull(Portion** param)
{
  return new BoolValPortion(param[0]->Spec().Type == porNULL);
}


//---------------------------------------
//           Null
//---------------------------------------

Portion* GSM_Null(Portion** param)
{
  return new NullPortion(param[0]->Spec().Type);
}



//---------------------------------------
//           Randomize
//---------------------------------------

long _idum = -1;

Portion* GSM_Randomize_Integer(Portion** param)
{
  long _RandomSeed = ((IntPortion*) param[1])->Value();
  if(_RandomSeed > 0)
    _RandomSeed = -_RandomSeed;
  long v;
  if(_RandomSeed != 0)
    _idum = _RandomSeed;
  v = ran1(&_idum);
  return new IntValPortion(v);
}

Portion* GSM_Randomize_Float(Portion** param)
{
  long _RandomSeed = ((IntPortion*) param[1])->Value();
  if(_RandomSeed > 0)
    _RandomSeed = -_RandomSeed;
  double v;
  if(_RandomSeed != 0)
    _idum = _RandomSeed;
  v = (double) ran1(&_idum) / IM;
  return new FloatValPortion(v);
}

Portion* GSM_Randomize_Rational(Portion** param)
{
  long _RandomSeed = ((IntPortion*) param[1])->Value();
  if(_RandomSeed > 0)
    _RandomSeed = -_RandomSeed;
  gRational v;
  if(_RandomSeed != 0)
    _idum = _RandomSeed;
  v = (gRational) ran1(&_idum) / (gRational) IM; 
  return new RationalValPortion(v);
}



//-------------------------------------------
//              Assign
//-------------------------------------------


Portion* GSM_Assign(Portion** param)
{
  _gsm->PushRef(((TextPortion*) param[0])->Value());
  Portion* p = 0;
  Portion* result = 0;
  if( param[1]->IsReference() )
  {
    p = param[1]->RefCopy();
    result = param[1]->RefCopy();
  }
  else
  {
    p = param[1]->ValCopy();
    result = param[1]->ValCopy();
  }
  _gsm->Push( p );
  _gsm->Assign();
  return result;
}

Portion* GSM_UnAssign(Portion** param)
{
  _gsm->PushRef(((TextPortion*) param[0])->Value());
  return _gsm->UnAssignExt();
}

Portion* GSM_Fake(Portion**)
{
  assert(0);
  return 0;
}

//-------------------------------------------------------------------
//                      mathematical operators
//------------------------------------------------------------------


//---------------------- GSM_Add ---------------------------

Portion* GSM_Add_double(Portion** param)
{
  Portion* result = 0;
  result = new FloatValPortion
    (
     ((FloatPortion*) param[0])->Value() +
     ((FloatPortion*) param[1])->Value()
    );
  return result;
}

Portion* GSM_Add_int(Portion** param)
{
  Portion* result = 0;
  result = new IntValPortion
    (
     ((IntPortion*) param[0])->Value() +
     ((IntPortion*) param[1])->Value()
    );
  return result;
}

Portion* GSM_Add_gRational(Portion** param)
{
  Portion* result = 0;
  result = new RationalValPortion
    (
     ((RationalPortion*) param[0])->Value() +
     ((RationalPortion*) param[1])->Value()
    );
  return result;
}

Portion* GSM_Add_MixedFloat(Portion** param)
{
  Portion* result = 0;
  if(((MixedPortion<double>*) param[0])->Value() == 0 ||
     ((MixedPortion<double>*) param[1])->Value() == 0)
    return new ErrorPortion("Cannot operate on a null Mixed value");

  if(((MixedPortion<double>*) param[0])->Value()->Support() !=
     ((MixedPortion<double>*) param[1])->Value()->Support())
    return new ErrorPortion("Support mismatch");

  result = param[0]->ValCopy();
  (* (MixedSolution<double>*) ((MixedPortion<double>*) result)->Value()) +=
    (* (MixedSolution<double>*) ((MixedPortion<double>*) param[1])->Value());
  return result;
}

Portion* GSM_Add_MixedRational(Portion** param)
{
  Portion* result = 0;
  if(((MixedPortion<gRational>*) param[0])->Value() == 0 ||
     ((MixedPortion<gRational>*) param[1])->Value() == 0)
    return new ErrorPortion("Cannot operate on a null Mixed value");

  if(((MixedPortion<gRational>*) param[0])->Value()->Support() !=
     ((MixedPortion<gRational>*) param[1])->Value()->Support())
    return new ErrorPortion("Support mismatch");

  result = param[0]->ValCopy();
  (* (MixedSolution<gRational>*) ((MixedPortion<gRational>*) result)->Value()) +=
    (* (MixedSolution<gRational>*) ((MixedPortion<gRational>*) param[1])->Value());
  return result;
}


Portion* GSM_Add_BehavFloat(Portion** param)
{
  Portion* result = 0;
  if(((BehavPortion<double>*) param[0])->Value() == 0 ||
     ((BehavPortion<double>*) param[1])->Value() == 0)
    return new ErrorPortion("Cannot operate on a null Behav value");

  if(((BehavPortion<double>*) param[0])->Value()->Support() !=
     ((BehavPortion<double>*) param[1])->Value()->Support())
    return new ErrorPortion("Support mismatch");

  result = param[0]->ValCopy();
  (* (BehavSolution<double>*) ((BehavPortion<double>*) result)->Value()) +=
    (* (BehavSolution<double>*) ((BehavPortion<double>*) param[1])->Value());
  return result;
}

Portion* GSM_Add_BehavRational(Portion** param)
{
  Portion* result = 0;
  if(((BehavPortion<gRational>*) param[0])->Value() == 0 ||
     ((BehavPortion<gRational>*) param[1])->Value() == 0)
    return new ErrorPortion("Cannot operate on a null Mixed value");

  if(((BehavPortion<gRational>*) param[0])->Value()->Support() !=
     ((BehavPortion<gRational>*) param[1])->Value()->Support())
    return new ErrorPortion("Support mismatch");

  result = param[0]->ValCopy();
  (* (BehavSolution<gRational>*) ((BehavPortion<gRational>*) result)->Value()) +=
    (* (BehavSolution<gRational>*) ((BehavPortion<gRational>*) param[1])->Value());
  return result;
}


//--------------------- Concat ---------------------------

Portion* GSM_Concat_gString(Portion** param)
{
  Portion* result = 0;
  result = new TextValPortion
    (
     ((TextPortion*) param[0])->Value() +
     ((TextPortion*) param[1])->Value()
    );
  return result;
}

Portion* GSM_Concat_List(Portion** param)
{
  Portion* result = 0;
  int i;
  int append_result;

  // gBlock<Portion*>& p_value = ((ListPortion*) param[1])->Value();
  gList<Portion*>& p_value = ((ListPortion*) param[1])->Value();
  result = new ListValPortion(((ListPortion*) param[0])->Value());
  for(i = 1; i <= p_value.Length(); i++)
  {
    append_result = ((ListPortion*) result)->Append(p_value[i]->ValCopy());
    if(append_result == 0)
    {
      delete result;
      result = new ErrorPortion
	("Attempted concatenating lists of different types");
      break;
    }
  }
  return result;
}



//---------------------- GSM_Subtract ------------------------

Portion* GSM_Subtract_double(Portion** param)
{
  Portion* result = 0;
  result = new FloatValPortion
    (
     ((FloatPortion*) param[0])->Value() -
     ((FloatPortion*) param[1])->Value()
    );
  return result;
}

Portion* GSM_Subtract_int(Portion** param)
{
  Portion* result = 0;
  result = new IntValPortion
    (
     ((IntPortion*) param[0])->Value() -
     ((IntPortion*) param[1])->Value()
    );
  return result;
}

Portion* GSM_Subtract_gRational(Portion** param)
{
  Portion* result = 0;
  result = new RationalValPortion
    (
     ((RationalPortion*) param[0])->Value() -
     ((RationalPortion*) param[1])->Value()
    );
  return result;
}


Portion* GSM_Subtract_MixedFloat(Portion** param)
{
  Portion* result = 0;
  if(((MixedPortion<double>*) param[0])->Value() == 0 ||
     ((MixedPortion<double>*) param[1])->Value() == 0)
    return new ErrorPortion("Cannot operate on a null Mixed value");

  if(((MixedPortion<double>*) param[0])->Value()->Support() !=
     ((MixedPortion<double>*) param[1])->Value()->Support())
    return new ErrorPortion("Support mismatch");

  result = param[0]->ValCopy();
  (* (MixedSolution<double>*) ((MixedPortion<double>*) result)->Value()) -=
    (* (MixedSolution<double>*) ((MixedPortion<double>*) param[1])->Value());
  return result;
}

Portion* GSM_Subtract_MixedRational(Portion** param)
{
  Portion* result = 0;
  if(((MixedPortion<gRational>*) param[0])->Value() == 0 ||
     ((MixedPortion<gRational>*) param[1])->Value() == 0)
    return new ErrorPortion("Cannot operate on a null Mixed value");

  if(((MixedPortion<gRational>*) param[0])->Value()->Support() !=
     ((MixedPortion<gRational>*) param[1])->Value()->Support())
    return new ErrorPortion("Support mismatch");

  result = param[0]->ValCopy();
  (* (MixedSolution<gRational>*) ((MixedPortion<gRational>*) result)->Value()) -=
    (* (MixedSolution<gRational>*) ((MixedPortion<gRational>*) param[1])->Value());
  return result;
}


Portion* GSM_Subtract_BehavFloat(Portion** param)
{
  Portion* result = 0;
  if(((BehavPortion<double>*) param[0])->Value() == 0 ||
     ((BehavPortion<double>*) param[1])->Value() == 0)
    return new ErrorPortion("Cannot operate on a null Behav value");

  if(((BehavPortion<double>*) param[0])->Value()->Support() !=
     ((BehavPortion<double>*) param[1])->Value()->Support())
    return new ErrorPortion("Support mismatch");

  result = param[0]->ValCopy();
  (* (BehavSolution<double>*) ((BehavPortion<double>*) result)->Value()) -=
    (* (BehavSolution<double>*) ((BehavPortion<double>*) param[1])->Value());
  return result;
}

Portion* GSM_Subtract_BehavRational(Portion** param)
{
  Portion* result = 0;
  if(((BehavPortion<gRational>*) param[0])->Value() == 0 ||
     ((BehavPortion<gRational>*) param[1])->Value() == 0)
    return new ErrorPortion("Cannot operate on a null Mixed value");

  if(((BehavPortion<gRational>*) param[0])->Value()->Support() !=
     ((BehavPortion<gRational>*) param[1])->Value()->Support())
    return new ErrorPortion("Support mismatch");

  result = param[0]->ValCopy();
  (* (BehavSolution<gRational>*) ((BehavPortion<gRational>*) result)->Value()) -=
    (* (BehavSolution<gRational>*) ((BehavPortion<gRational>*) param[1])->Value());
  return result;
}



//------------------------- GSM_Multiply ---------------------------

Portion* GSM_Multiply_double(Portion** param)
{
  Portion* result = 0;
  result = new FloatValPortion
    (
     ((FloatPortion*) param[0])->Value() *
     ((FloatPortion*) param[1])->Value()
    );
  return result;
}

Portion* GSM_Multiply_int(Portion** param)
{
  Portion* result = 0;
  result = new IntValPortion
    (
     ((IntPortion*) param[0])->Value() *
     ((IntPortion*) param[1])->Value()
    );
  return result;
}

Portion* GSM_Multiply_gRational(Portion** param)
{
  Portion* result = 0;
  result = new RationalValPortion
    (
     ((RationalPortion*) param[0])->Value() *
     ((RationalPortion*) param[1])->Value()
    );
  return result;
}


Portion* GSM_Multiply_MixedFloat(Portion** param)
{
  Portion* result = 0;
  if(((MixedPortion<double>*) param[1])->Value() == 0)
    return new ErrorPortion("Cannot operate on a null Mixed value");
  
  result = param[1]->ValCopy();
  (* (MixedSolution<double>*) ((MixedPortion<double>*) result)->Value()) *=
    ((FloatPortion*) param[0])->Value();
  return result;
}

Portion* GSM_Multiply_MixedRational(Portion** param)
{
  Portion* result = 0;
  if(((MixedPortion<gRational>*) param[1])->Value() == 0)
    return new ErrorPortion("Cannot operate on a null Mixed value");
  
  result = param[1]->ValCopy();
  (* (MixedSolution<gRational>*) ((MixedPortion<gRational>*) result)->Value()) *=
    ((RationalPortion*) param[0])->Value();
  return result;
}

Portion* GSM_Multiply_BehavFloat(Portion** param)
{
  Portion* result = 0;
  if(((BehavPortion<double>*) param[1])->Value() == 0)
    return new ErrorPortion("Cannot operate on a null Behav value");
  
  result = param[1]->ValCopy();
  (* (BehavSolution<double>*) ((BehavPortion<double>*) result)->Value()) *=
    ((FloatPortion*) param[0])->Value();
  return result;
}

Portion* GSM_Multiply_BehavRational(Portion** param)
{
  Portion* result = 0;
  if(((BehavPortion<gRational>*) param[1])->Value() == 0)
    return new ErrorPortion("Cannot operate on a null Behav value");
  
  result = param[1]->ValCopy();
  (* (BehavSolution<gRational>*) ((BehavPortion<gRational>*) result)->Value()) *=
    ((RationalPortion*) param[0])->Value();
  return result;
}


//---------------------------- GSM_Divide -------------------------------

Portion* GSM_Divide_double(Portion** param)
{
  Portion* result = 0;
  if(((FloatPortion*) param[1])->Value() != 0)
  {
    result = new FloatValPortion
      (
       ((FloatPortion*) param[0])->Value() /
       ((FloatPortion*) param[1])->Value()
      );
  }
  else
  {
    result = new NullPortion(porFLOAT);
    //result = new ErrorPortion("Division by zero");
  }
  return result;
}


Portion* GSM_Divide_int(Portion** param)
{
  Portion* result = 0;
  if(((IntPortion*) param[1])->Value() != 0)
  {
    result = new RationalValPortion(((IntPortion*) param[0])->Value());
    ((RationalPortion*) result)->Value() /= 
      ((IntPortion*) param[1])->Value();
  }
  else
  {
    result = new NullPortion(porRATIONAL);
    //result = new ErrorPortion("Division by zero");
  }
  return result;
}



Portion* GSM_Divide_gRational(Portion** param)
{
  Portion* result = 0;
  if(((RationalPortion*) param[1])->Value() != (gRational)0)
  {
    result = new RationalValPortion
      (
       ((RationalPortion*) param[0])->Value() /
       ((RationalPortion*) param[1])->Value()
      );
  }
  else
  {
    result = new NullPortion(porRATIONAL);
    //result = new ErrorPortion("Division by zero");
  }
  return result;
}



//-------------------------- GSM_IntegerDivide -------------------------------

Portion* GSM_IntegerDivide_int(Portion** param)
{
  Portion* result = 0;
  if(((IntPortion*) param[1])->Value() != 0)
  {
    result = new IntValPortion
      (
       ((IntPortion*) param[0])->Value() /
       ((IntPortion*) param[1])->Value()
      );
  }
  else
  {
    result = new NullPortion(porINTEGER);
    //result = new ErrorPortion("Division by zero");
  }
  return result;
}




//---------------------- GSM_Exp, GSM_Log --------------------

Portion *GSM_Exp(Portion **param)
{
  return new FloatValPortion
    (exp(((FloatPortion *) param[0])->Value()));
}

Portion *GSM_Log(Portion **param)
{
  double d = ((FloatPortion *) param[0])->Value();
  if(d <= 0.0)
    return new NullPortion(porFLOAT);
    //return new ErrorPortion("Argument must be greater than zero");
  else
    return new FloatValPortion(log(d));
}


//------------------------ GSM_Power ------------------------

Portion* GSM_Power_Int_Int(Portion** param)
{
  long base = ((IntPortion*) param[0])->Value();
  long exponent = ((IntPortion*) param[1])->Value();
  return new IntValPortion((long) pow((double) base, (double) exponent));
}

Portion* GSM_Power_Float_Int(Portion** param)
{
  double base = ((FloatPortion*) param[0])->Value();
  long exponent = ((IntPortion*) param[1])->Value();
  return new FloatValPortion(pow(base, (double) exponent));
}

Portion* GSM_Power_Rational_Int(Portion** param)
{
  gRational base = ((RationalPortion*) param[0])->Value();
  long exponent = ((IntPortion*) param[1])->Value();
  return new RationalValPortion(pow(base, exponent));
}


Portion* GSM_Power_Int_Float(Portion** param)
{
  long base = ((IntPortion*) param[0])->Value();
  double exponent = ((FloatPortion*) param[1])->Value();
  return new FloatValPortion((double) pow((double) base, exponent));
}

Portion* GSM_Power_Float_Float(Portion** param)
{
  double base = ((FloatPortion*) param[0])->Value();
  double exponent = ((FloatPortion*) param[1])->Value();
  return new FloatValPortion((double) pow(base, exponent));
}

Portion* GSM_Power_Rational_Float(Portion** param)
{
  gRational base = ((RationalPortion*) param[0])->Value();
  double exponent = ((FloatPortion*) param[1])->Value();
  return new FloatValPortion((double) pow(base, ((long) exponent)));
}




//------------------------ GSM_Negate ------------------------

Portion* GSM_Negate_double(Portion** param)
{
  Portion* result = 0;
  result = new FloatValPortion
    (
     -((FloatPortion*) param[0])->Value()
    );
  return result;
}

Portion* GSM_Negate_int(Portion** param)
{
  Portion* result = 0;
  result = new IntValPortion
    (
     -((IntPortion*) param[0])->Value()
    );
  return result;
}

Portion* GSM_Negate_gRational(Portion** param)
{
  Portion* result = 0;
  result = new RationalValPortion
    (
     -((RationalPortion*) param[0])->Value()
    );
  return result;
}



//--------------------------------------------------------------------
//                    integer math operators
//--------------------------------------------------------------------

//---------------------- GSM_Modulus ---------------------------

Portion* GSM_Modulus_int(Portion** param)
{
  Portion* result = 0;
  if(((IntPortion*) param[1])->Value() != 0)
  {
    result = new IntValPortion
      (
       ((IntPortion*) param[0])->Value() %
       ((IntPortion*) param[1])->Value()
      );
  }
  else
  {
    result = new NullPortion(porINTEGER);
    //result = new ErrorPortion("Division by zero");
  }
  return result;
}



//-------------------------------------------------------------------
//                      relational operators
//------------------------------------------------------------------

//-----------
// Equal
//-----------

Portion* GSM_Equal_Float(Portion** param)
{
  if( (param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type == 
 			       param[1]->Spec().Type );
  }

  return new BoolValPortion(((FloatPortion *) param[0])->Value() ==
			    ((FloatPortion *) param[1])->Value());
}

Portion* GSM_Equal_Integer(Portion** param)
{
  if( (param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type == 
 			       param[1]->Spec().Type );
  }

  return new BoolValPortion(((IntPortion *) param[0])->Value() ==
			    ((IntPortion *) param[1])->Value());
}

Portion* GSM_Equal_Rational(Portion** param)
{
  if( (param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type == 
 			       param[1]->Spec().Type );
  }
  return new BoolValPortion(((RationalPortion *) param[0])->Value() ==
			    ((RationalPortion *) param[1])->Value());
}

Portion* GSM_Equal_Text(Portion** param)
{
  if( (param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type == 
 			       param[1]->Spec().Type );
  }
  return new BoolValPortion(((TextPortion *) param[0])->Value() ==
			    ((TextPortion *) param[1])->Value());
}

Portion* GSM_Equal_Boolean(Portion** param)
{
  if( (param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type == 
 			       param[1]->Spec().Type );
  }
  return new BoolValPortion(((BoolPortion *) param[0])->Value() ==
			    ((BoolPortion *) param[1])->Value());
}

Portion* GSM_Equal_Efg(Portion** param)
{
  if( (param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type == 
 			       param[1]->Spec().Type );
  }
  return new BoolValPortion(((EfgPortion *) param[0])->Value() ==
			    ((EfgPortion *) param[1])->Value());
}

Portion* GSM_Equal_EfPlayer(Portion** param)
{
  if( (param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type == 
 			       param[1]->Spec().Type );
  }
  return new BoolValPortion(((EfPlayerPortion *) param[0])->Value() ==
			    ((EfPlayerPortion *) param[1])->Value());
}

Portion* GSM_Equal_Node(Portion** param)
{
  if( (param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type == 
 			       param[1]->Spec().Type );
  }
  return new BoolValPortion(((NodePortion *) param[0])->Value() ==
			    ((NodePortion *) param[1])->Value());
}

Portion* GSM_Equal_Infoset(Portion** param)
{
  if( (param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type == 
 			       param[1]->Spec().Type );
  }
  return new BoolValPortion(((InfosetPortion *) param[0])->Value() ==
			    ((InfosetPortion *) param[1])->Value());
}

Portion* GSM_Equal_Outcome(Portion** param)
{
  if( (param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type == 
 			       param[1]->Spec().Type );
  }
  return new BoolValPortion(((EfOutcomePortion *) param[0])->Value() ==
			    ((EfOutcomePortion *) param[1])->Value());
}

Portion* GSM_Equal_Action(Portion** param)
{
  if( (param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type == 
 			       param[1]->Spec().Type );
  }
  return new BoolValPortion(((ActionPortion *) param[0])->Value() ==
			    ((ActionPortion *) param[1])->Value());
}

Portion* GSM_Equal_EfSupport(Portion** param)
{
  if( (param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type == 
 			       param[1]->Spec().Type );
  }
  return new BoolValPortion((*((EfSupportPortion *) param[0])->Value()) ==
			    (*((EfSupportPortion *) param[1])->Value()));
}

Portion* GSM_Equal_BehavFloat(Portion** param)
{
  if( (param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type == 
 			       param[1]->Spec().Type );
  }
  return new BoolValPortion
    ((*(BehavSolution<double> *) ((BehavPortion<double> *) param[0])->Value()) ==
     (*(BehavSolution<double> *) ((BehavPortion<double> *) param[1])->Value()));
}

Portion* GSM_Equal_BehavRational(Portion** param)
{
  if( (param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type == 
 			       param[1]->Spec().Type );
  }
  return new BoolValPortion
    ((*(BehavSolution<gRational> *) ((BehavPortion<gRational> *) param[0])->Value()) ==
     (*(BehavSolution<gRational> *) ((BehavPortion<gRational> *) param[1])->Value()));
}

Portion *GSM_Equal_Nfg(Portion** param)
{
  if( (param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type == 
 			       param[1]->Spec().Type );
  }

// This occurs when games are of different subtype
  if (param[0]->Spec().Type != param[0]->Spec().Type)
    return new BoolValPortion(false);

  if (param[0]->Spec().Type == porNFG_FLOAT)
    return new BoolValPortion(((NfgPortion<double> *) param[0])->Value() ==
			      ((NfgPortion<double> *) param[1])->Value());
  else
    return new BoolValPortion(((NfgPortion<gRational> *) param[0])->Value() ==
			      ((NfgPortion<gRational> *) param[1])->Value());

}

Portion* GSM_Equal_NfPlayer(Portion** param)
{
  if( (param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type == 
 			       param[1]->Spec().Type );
  }
  return new BoolValPortion(((NfPlayerPortion *) param[0])->Value() ==
			    ((NfPlayerPortion *) param[1])->Value());
}

Portion* GSM_Equal_Strategy(Portion** param)
{
  if( (param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type == 
 			       param[1]->Spec().Type );
  }
  return new BoolValPortion(((StrategyPortion *) param[0])->Value() ==
			    ((StrategyPortion *) param[1])->Value());
}

Portion* GSM_Equal_NfSupport(Portion** param)
{
  if( (param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type == 
 			       param[1]->Spec().Type );
  }
  return new BoolValPortion((*((NfSupportPortion *) param[0])->Value()) ==
			    (*((NfSupportPortion *) param[1])->Value()));
}

Portion* GSM_Equal_MixedFloat(Portion** param)
{
  if( (param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type == 
 			       param[1]->Spec().Type );
  }
  return new BoolValPortion
    ((*(MixedSolution<double> *) ((MixedPortion<double> *) param[0])->Value()) ==
     (*(MixedSolution<double> *) ((MixedPortion<double> *) param[1])->Value()));
}

Portion* GSM_Equal_MixedRational(Portion** param) 
{
  if( (param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type == 
 			       param[1]->Spec().Type );
  }
  return new BoolValPortion
    ((*(MixedSolution<gRational> *) ((MixedPortion<gRational> *) param[0])->Value()) ==
     (*(MixedSolution<gRational> *) ((MixedPortion<gRational> *) param[1])->Value()));
}

//------------
// NotEqual
//------------

Portion* GSM_NotEqual_Float(Portion** param)
{
  return new BoolValPortion(((FloatPortion *) param[0])->Value() !=
			    ((FloatPortion *) param[1])->Value());
}

Portion* GSM_NotEqual_Integer(Portion** param)
{
  return new BoolValPortion(((IntPortion *) param[0])->Value() !=
			    ((IntPortion *) param[1])->Value());
}

Portion* GSM_NotEqual_Rational(Portion** param)
{
  return new BoolValPortion(((RationalPortion *) param[0])->Value() !=
			    ((RationalPortion *) param[1])->Value());
}

Portion* GSM_NotEqual_Text(Portion** param)
{
  return new BoolValPortion(((TextPortion *) param[0])->Value() !=
			    ((TextPortion *) param[1])->Value());
}

Portion* GSM_NotEqual_Boolean(Portion** param)
{
  return new BoolValPortion(((BoolPortion *) param[0])->Value() !=
			    ((BoolPortion *) param[1])->Value());
}

Portion* GSM_NotEqual_Efg(Portion** param)
{
  return new BoolValPortion(((EfgPortion *) param[0])->Value() !=
			    ((EfgPortion *) param[1])->Value());
}

Portion* GSM_NotEqual_EfPlayer(Portion** param)
{
  return new BoolValPortion(((EfPlayerPortion *) param[0])->Value() !=
			    ((EfPlayerPortion *) param[1])->Value());
}

Portion* GSM_NotEqual_Node(Portion** param)
{
  return new BoolValPortion(((NodePortion *) param[0])->Value() !=
			    ((NodePortion *) param[1])->Value());
}

Portion* GSM_NotEqual_Infoset(Portion** param)
{
  return new BoolValPortion(((InfosetPortion *) param[0])->Value() !=
			    ((InfosetPortion *) param[1])->Value());
}

Portion* GSM_NotEqual_Outcome(Portion** param)
{
  return new BoolValPortion(((EfOutcomePortion *) param[0])->Value() !=
			    ((EfOutcomePortion *) param[1])->Value());
}

Portion* GSM_NotEqual_Action(Portion** param)
{
  return new BoolValPortion(((ActionPortion *) param[0])->Value() !=
			    ((ActionPortion *) param[1])->Value());
}

Portion* GSM_NotEqual_EfSupport(Portion** param)
{
  return new BoolValPortion((*((EfSupportPortion *) param[0])->Value()) !=
			    (*((EfSupportPortion *) param[1])->Value()));
}

Portion* GSM_NotEqual_BehavFloat(Portion** param)
{
  return new BoolValPortion
    ((*(BehavSolution<double> *) ((BehavPortion<double> *) param[0])->Value()) !=
     (*(BehavSolution<double> *) ((BehavPortion<double> *) param[1])->Value()));
}

Portion* GSM_NotEqual_BehavRational(Portion** param)
{
  return new BoolValPortion
    ((*(BehavSolution<gRational> *) ((BehavPortion<gRational> *) param[0])->Value()) !=
     (*(BehavSolution<gRational> *) ((BehavPortion<gRational> *) param[1])->Value()));
}

Portion* GSM_NotEqual_NfPlayer(Portion** param)
{
  return new BoolValPortion(((NfPlayerPortion *) param[0])->Value() !=
			    ((NfPlayerPortion *) param[1])->Value());
}

Portion* GSM_NotEqual_Strategy(Portion** param)
{
  return new BoolValPortion(((StrategyPortion *) param[0])->Value() !=
			    ((StrategyPortion *) param[1])->Value());
}

Portion* GSM_NotEqual_Nfg(Portion** param)
{
  if ((param[0]->Spec().Type == porNULL) || 
      (param[1]->Spec().Type == porNULL) )
  {
    return new BoolValPortion( param[0]->Spec().Type != 
 			       param[1]->Spec().Type );
  }

// This occurs when games are of different subtype
  if (param[0]->Spec().Type != param[0]->Spec().Type)
    return new BoolValPortion(true);

  if (param[0]->Spec().Type == porNFG_FLOAT)
    return new BoolValPortion(((NfgPortion<double> *) param[0])->Value() !=
			      ((NfgPortion<double> *) param[1])->Value());
  else
    return new BoolValPortion(((NfgPortion<gRational> *) param[0])->Value() !=
			      ((NfgPortion<gRational> *) param[1])->Value());
}

Portion* GSM_NotEqual_NfSupport(Portion** param)
{
  return new BoolValPortion((*((NfSupportPortion *) param[0])->Value()) !=
			    (*((NfSupportPortion *) param[1])->Value()));
}

Portion* GSM_NotEqual_MixedFloat(Portion** param)
{
  return new BoolValPortion
    ((*(MixedSolution<double> *) ((MixedPortion<double> *) param[0])->Value()) !=
     (*(MixedSolution<double> *) ((MixedPortion<double> *) param[1])->Value()));
}

Portion* GSM_NotEqual_MixedRational(Portion** param) 
{
  return new BoolValPortion
    ((*(MixedSolution<gRational> *) ((MixedPortion<gRational> *) param[0])->Value()) !=
     (*(MixedSolution<gRational> *) ((MixedPortion<gRational> *) param[1])->Value()));
}





//-------------------------- GSM_GreaterThan -------------------------

Portion* GSM_GreaterThan_double(Portion** param)
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ((FloatPortion*) param[0])->Value() >
     ((FloatPortion*) param[1])->Value()
    );
  return result;
}

Portion* GSM_GreaterThan_int(Portion** param)
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ((IntPortion*) param[0])->Value() >
     ((IntPortion*) param[1])->Value()
    );
  return result;
}

Portion* GSM_GreaterThan_gRational(Portion** param)
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ((RationalPortion*) param[0])->Value() >
     ((RationalPortion*) param[1])->Value()
    );
  return result;
}

Portion* GSM_GreaterThan_gString(Portion** param)
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ((TextPortion*) param[0])->Value() >
     ((TextPortion*) param[1])->Value()
    );
  return result;
}


//----------------------- GSM_LessThan -----------------------

Portion* GSM_LessThan_double(Portion** param)
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ((FloatPortion*) param[0])->Value() <
     ((FloatPortion*) param[1])->Value()
    );
  return result;
}

Portion* GSM_LessThan_int(Portion** param)
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ((IntPortion*) param[0])->Value() <
     ((IntPortion*) param[1])->Value()
    );
  return result;
}

Portion* GSM_LessThan_gRational(Portion** param)
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ((RationalPortion*) param[0])->Value() <
     ((RationalPortion*) param[1])->Value()
    );
  return result;
}

Portion* GSM_LessThan_gString(Portion** param)
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ((TextPortion*) param[0])->Value() <
     ((TextPortion*) param[1])->Value()
    );
  return result;
}


//--------------------- GSM_GreaterThanOrEqualTo --------------------

Portion* GSM_GreaterThanOrEqualTo_double(Portion** param)
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ((FloatPortion*) param[0])->Value() >=
     ((FloatPortion*) param[1])->Value()
    );
  return result;
}

Portion* GSM_GreaterThanOrEqualTo_int(Portion** param)
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ((IntPortion*) param[0])->Value() >=
     ((IntPortion*) param[1])->Value()
    );
  return result;
}

Portion* GSM_GreaterThanOrEqualTo_gRational(Portion** param)
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ((RationalPortion*) param[0])->Value() >=
     ((RationalPortion*) param[1])->Value()
    );
  return result;
}

Portion* GSM_GreaterThanOrEqualTo_gString(Portion** param)
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ((TextPortion*) param[0])->Value() >=
     ((TextPortion*) param[1])->Value()
    );
  return result;
}


//--------------------- GSM_LessThanOrEqualTo ---------------------

Portion* GSM_LessThanOrEqualTo_double(Portion** param)
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ((FloatPortion*) param[0])->Value() <=
     ((FloatPortion*) param[1])->Value()
    );
  return result;
}

Portion* GSM_LessThanOrEqualTo_int(Portion** param)
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ((IntPortion*) param[0])->Value() <=
     ((IntPortion*) param[1])->Value()
    );
  return result;
}

Portion* GSM_LessThanOrEqualTo_gRational(Portion** param)
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ((RationalPortion*) param[0])->Value() <=
     ((RationalPortion*) param[1])->Value()
    );
  return result;
}

Portion* GSM_LessThanOrEqualTo_gString(Portion** param)
{
  Portion* result = 0;
  result = new BoolValPortion
    (
     ((TextPortion*) param[0])->Value() <=
     ((TextPortion*) param[1])->Value()
    );
  return result;
}




//----------------------------------------------------------------
//                       logical operators
//----------------------------------------------------------------



Portion* GSM_AND(Portion** param)
{
  Portion* result = 0;

  assert(param[0]->Spec().Type == porBOOL);
  
  result = new BoolValPortion
    (
     ((BoolPortion*) param[0])->Value() &&
     ((BoolPortion*) param[1])->Value()
    );
  return result;
}



Portion* GSM_OR(Portion** param)
{
  Portion* result = 0;

  assert(param[0]->Spec().Type == porBOOL);
  
  result = new BoolValPortion
    (
     ((BoolPortion*) param[0])->Value() ||
     ((BoolPortion*) param[1])->Value()
    );

  return result;
}



Portion* GSM_NOT(Portion** param)
{
  Portion* result = 0;

  assert(param[0]->Spec().Type == porBOOL);
  
  result = new BoolValPortion(!((BoolPortion*) param[0])->Value());

  return result;
}


Portion *GSM_Parentheses(Portion **param)
{
  return param[0]->ValCopy();
}

//-----------------------------------------------------------------
//    NewStream function - possibly belong somewhere else
//-----------------------------------------------------------------


Portion* GSM_NewOutputStream(Portion** param)
{
  Portion* result = 0;
  gOutput* g;
  
  gString filename = ((TextPortion*) param[0])->Value();
  bool append = ((BoolPortion*) param[1])->Value();

  g = new gFileOutput(filename, append);
  
  if(g->IsValid())
    result = new OutputValPortion(*g);    
  else
    result = new ErrorPortion((gString) "Error opening file \"" + 
			      ((TextPortion*) param[0])->Value() + "\"");
  return result;
}


Portion* GSM_NewInputStream(Portion** param)
{
  Portion* result = 0;
  gInput* g;
  
  assert(param[0]->Spec().Type == porTEXT);

  g = new gFileInput(((TextPortion*) param[0])->Value());

  if(g->IsValid())
    result = new InputValPortion(*g);
  else
    result = new ErrorPortion((gString) "Error opening file \"" + 
			      ((TextPortion*) param[0])->Value() + "\"");
  
  return result;
}




//-----------------------------------------------------------------
//    Write and SetFormat function - possibly belong somewhere else
//-----------------------------------------------------------------

long _WriteWidth = 0;
long _WritePrecis = 6;
bool _WriteExpmode = false;
bool _WriteQuoted = true;
bool _WriteListBraces = true;
bool _WriteListCommas = true;
long _WriteListLF = 0;
long _WriteListIndent = 2;
long _WriteSolutionInfo = 1;


void GSM_SetWriteOptions(void)
{
  Portion::_SetWriteWidth(_WriteWidth);
  Portion::_SetWritePrecis(_WritePrecis);
  Portion::_SetWriteExpmode(_WriteExpmode);
  Portion::_SetWriteQuoted(_WriteQuoted);
  Portion::_SetWriteListBraces(_WriteListBraces);
  Portion::_SetWriteListCommas(_WriteListCommas);
  Portion::_SetWriteListLF(_WriteListLF);
  Portion::_SetWriteListIndent(_WriteListIndent);
  Portion::_SetWriteSolutionInfo(_WriteSolutionInfo);

  ToStringWidth( _WriteWidth );
  ToStringPrecision( _WritePrecis );
}


//------------------------------------------------
// Set*Format
//------------------------------------------------

Portion* GSM_SetListFormat(Portion** param)
{
  _WriteListBraces = ((BoolPortion*) param[0])->Value();
  _WriteListCommas = ((BoolPortion*) param[1])->Value();
  _WriteListLF = ((IntPortion*) param[2])->Value();
  _WriteListIndent = ((IntPortion*) param[3])->Value();

  GSM_SetWriteOptions();

  return new BoolValPortion(true);
}

Portion* GSM_SetNumericFormat(Portion** param)
{
  _WriteWidth = ((IntPortion*) param[0])->Value();
  _WritePrecis = ((IntPortion*) param[1])->Value();
  _WriteExpmode = ((BoolPortion*) param[2])->Value();

  GSM_SetWriteOptions();

  return new BoolValPortion(true);
}


Portion* GSM_SetTextFormat(Portion** param)
{
  _WriteQuoted = ((BoolPortion*) param[0])->Value();

  GSM_SetWriteOptions();

  return new BoolValPortion(true);
}


Portion* GSM_SetSolutionFormat(Portion** param)
{
  _WriteSolutionInfo = ((IntPortion*) param[0])->Value();

  GSM_SetWriteOptions();

  return new BoolValPortion(true);
}


//------------------------------------------------
// Get*Format
//------------------------------------------------


Portion* GSM_GetListFormat(Portion** param)
{
  ((BoolPortion*) param[0])->Value() = _WriteListBraces;
  ((BoolPortion*) param[1])->Value() = _WriteListCommas;
  ((IntPortion*) param[2])->Value() = _WriteListLF;
  ((IntPortion*) param[3])->Value() = _WriteListIndent;

  return new BoolValPortion(true);
}

Portion* GSM_GetNumericFormat(Portion** param)
{
  ((IntPortion*) param[0])->Value() = _WriteWidth;
  ((IntPortion*) param[1])->Value() = _WritePrecis;
  ((BoolPortion*) param[2])->Value() = _WriteExpmode;

  return new BoolValPortion(true);
}


Portion* GSM_GetTextFormat(Portion** param)
{
  ((BoolPortion*) param[0])->Value() = _WriteQuoted;

  return new BoolValPortion(true);
}


Portion* GSM_GetSolutionFormat(Portion** param)
{
  ((IntPortion*) param[0])->Value() = _WriteSolutionInfo;

  return new BoolValPortion(true);
}






Portion* GSM_SetVerbose(Portion** param)
{
  gcmdline.SetVerbose( ((BoolPortion*) param[0])->Value() );
  return new BoolValPortion( gcmdline.Verbose() );
  // _gsm->SetVerbose( ((BoolPortion*) param[0])->Value() );
  // return new BoolValPortion( _gsm->Verbose() );
}





Portion *GSM_Print(Portion **param)
{
  gout << param[0] << '\n';
  return param[0]->ValCopy();
}


Portion* GSM_Write_numerical(Portion** param)
{
  assert(param[1]->Spec().Type & (porINTEGER|porFLOAT|porRATIONAL));
  gOutput& s = ((OutputPortion*) param[0])->Value();
  s << param[1];

  // swap the first parameter with the return value, so things like
  //   Output["..."] << x << y  would work
  Portion* p = param[0];
  param[0] = p->RefCopy();
  return p;
}


Portion* GSM_Write_gString(Portion** param)
{
  assert(param[1]->Spec().Type == porTEXT);
  gOutput& s = ((OutputPortion*) param[0])->Value();
  s << param[1];

  // swap the first parameter with the return value, so things like
  //   Output["..."] << x << y  would work
  Portion* p = param[0];
  param[0] = p->RefCopy();
  return p;
}


Portion* GSM_Write_Mixed(Portion** param)
{
  assert(param[1]->Spec().Type & porMIXED);
  gOutput& s = ((OutputPortion*) param[0])->Value();
  s << param[1];

  // swap the first parameter with the return value, so things like
  //   Output["..."] << x << y  would work
  Portion* p = param[0];
  param[0] = p->RefCopy();
  return p;
}


Portion* GSM_Write_Behav(Portion** param)
{
  assert(param[1]->Spec().Type & porBEHAV);
  gOutput& s = ((OutputPortion*) param[0])->Value();
  s << param[1];

  // swap the first parameter with the return value, so things like
  //   Output["..."] << x << y  would work
  Portion* p = param[0];
  param[0] = p->RefCopy();
  return p;
}


Portion* GSM_Write_NfSupport(Portion** param)
{
  assert(param[1]->Spec().Type & porNFSUPPORT);
  gOutput& s = ((OutputPortion*) param[0])->Value();
  s << param[1];

  // swap the first parameter with the return value, so things like
  //   Output["..."] << x << y  would work
  Portion* p = param[0];
  param[0] = p->RefCopy();
  return p;
}

Portion* GSM_Write_EfSupport(Portion** param)
{
  assert(param[1]->Spec().Type & porEFSUPPORT);
  gOutput& s = ((OutputPortion*) param[0])->Value();
  s << param[1];

  // swap the first parameter with the return value, so things like
  //   Output["..."] << x << y  would work
  Portion* p = param[0];
  param[0] = p->RefCopy();
  return p;
}

Portion* GSM_Write_Strategy(Portion** param)
{
  assert(param[1]->Spec().Type & porSTRATEGY);
  gOutput& s = ((OutputPortion*) param[0])->Value();
  s << param[1];

  // swap the first parameter with the return value, so things like
  //   Output["..."] << x << y  would work
  Portion* p = param[0];
  param[0] = p->RefCopy();
  return p;
}


Portion* GSM_Write_Nfg(Portion** param)
{
  assert(param[1]->Spec().Type & porNFG);
  gOutput& s = ((OutputPortion*) param[0])->Value();
  
  if (param[1]->Spec().Type == porNFG_FLOAT)   {
    Nfg<double> *nfg = ((NfgPortion<double> *) param[1])->Value();
    nfg->WriteNfgFile(s);
  }
  else  {
    Nfg<gRational> *nfg = ((NfgPortion<gRational> *) param[1])->Value();
    nfg->WriteNfgFile(s);
  }
    

  // swap the first parameter with the return value, so things like
  //   Output["..."] << x << y  would work
  Portion* p = param[0];
  param[0] = p->RefCopy();
  return p;
}


Portion* GSM_Write_EfgFloat(Portion** param)
{
  assert(param[1]->Spec().Type & porEFG);
  gOutput& s = ((OutputPortion*) param[0])->Value();

  Efg<double> *efg = (Efg<double> *) ((EfgPortion*) param[1])->Value();

  efg->WriteEfgFile(s);


  // swap the first parameter with the return value, so things like
  //   Output["..."] << x << y  would work
  Portion* p = param[0];
  param[0] = p->RefCopy();
  return p;
}

Portion* GSM_Write_EfgRational(Portion** param)
{
  assert(param[1]->Spec().Type & porEFG);
  gOutput& s = ((OutputPortion*) param[0])->Value();

  Efg<gRational> *efg = (Efg<gRational> *) ((EfgPortion*) param[1])->Value();

  efg->WriteEfgFile(s);


  // swap the first parameter with the return value, so things like
  //   Output["..."] << x << y  would work
  Portion* p = param[0];
  param[0] = p->RefCopy();
  return p;
}




Portion* GSM_Write_list(Portion** param)
{
  assert(param[1]->Spec().ListDepth > 0);
  gOutput& s = ((OutputPortion*) param[0])->Value();
  s << param[1];

  // swap the first parameter with the return value, so things like
  //   Output["..."] << x << y  would work
  Portion* p = param[0];
  param[0] = p->RefCopy();
  return p;
}



//------------------------------ Read --------------------------//





Portion* GSM_Read_Bool(Portion** param)
{
  gInput& input = ((InputPortion*) param[0])->Value();
  long old_pos = input.getpos();
  bool value = false;
  bool error = false;
  char c = ' ';

  if(input.eof())
  {
    input.setpos(old_pos);
    return new ErrorPortion("End of file reached");
  }
  while(!input.eof() && isspace(c))
    input.get(c);
  if(c == 'T')
  {
    if(!input.eof()) input.get(c); if(c != 'r') error = true;
    if(!input.eof()) input.get(c); if(c != 'u') error = true;
    if(!input.eof()) input.get(c); if(c != 'e') error = true;
    value = true;
  }
  else if(c == 'F')
  {
    if(!input.eof()) input.get(c); if(c != 'a') error = true;
    if(!input.eof()) input.get(c); if(c != 'l') error = true;
    if(!input.eof()) input.get(c); if(c != 's') error = true;
    if(!input.eof()) input.get(c); if(c != 'e') error = true;
    value = false;
  }
  else
    error = true;

  if(error)
  {
    input.setpos(old_pos);
    return new ErrorPortion("No boolean data found");
  }

  ((BoolPortion*) param[1])->Value() = value;


  // swap the first parameter with the return value, so things like
  //   Input["..."] >> x >> y  would work
  Portion* p = param[0];
  param[0] = p->RefCopy();
  return p;
}


Portion* GSM_Read_Integer(Portion** param)
{
  gInput& input = ((InputPortion*) param[0])->Value();
  long old_pos = input.getpos();
  int value;
  char c;

  if(input.eof())
  {
    input.setpos(old_pos);
    return new ErrorPortion("End of file reached");
  }
  input >> value;
  if(!input.IsValid())
  {
    input.setpos(old_pos);
    return new ErrorPortion("File read error");
  }

  input.get(c);
  while(!input.eof() && isspace(c)) 
    input.get(c); 
  if(c == '/')
  {
    input.setpos(old_pos);
    return new ErrorPortion("Type mismatch: expected INTEGER, got RATIONAL");
  }
  else if(c == '.')
  {
    input.setpos(old_pos);
    return new ErrorPortion("Type mismatch: expected INTEGER, got FLOAT");
  }
  else
    input.unget(c);

  ((IntPortion*) param[1])->Value() = value;


  // swap the first parameter with the return value, so things like
  //   Input["..."] >> x >> y  would work
  Portion* p = param[0];
  param[0] = p->RefCopy();
  return p;
}

Portion* GSM_Read_Float(Portion** param)
{
  gInput& input = ((InputPortion*) param[0])->Value();
  long old_pos = input.getpos();
  double value;
  char c;

  if(input.eof())
  {
    input.setpos(old_pos);
    return new ErrorPortion("End of file reached");
  }
  input >> value;
  if(!input.IsValid()) 
  {
    input.setpos(old_pos);
    return new ErrorPortion("File read error");
  }

  input.get(c);
  while(!input.eof() && isspace(c)) 
    input.get(c); 
  if(c == '/')
  {
    input.setpos(old_pos);
    return new ErrorPortion("Type mismatch: expected FLOAT, got RATIONAL");
  }
  else
    input.unget(c);

  ((FloatPortion*) param[1])->Value() = value;


  // swap the first parameter with the return value, so things like
  //   Input["..."] >> x >> y  would work
  Portion* p = param[0];
  param[0] = p->RefCopy();
  return p;
}

Portion* GSM_Read_Rational(Portion** param)
{
  int numerator = 0;
  int denominator = 0;
  char c = ' ';
  gInput& input = ((InputPortion*) param[0])->Value();
  long old_pos = input.getpos();

  if(input.eof())
  {
    input.setpos(old_pos);
    return new ErrorPortion("End of file reached");
  }
  input >> numerator;
  if(!input.IsValid())
  {
    input.setpos(old_pos);
    return new ErrorPortion("File read error");
  }

  input.get(c);
  while(!input.eof() && isspace(c)) 
    input.get(c); 
  if(!input.eof() && c == '.')
  {
    input.setpos(old_pos);
    return new ErrorPortion("Expected RATIONAL, got FLOAT");
  }
  if(input.eof() || c != '/')
  {
    input.setpos(old_pos);
    return new ErrorPortion("Expected RATIONAL, got INTEGER");
  }

  if(input.eof())
  {
    input.setpos(old_pos);
    return new ErrorPortion("End of file reached");
  }
  input >> denominator;
  if(!input.IsValid())
  {
    input.setpos(old_pos);
    return new ErrorPortion("File read error");
  }

  if(denominator == 0)
  {
    input.setpos(old_pos);
    return new ErrorPortion("Division by zero");
  }

  ((RationalPortion*) param[1])->Value() = numerator;
  ((RationalPortion*) param[1])->Value() /= denominator;
 

  // swap the first parameter with the return value, so things like
  //   Input["..."] >> x >> y  would work
  Portion* p = param[0];
  param[0] = p->RefCopy();
  return p;
}

Portion* GSM_Read_Text(Portion** param)
{
  char c = ' ';
  gString s;
  gString t;
  gInput& input = ((InputPortion*) param[0])->Value();
  long old_pos = input.getpos();

  while(!input.eof() && isspace(c))
    input.get(c);
  if(input.eof())
  {
    input.setpos(old_pos);
    return new ErrorPortion("End of file reached");
  }
  if(!input.eof() && c == '\"')
    input.get(c); 
  else
  {
    input.unget(c);
    input.setpos(old_pos);
    return new ErrorPortion("File read error: missing starting \"");
  }

  while(!input.eof() && c != '\"')
  { t+=c; input.get(c); }
  if(input.eof())
  {
    input.setpos(old_pos);
    return new ErrorPortion("End of file reached");  
  }
  
  ((TextPortion*) param[1])->Value() = t;
 

  // swap the first parameter with the return value, so things like
  //   Input["..."] >> x >> y  would work
  Portion* p = param[0];
  param[0] = p->RefCopy();
  return p;
}



Portion* GSM_Read_List(Portion** param, PortionSpec spec,
		       Portion* (*func) (Portion**), bool ListFormat)
{
  Portion* p = NULL;
  Portion* sub_param[2];
  ListPortion* list = NULL;
  int i = 0;
  char c = ' ';  
  gInput& input = ((InputPortion*) param[0])->Value();
  list = ((ListPortion*) param[1]);
  long old_pos = input.getpos();


  while(!input.eof() && isspace(c))
    input.get(c);
  if(input.eof())
  {
    input.setpos(old_pos);
    return new ErrorPortion("End of file reached");
  }
  if(!ListFormat)
  {
    if(c == '{')
      ListFormat = true;
    else
      input.unget(c);
  }
  else
  {
    if(c != '{')
    {
      input.setpos(old_pos);
      return new ErrorPortion("\'{\' expected");
    }
  }




  for(i=1; i <= list->Length(); i++)
  {
    assert((*list)[i]->Spec().Type==spec.Type);
    sub_param[0] = param[0];
    sub_param[1] = (*list)[i];

    if(i > 1) 
    {
      c = ' ';
      while(!input.eof() && isspace(c))
	input.get(c);
      if(c == ',')
      {
	if(!ListFormat)
	  input.unget(c);
      }
      else
	input.unget(c);      
    }

    if((*list)[i]->Spec() == spec)
      p = (*func)(sub_param);
    else
      p = GSM_Read_List(sub_param, spec, func, ListFormat);

    if(p->Spec().Type == porERROR)
    {
      input.setpos(old_pos);
      return p;
    }
    else
    {
      // okay, complicated things going on here
      // we want to delete the return value, but
      //   p is actually swapped with sub_param[0] in the
      //   GSM_Read() functions!  So, can't just delete p;
      //   need to swap p and sub_param[0] first.

      // just to make sure that the above description is still
      //   correct...
      assert( p == param[0] );
      assert( sub_param[0] != param[0] );
      
      // delete and swap
      delete sub_param[0];
      sub_param[0] = p;
      p = NULL;
    }
  }

  if(ListFormat)
  {
    c = ' ';
    while(!input.eof() && isspace(c))
      input.get(c);
    if(c != '}')
    {
      input.setpos(old_pos);
      return new ErrorPortion("Mismatched braces");
    }
    if(input.eof())
    {
      input.setpos(old_pos);
      return new ErrorPortion("End of file reached");
    }
  }


  // swap the first parameter with the return value, so things like
  //   Input["..."] >> x >> y  would work
  Portion* result = param[0];
  param[0] = result->RefCopy();
  return result;
}



Portion* GSM_Read_List_Bool(Portion** param)
{
  Portion* temp = param[1]->ValCopy();
  Portion* p = GSM_Read_List(param, porBOOL, GSM_Read_Bool, false);
  if(p->Spec().Type == porERROR)
    ((ListPortion*) param[1])->AssignFrom(temp);
  delete temp;
  return p;
}

Portion* GSM_Read_List_Integer(Portion** param)
{
  Portion* temp = param[1]->ValCopy();
  Portion* p = GSM_Read_List(param, porINTEGER, GSM_Read_Integer, false);
  if(p->Spec().Type == porERROR)
    ((ListPortion*) param[1])->AssignFrom(temp);
  delete temp;
  return p;
}

Portion* GSM_Read_List_Float(Portion** param)
{
  Portion* temp = param[1]->ValCopy();
  Portion* p = GSM_Read_List(param, porFLOAT, GSM_Read_Float, false);
  if(p->Spec().Type == porERROR)
    ((ListPortion*) param[1])->AssignFrom(temp);
  delete temp;
  return p;
}

Portion* GSM_Read_List_Rational(Portion** param)
{
  Portion* temp = param[1]->ValCopy();
  Portion* p = GSM_Read_List(param, porRATIONAL, GSM_Read_Rational, false);
  if(p->Spec().Type == porERROR)
    ((ListPortion*) param[1])->AssignFrom(temp);
  delete temp;
  return p;
}

Portion* GSM_Read_List_Text(Portion** param)
{
  Portion* temp = param[1]->ValCopy();
  Portion* p = GSM_Read_List(param, porTEXT, GSM_Read_Text, false);
  if(p->Spec().Type == porERROR)
    ((ListPortion*) param[1])->AssignFrom(temp);
  delete temp;
  return p;
}


//----------------------------------------------------------------
// The following functions were from solfunc.cc, but no longer
// correspond to built-in function.  However, they got used
// here as well...
//----------------------------------------------------------------

template <class T> Portion *gDPVectorToList(const gDPVector<T> &);

Portion *GSM_ListForm_BehavFloat(Portion **param)
{
  BehavSolution<double> *P = 
    (BehavSolution<double>*) ((BehavPortion<double>*) param[0])->Value();
  return gDPVectorToList(* (gDPVector<double>*) P);
}


Portion *GSM_ListForm_BehavRational(Portion **param)
{
  BehavSolution<gRational> *P = 
    (BehavSolution<gRational>*) ((BehavPortion<gRational>*) param[0])->Value();
  return gDPVectorToList(* (gDPVector<gRational>*) P);
}

Portion *GSM_ListForm_MixedFloat(Portion **param)
{
  int i;
  int j;
  Portion* p1;
  Portion* p2;
  Portion* por;

  MixedSolution<double> *P = 
    (MixedSolution<double>*) ((MixedPortion<double>*) param[0])->Value();

  por = new ListValPortion();

  for(i = 1; i <= P->Lengths().Length(); i++)
  {
    p1 = new ListValPortion();

    for(j = 1; j <= P->Lengths()[i]; j++)
    {
      p2 = new FloatValPortion((*P)(i, j));
      ((ListValPortion*) p1)->Append(p2);
    }

    ((ListValPortion*) por)->Append(p1);
  }

  return por;
}


Portion *GSM_ListForm_MixedRational(Portion **param)
{
  int i;
  int j;
  Portion* p1;
  Portion* p2;
  Portion* por;

  MixedSolution<gRational> *P = 
    (MixedSolution<gRational>*) ((MixedPortion<gRational>*) param[0])->Value();

  por = new ListValPortion();

  for(i = 1; i <= P->Lengths().Length(); i++)
  {
    p1 = new ListValPortion();

    for(j = 1; j <= P->Lengths()[i]; j++)
    {
      p2 = new RationalValPortion((*P)(i, j));
      ((ListValPortion*) p1)->Append(p2);
    }

    ((ListValPortion*) por)->Append(p1);
  }

  return por;
}

Portion *GSM_Mixed_NfgFloat(Portion **param)
{
  int i;
  int j;
  Portion* p1;
  Portion* p2;

  Nfg<double> &N = * (Nfg<double>*) ((NfgPortion<double>*) param[0])->Value();
  MixedSolution<double> *P = new MixedSolution<double>(N);

  if(((ListPortion*) param[1])->Length() != N.NumPlayers())
  {
    delete P;
    return new ErrorPortion("Mismatching number of players");
  }
  
  for(i = 1; i <= N.NumPlayers(); i++)
  {
    p1 = ((ListPortion*) param[1])->SubscriptCopy(i);
    if(p1->Spec().ListDepth == 0)
    {
      delete p1;
      delete P;
      return new ErrorPortion("Mismatching dimensionality");
    }
    if(((ListPortion*) p1)->Length() != N.NumStrats(i))
    {
      delete p1;
      delete P;
      return new ErrorPortion("Mismatching number of strategies");
    }

    for(j = 1; j <= N.NumStrats(i); j++)
    {
      p2 = ((ListPortion*) p1)->SubscriptCopy(j);
      if(p2->Spec().Type != porFLOAT)
      {
	delete p2;
	delete p1;
	delete P;
	return new ErrorPortion("Mismatching dimensionality");
      }
      
      (*P)(i, j) = ((FloatPortion*) p2)->Value();
      
      delete p2;
    }
    delete p1;
  }


  Portion* por = new MixedValPortion<double>(P);
  return por;
}



Portion *GSM_Mixed_NfgRational(Portion **param)
{
  int i;
  int j;
  Portion* p1;
  Portion* p2;

  Nfg<gRational> &N = * (Nfg<gRational>*) ((NfgPortion<gRational>*) param[0])->Value();
  MixedSolution<gRational> *P = new MixedSolution<gRational>(N);

  if(((ListPortion*) param[1])->Length() != N.NumPlayers())
  {
    delete P;
    return new ErrorPortion("Mismatching number of players");
  }
  
  for(i = 1; i <= N.NumPlayers(); i++)
  {
    p1 = ((ListPortion*) param[1])->SubscriptCopy(i);
    if(p1->Spec().ListDepth == 0)
    {
      delete p1;
      delete P;
      return new ErrorPortion("Mismatching dimensionality");
    }
    if(((ListPortion*) p1)->Length() != N.NumStrats(i))
    {
      delete p1;
      delete P;
      return new ErrorPortion("Mismatching number of strategies");
    }

    for(j = 1; j <= N.NumStrats(i); j++)
    {
      p2 = ((ListPortion*) p1)->SubscriptCopy(j);
      if(p2->Spec().Type != porRATIONAL)
      {
	delete p2;
	delete p1;
	delete P;
	return new ErrorPortion("Mismatching dimensionality");
      }
      
      (*P)(i, j) = ((RationalPortion*) p2)->Value();
      
      delete p2;
    }
    delete p1;
  }


  Portion* por = new MixedValPortion<gRational>(P);
  return por;
}

Portion *GSM_Behav_EfgFloat(Portion **param)
{
  int i;
  int j;
  int k;
  Portion* p1;
  Portion* p2;
  Portion* p3;

  Efg<double> &E = * (Efg<double>*) ((EfgPortion*) param[0])->Value();
  BehavSolution<double> *P = new BehavSolution<double>(E);

  if(((ListPortion*) param[1])->Length() != E.NumPlayers())
  {
    delete P;
    return new ErrorPortion("Mismatching number of players");
  }
  
  for(i = 1; i <= E.NumPlayers(); i++)
  {
    p1 = ((ListPortion*) param[1])->SubscriptCopy(i);
    if(p1->Spec().ListDepth == 0)
    {
      delete p1;
      delete P;
      return new ErrorPortion("Mismatching dimensionality");
    }
    if(((ListPortion*) p1)->Length() != E.Players()[i]->NumInfosets())
    {
      delete p1;
      delete P;
      return new ErrorPortion("Mismatching number of infosets");
    }

    for(j = 1; j <= E.Players()[i]->NumInfosets(); j++)
    {
      p2 = ((ListPortion*) p1)->SubscriptCopy(j);
      if(p2->Spec().ListDepth == 0)
      {
	delete p2;
	delete p1;
	delete P;
	return new ErrorPortion("Mismatching dimensionality");
      }
      if(((ListPortion*) p2)->Length() !=
	 E.Players()[i]->Infosets()[j]->NumActions())
      {
	delete p2;
	delete p1;
	delete P;
	return new ErrorPortion("Mismatching number of actions");
      }

      for(k = 1; k <= E.Players()[i]->Infosets()[j]->NumActions(); k++)
      {
	p3 = ((ListPortion*) p2)->SubscriptCopy(k);
	if(p3->Spec().Type != porFLOAT)
	{
	  delete p3;
	  delete p2;
	  delete p1;
	  delete P;
	  return new ErrorPortion("Mismatching dimensionality");
	}
      
	(*P)(i, j, k) = ((FloatPortion*) p3)->Value();

	delete p3;
      }
      delete p2;
    }
    delete p1;
  }

  Portion* por = new BehavValPortion<double>(P);
  return por;

}

Portion *GSM_Behav_EfgRational(Portion **param)
{
  int i;
  int j;
  int k;
  Portion* p1;
  Portion* p2;
  Portion* p3;

  Efg<gRational> &E = * (Efg<gRational>*) ((EfgPortion*) param[0])->Value();
  BehavSolution<gRational> *P = new BehavSolution<gRational>(E);

  if(((ListPortion*) param[1])->Length() != E.NumPlayers())
  {
    delete P;
    return new ErrorPortion("Mismatching number of players");
  }
  
  for(i = 1; i <= E.NumPlayers(); i++)
  {
    p1 = ((ListPortion*) param[1])->SubscriptCopy(i);
    if(p1->Spec().ListDepth == 0)
    {
      delete p1;
      delete P;
      return new ErrorPortion("Mismatching dimensionality");
    }
    if(((ListPortion*) p1)->Length() != E.Players()[i]->NumInfosets())
    {
      delete p1;
      delete P;
      return new ErrorPortion("Mismatching number of infosets");
    }

    for(j = 1; j <= E.Players()[i]->NumInfosets(); j++)
    {
      p2 = ((ListPortion*) p1)->SubscriptCopy(j);
      if(p2->Spec().ListDepth == 0)
      {
	delete p2;
	delete p1;
	delete P;
	return new ErrorPortion("Mismatching dimensionality");
      }
      if(((ListPortion*) p2)->Length() !=
	 E.Players()[i]->Infosets()[j]->NumActions())
      {
	delete p2;
	delete p1;
	delete P;
	return new ErrorPortion("Mismatching number of actions");
      }

      for(k = 1; k <= E.Players()[i]->Infosets()[j]->NumActions(); k++)
      {
	p3 = ((ListPortion*) p2)->SubscriptCopy(k);
	if(p3->Spec().Type != porRATIONAL)
	{
	  delete p3;
	  delete p2;
	  delete p1;
	  delete P;
	  return new ErrorPortion("Mismatching dimensionality");
	}
      
	(*P)(i, j, k) = ((RationalPortion*) p3)->Value();

	delete p3;
      }
      delete p2;
    }
    delete p1;
  }

  Portion* por = new BehavValPortion<gRational>(P);
  return por;
}


//----------------------------------------------------------
// End of transplanted functions
//----------------------------------------------------------


Portion* GSM_Read_MixedFloat(Portion** param)
{
  Portion* sub_param[2];
  Portion* owner = 
    new NfgValPortion<double>(& ((MixedSolution<double>*) 
		       ((MixedPortion<double>*) param[1])->Value())->BelongsTo());

  sub_param[0] = param[1];
  sub_param[1] = 0;
  Portion* list = GSM_ListForm_MixedFloat(sub_param);

  sub_param[0] = param[0];
  sub_param[1] = list;
  Portion* result = GSM_Read_List_Float(sub_param);
  // result and sub_param[0] have been swapped by GSM_Read() functions!
  //   Make sure that this is still the case, and propagate the swap
  //   to param[0] as well
  assert( result == param[0] );
  assert( sub_param[0] != param[0] );
  param[0] = sub_param[0];

  sub_param[0] = owner;
  sub_param[1] = list;
  Portion* p = GSM_Mixed_NfgFloat(sub_param);
  (*((MixedSolution<double>*) ((MixedPortion<double>*) param[1])->Value())) = 
    (*((MixedSolution<double>*) ((MixedPortion<double>*) p)->Value()));

  delete owner;
  delete list;
  delete p;

  return result;
}



Portion* GSM_Read_MixedRational(Portion** param)
{
  Portion* sub_param[2];
  Portion* owner = 
    new NfgValPortion<gRational>(& ((MixedSolution<gRational>*) 
		       ((MixedPortion<gRational>*) param[1])->Value())->BelongsTo());

  sub_param[0] = param[1];
  sub_param[1] = 0;
  Portion* list = GSM_ListForm_MixedRational(sub_param);

  sub_param[0] = param[0];
  sub_param[1] = list;
  Portion* result = GSM_Read_List_Float(sub_param);
  // result and sub_param[0] have been swapped by GSM_Read() functions!
  //   Make sure that this is still the case, and propagate the swap
  //   to param[0] as well
  assert( result == param[0] );
  assert( sub_param[0] != param[0] );
  param[0] = sub_param[0];

  sub_param[0] = owner;
  sub_param[1] = list;
  Portion* p = GSM_Mixed_NfgRational(sub_param);
  (*((MixedSolution<gRational>*) ((MixedPortion<gRational>*) param[1])->Value())) = 
    (*((MixedSolution<gRational>*) ((MixedPortion<gRational>*) p)->Value()));

  delete owner;
  delete list;
  delete p;

  return result;
}



extern Portion* GSM_ListForm_BehavFloat(Portion** param);
extern Portion* GSM_Behav_EfgFloat(Portion **param);

Portion* GSM_Read_BehavFloat(Portion** param)
{
  Portion* sub_param[2];
  Portion* owner = 
    new EfgValPortion(&((BehavSolution<double>*) 
		       ((BehavPortion<double>*) param[1])->Value())->BelongsTo());

  sub_param[0] = param[1];
  sub_param[1] = 0;
  Portion* list = GSM_ListForm_BehavFloat(sub_param);

  sub_param[0] = param[0];
  sub_param[1] = list;
  Portion* result = GSM_Read_List_Float(sub_param);
  // result and sub_param[0] have been swapped by GSM_Read() functions!
  //   Make sure that this is still the case, and propagate the swap
  //   to param[0] as well
  assert( result == param[0] );
  assert( sub_param[0] != param[0] );
  param[0] = sub_param[0];

  sub_param[0] = owner;
  sub_param[1] = list;
  Portion* p = GSM_Behav_EfgFloat(sub_param);
  (*((BehavSolution<double>*) ((BehavPortion<double>*) param[1])->Value())) = 
    (*((BehavSolution<double>*) ((BehavPortion<double>*) p)->Value()));

  delete owner;
  delete list;
  delete p;

  return result;
}


extern Portion* GSM_ListForm_BehavRational(Portion** param);
extern Portion* GSM_Behav_EfgRational(Portion **param);

Portion* GSM_Read_BehavRational(Portion** param)
{
  Portion* sub_param[2];
  Portion* owner = 
    new EfgValPortion(&((BehavSolution<gRational>*) 
		       ((BehavPortion<gRational>*) param[1])->Value())->BelongsTo());

  sub_param[0] = param[1];
  sub_param[1] = 0;
  Portion* list = GSM_ListForm_BehavRational(sub_param);

  sub_param[0] = param[0];
  sub_param[1] = list;
  Portion* result = GSM_Read_List_Rational(sub_param);
  // result and sub_param[0] have been swapped by GSM_Read() functions!
  //   Make sure that this is still the case, and propagate the swap
  //   to param[0] as well
  assert( result == param[0] );
  assert( sub_param[0] != param[0] );
  param[0] = sub_param[0];

  sub_param[0] = owner;
  sub_param[1] = list;
  Portion* p = GSM_Behav_EfgRational(sub_param);
  (*((BehavSolution<gRational>*) ((BehavPortion<gRational>*) param[1])->Value())) = 
    (*((BehavSolution<gRational>*) ((BehavPortion<gRational>*) p)->Value()));

  delete owner;
  delete list;
  delete p;

  return result;
}




Portion* GSM_Read_Undefined(Portion** param)
{
  /* will go through and try to read the input as different format until
     it succeeds */

  Portion* sub_param[2];
  char c = ' ';  
  gInput& input = ((InputPortion*) param[0])->Value();
  long old_pos = input.getpos();

  Portion* result = 0;

  assert(param[1] == 0);


  while(!input.eof() && isspace(c))
    input.get(c);
  if(input.eof())
    return new ErrorPortion("End of file reached");
  if(c == '{')
  {
    param[1] = new ListValPortion();

    bool read_success = true;

    do
    {
      sub_param[0] = param[0];
      sub_param[1] = 0;
      result = GSM_Read_Undefined(sub_param);
      if(result->Spec().Type != porERROR)
      {
	((ListPortion*) param[1])->Append(sub_param[1]);

	// okay, complicated things going on here
	// we want to delete the return value, but
	//   result is actually swapped with sub_param[0] in the
	//   GSM_Read() functions!  So, can't just delete p;
	//   need to swap result and sub_param[0] first.
	
	// just to make sure that the above description is still
	//   correct...
	assert( result == param[0] );
	assert( sub_param[0] != param[0] );
	
	// delete and swap
	delete sub_param[0];
	sub_param[0] = result;
	result = NULL;
      }
      else
      {
	delete result;
	result = NULL;
	read_success = false;
      }

      c = ' ';
      while(!input.eof() && isspace(c))
	input.get(c);
      if(!input.eof() && c != ',')
	input.unget(c);

    } while(read_success && !input.eof());

    
    assert( result == NULL );
    
    c = ' ';
    while(!input.eof() && isspace(c))
      input.get(c);
    if(input.eof())
    {
      delete param[1];
      param[1] = 0;
      result = new ErrorPortion("End of file reached");
    }
    else if(c != '}')
    {
      delete param[1];
      param[1] = 0;
      result = new ErrorPortion("Mismatching braces");
    }
    else
    {
      // swap the first parameter with the return value, so things like
      //   Input["..."] >> x >> y  would work
      result = param[0];
      param[0] = result->RefCopy();
    }

  }
  else // not a list
  {
    input.unget(c);
    param[1] = new BoolValPortion(false);
    result = GSM_Read_Bool(param);

    if(result->Spec().Type == porERROR)
    {
      delete param[1];
      delete result;
      param[1] = new IntValPortion(0);
      result = GSM_Read_Integer(param);    
    }
    if(result->Spec().Type == porERROR)
    {
      delete param[1];
      delete result;
      param[1] = new FloatValPortion(0);
      result = GSM_Read_Float(param);    
    }
    if(result->Spec().Type == porERROR)
    {
      delete param[1];
      delete result;
      param[1] = new RationalValPortion(0);
      result = GSM_Read_Rational(param);    
    }
    if(result->Spec().Type == porERROR)
    {
      delete param[1];
      delete result;
      param[1] = new TextValPortion((gString) "");
      result = GSM_Read_Text(param);    
    }
    if(result->Spec().Type == porERROR)
    {
      delete param[1];
      delete result;
      param[1] = 0;
      result = new ErrorPortion("Cannot determine data type");
      input.setpos(old_pos);
    }

  }

  return result;
}

//
// Don't know where this really belongs, but this seems like a good
// a place as any, at least for now.
//

Portion *GSM_Version(Portion **)
{
  return new FloatValPortion(GCL_VERSION);
}


Portion* GSM_Help(Portion** param)
{
  return _gsm->Help(((TextPortion*) param[0])->Value(),
		    ((BoolPortion*) param[1])->Value(),
		    ((BoolPortion*) param[2])->Value() );
}

gString GetLine(gInput& f)
{
  char c = 0;
  gString result;
  while(f.IsValid())
  {
    f >> c;
    if(f.eof())
      break;
    if(c != '\n')
      result += c;
    else
      break;
  }
  return result;
}

Portion* GSM_Manual(Portion** param)
{
  gString txt = ((TextPortion*) param[0])->Value();
  gOutput& s = ((OutputPortion*) param[1])->Value();
  ListPortion* Prototypes = (ListPortion*) _gsm->Help(txt, true, true, true);
  int i;
  int body;

  for(i=1; i<=Prototypes->Length(); i++)
  {
	 assert(Prototypes->Spec().Type == porTEXT);
	 s << ((TextPortion*) (*Prototypes)[i])->Value() << '\n';
  }


//  gString name = "gcl.man";   This gives problems on BC (rdm)
  char * name = "gcl.man";    // this change necessary for BC
  gFileInput* f = NULL;





  // This section is very inelegantly adopted from gcompile.yy...
  // This section and its gcompile.yy parallel should be converted into
  // one function...

  extern char* _SourceDir;
  const char* SOURCE = _SourceDir;
  assert( SOURCE );

#ifdef __GNUG__
  const char SLASH1= '/';
  const char SLASH = '/';
#elif defined __BORLANDC__
  const char SLASH1= '\\';
  const char * SLASH = "\\";
#endif   // __GNUG__

  bool search = false;
  bool man_found = false;
  if( strchr( name, SLASH1 ) == NULL )
	 search = true;
  gString ManFileName;

  ManFileName = (gString) name;
  f = new gFileInput( ManFileName );
  if (!f->IsValid())
  {
	 delete f;
	 f = NULL;
  }
  else
  {  
    man_found = true;
  }

  if( search )
  {

    if( !man_found && (System::GetEnv( "HOME" ) != NULL) )
    {
      ManFileName = (gString) System::GetEnv( "HOME" ) + SLASH + name;
      f = new gFileInput( ManFileName );
      if (!f->IsValid())
      {
	delete f;
	f = NULL;
      }
      else
      {  
        man_found = true;
      }
    }

    if( !man_found && (System::GetEnv( "GCLLIB" ) != NULL) )
    {
      ManFileName = (gString) System::GetEnv( "GCLLIB" ) + SLASH + name;
      f = new gFileInput( ManFileName );
      if (!f->IsValid())
      {
	delete f;
	f = NULL;
      }
      else
      {
        man_found = true;
      }
    }

    if( !man_found && (SOURCE != NULL) )
    {
      ManFileName = (gString) SOURCE + SLASH + name;
      f = new gFileInput( ManFileName );
      if (!f->IsValid())
      {
        delete f;
	f = NULL;
      }
      else
      {  
        man_found = true;
      }
    }

  }

  // End bad section

  if (f == NULL)
    return new BoolValPortion(false);


  gString line;
  gString line_out;
  bool found = false;
  while(f->IsValid() && !f->eof() && !found)
  {
    line = GetLine(*f);
    if(line.length() > txt.length())
      if( line.left(txt.length() + 1).dncase() == (txt + "[").dncase() )
	found = true;
  }
  if(found)
  {
    body = 0;
    while(f->IsValid() && !f->eof())
    {
      line = GetLine(*f);      
      if(line.length()>=3 && line.left(3) == "\\bd")
	body++;
      if(body > 0)
      {
	line_out = line;
	while(true)
	{
	  char* s;
	  int idx;
	  int numchars = 0;
	  if((s=strstr(line_out.stradr(), "\\bd")) != 0)
	    numchars = 3;
	  else if((s=strstr(line_out.stradr(), "\\ed")) != 0)
	    numchars = 3;
	  else if((s=strstr(line_out.stradr(), "\\item")) != 0)
	    numchars = 5;
	  else if((s=strstr(line_out.stradr(), "\\tt")) != 0)
	    numchars = 4;
	  else if((s=strstr(line_out.stradr(), "\\em")) != 0)
	    numchars = 4;
	  else if((s=strstr(line_out.stradr(), "$")) != 0)
	  {
	    idx = s - line_out.stradr();
	    line_out[idx] = '\'';
	    numchars = 0;
	  }
	  else if((s=strstr(line_out.stradr(), "\\verb")) != 0)
	  {
	    numchars = 5;
	    idx = s - line_out.stradr();
	    for(i=0; i<numchars; i++) 
	      line_out.remove(idx);
	    if(line_out.length()>idx)
	    {
	      char c;
	      c = line_out[idx];
	      line_out[idx] = '\"';
	      while(line_out.length()>idx)
	      {
		idx++;
		if(line_out[idx]==c)
		  break;
	      }
	      line_out[idx] = '\"';
	    }
	    numchars = 0;
	  }
	  else
	    break;
	  idx = s - line_out.stradr();
	  for(i=0; i<numchars; i++) 
	    line_out.remove(idx);
	  if(line_out.length()>idx && line_out[idx] == ' ')
	    line_out.remove(idx);
	}
	for(i=0; i<body; i++)
	  s << ' ';
	s << line_out << '\n';
      }
      if(line.length()>=3 && line.left(3) == "\\ed")
      {
	body--;
	if(body <= 0)
	  break;
      }
    }
  }

  /*
  if(!found)
  {
    s << '\n';
    s << "  " << "Manual not found\n";
    s << '\n';
  }
  */

  delete f;

  return new BoolValPortion(found);
}

Portion* GSM_HelpVars(Portion** param)
{
  return _gsm->HelpVars(((TextPortion*) param[0])->Value());
}

Portion* GSM_Clear(Portion**)
{
  _gsm->Clear();
  return new BoolValPortion(true);
}



Portion* GSM_GetEnv( Portion** param )
{
  if( ((TextPortion*) param[0])->Value().length() == 0 )
    return new ErrorPortion( "Invalid environment variable name" );

  return 
    new TextValPortion( System::GetEnv( ((TextPortion*) param[0])->Value() ) );
}

Portion* GSM_SetEnv( Portion** param )
{
  if( ((TextPortion*) param[0])->Value().length() == 0 )
    return new ErrorPortion( "Invalid environment variable name" );

  int result = 0;
  result = System::SetEnv( ((TextPortion*) param[0])->Value(),
			   ((TextPortion*) param[1])->Value() );
  if( result == 0 )
    return new BoolValPortion( true );
  else
    return new ErrorPortion( "Insufficient environment space" );
}

Portion* GSM_UnSetEnv( Portion** param )
{
  if( ((TextPortion*) param[0])->Value().length() == 0 )
    return new ErrorPortion( "Invalid environment variable name" );

  int result = 0;
  result = System::UnSetEnv( ((TextPortion*) param[0])->Value() );
  if( result == 0 )
    return new BoolValPortion( true );
  else
    return new ErrorPortion( "Insufficient environment space" );
}

Portion* GSM_Shell( Portion** param )
{
  gString str = ((TextPortion*) param[0])->Value();
  bool spawn = ((BoolPortion*) param[1])->Value();

  int result = -1;
  if( !spawn )
  {
    if( str.length() > 0 )
      result = System::Shell( str );
    else
      result = System::Shell( 0 );
  }
  else
  {
    if( str.length() > 0 )
      result = System::Spawn( str );
    else
      result = System::Spawn( 0 );
  }

  return new IntValPortion( result );
}


extern char* _ExePath;
Portion* GSM_ExePath( Portion** param)
{
#ifdef __GNUG__
  const char SLASH = '/';
#elif defined __BORLANDC__
  const char SLASH = '\\';
#endif   // __GNUG__
  bool file = ((BoolPortion*) param[0])->Value();
  bool path = ((BoolPortion*) param[1])->Value();

  assert( _ExePath );
  gString txt( _ExePath );

  if( file && path )
  {
  }
  else if( file )
  {
    if( txt.lastOccur( SLASH ) > 0 )
      txt = txt.right( txt.length() - txt.lastOccur( SLASH ) );
    else
      txt = "";
  }
  else if( path )
  {
    if( txt.lastOccur( SLASH ) > 0 )
      txt = txt.left( txt.lastOccur( SLASH ) );
  }
  if( !file && !path )
    txt = "";
  return new TextValPortion( txt );
}



Portion* GSM_Platform( Portion** )
{
#ifdef __svr4__
  return new TextValPortion( "SVR4" );
#elif defined sparc
  return new TextValPortion( "SPARC" );
#elif defined sun
  return new TextValPortion( "SUN" );
#elif defined _AIX32
  return new TextValPortion( "AIX32" );
#elif defined _AIX
  return new TextValPortion( "AIX" );
#elif defined hpux
  return new TextValPortion( "HP UX" );
#elif defined hppa
  return new TextValPortion( "HPPA" );
#elif defined __BORLANDC__
  return new TextValPortion( "DOS/Windows" );
#else
  return new TextValPortion( "Unknown" );
#endif
}


extern gStack<gString> GCL_InputFileNames;
Portion* GSM_GetPath( Portion** param )
{
#ifdef __GNUG__
  const char SLASH = '/';
#elif defined __BORLANDC__
  const char SLASH = '\\';
#endif   // __GNUG__
  bool file = ((BoolPortion*) param[0])->Value();
  bool path = ((BoolPortion*) param[1])->Value();
  if( GCL_InputFileNames.Depth() > 0 )
  {
    gString txt = GCL_InputFileNames.Peek();

    if( file && path )
    {
    }
    else if( file )
    {
      if( txt.lastOccur( SLASH ) > 0 )
	txt = txt.right( txt.length() - txt.lastOccur( SLASH ) );
      else
	txt = "";
    }
    else if( path )
    {
      if( txt.lastOccur( SLASH ) > 0 )
	txt = txt.left( txt.lastOccur( SLASH ) );
    }
    if( !file && !path )
      txt = "";
    
    return new TextValPortion( txt );
  }
  else
    return new TextValPortion( "" );
}



#include <sys/types.h>
#ifndef __BORLANDC__
#include <sys/time.h>
#endif
#include <time.h>

Portion* GSM_Date(Portion**)
{
  time_t now = time(0);
  gString AscTime = asctime(localtime(&now));
  return new TextValPortion(AscTime.mid(11, 1) +
			    AscTime.mid(4, 21) + ", " + AscTime.mid(8, 12));
}





void Init_gsmoper(GSM* gsm)
{
  FuncDescObj* FuncObj;

  FuncObj = new FuncDescObj("Version", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Version, porFLOAT, 0));
  gsm->AddFunction(FuncObj);

  //---------------------- Assign ------------------------


  ParamInfoType xy_Int[] =
  {
    ParamInfoType("x", porINTEGER),
    ParamInfoType("y", porINTEGER)
  };

  ParamInfoType xy_Float[] =
  {
    ParamInfoType("x", porFLOAT),
    ParamInfoType("y", porFLOAT)
  };

  ParamInfoType xy_Rational[] =
  {
    ParamInfoType("x", porRATIONAL),
    ParamInfoType("y", porRATIONAL)
  };

  ParamInfoType xy_Text[] =
  {
    ParamInfoType("x", porTEXT),
    ParamInfoType("y", porTEXT)
  };

  ParamInfoType xy_Bool[] =
  {
    ParamInfoType("x", porBOOL),
    ParamInfoType("y", porBOOL)
  };


  ParamInfoType xy_NfSupport[] =
  {
    ParamInfoType("x", porNFSUPPORT),
    ParamInfoType("y", porNFSUPPORT)
  };

  ParamInfoType xy_MixedFloat[] =
  {
    ParamInfoType("x", porMIXED_FLOAT),
    ParamInfoType("y", porMIXED_FLOAT)
  };

  ParamInfoType xy_MixedRational[] =
  {
    ParamInfoType("x", porMIXED_RATIONAL),
    ParamInfoType("y", porMIXED_RATIONAL)
  };

  ParamInfoType xy_EfSupport[] =
  {
    ParamInfoType("x", porEFSUPPORT),
    ParamInfoType("y", porEFSUPPORT)
  };

  ParamInfoType xy_BehavFloat[] =
  {
    ParamInfoType("x", porBEHAV_FLOAT),
    ParamInfoType("y", porBEHAV_FLOAT)
  };

  ParamInfoType xy_BehavRational[] =
  {
    ParamInfoType("x", porBEHAV_RATIONAL),
    ParamInfoType("y", porBEHAV_RATIONAL)
  };

  ParamInfoType xy_Efg[] =
  {
    ParamInfoType("x", porEFG),
    ParamInfoType("y", porEFG)
  };

  ParamInfoType xy_EfPlayer[] =
  {
    ParamInfoType("x", porEFPLAYER),
    ParamInfoType("y", porEFPLAYER)
  };

  ParamInfoType xy_NfPlayer[] =
  {
    ParamInfoType("x", porNFPLAYER),
    ParamInfoType("y", porNFPLAYER),
  };

  ParamInfoType xy_Node[] =
  {
    ParamInfoType("x", porNODE),
    ParamInfoType("y", porNODE)
  };

  ParamInfoType xy_Infoset[] =
  {
    ParamInfoType("x", porINFOSET),
    ParamInfoType("y", porINFOSET)
  };

  ParamInfoType xy_Outcome[] =
  {
    ParamInfoType("x", porEFOUTCOME),
    ParamInfoType("y", porEFOUTCOME)
  };

  ParamInfoType xy_Action[] =
  {
    ParamInfoType("x", porACTION),
    ParamInfoType("y", porACTION)
  };

  ParamInfoType xy_Strategy[] =
  {
    ParamInfoType("x", porSTRATEGY),
    ParamInfoType("y", porSTRATEGY)
  };

  ParamInfoType xy_Nfg[] =
  {
    ParamInfoType("x", porNFG),
    ParamInfoType("y", porNFG)
  };



  ParamInfoType x_Float[] =
  {
    ParamInfoType("x", porFLOAT)
  };


  ParamInfoType file_Text[] =
  {
    ParamInfoType("file", porTEXT)
  };


  //-------------------- Plus ----------------------------

  FuncObj = new FuncDescObj("Plus", 7);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Add_double, 
				       porFLOAT, 2, xy_Float));
  FuncObj->SetFuncInfo(1, FuncInfoType(GSM_Add_int, 
				       porINTEGER, 2, xy_Int));
  FuncObj->SetFuncInfo(2, FuncInfoType(GSM_Add_gRational, 
				       porRATIONAL, 2, xy_Rational));
  FuncObj->SetFuncInfo(3, FuncInfoType(GSM_Add_MixedFloat, 
				       porMIXED_FLOAT, 2, xy_MixedFloat));
  FuncObj->SetFuncInfo(4, FuncInfoType(GSM_Add_MixedRational,
				       porMIXED_RATIONAL, 2,xy_MixedRational));
  FuncObj->SetFuncInfo(5, FuncInfoType(GSM_Add_BehavFloat, 
				       porBEHAV_FLOAT, 2, xy_BehavFloat));
  FuncObj->SetFuncInfo(6, FuncInfoType(GSM_Add_BehavRational, 
				       porBEHAV_RATIONAL, 2,xy_BehavRational));
  gsm->AddFunction(FuncObj);

  //-------------------- Concat -------------------------
  
  FuncObj = new FuncDescObj("Concat", 2);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Concat_gString, 
				       porTEXT, 2, xy_Text));
  FuncObj->SetFuncInfo(1, FuncInfoType(GSM_Concat_List, 
				       PortionSpec(porANYTYPE, 1), 2));
  FuncObj->SetParamInfo(1, 0, ParamInfoType("x", 
					    PortionSpec(porANYTYPE, NLIST)));
  FuncObj->SetParamInfo(1, 1, ParamInfoType("y", 
					    PortionSpec(porANYTYPE, NLIST)));
  gsm->AddFunction(FuncObj);

  //----------------------- Minus ------------------------

  FuncObj = new FuncDescObj("Minus", 7);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Subtract_double, 
				       porFLOAT, 2, xy_Float));
  FuncObj->SetFuncInfo(1, FuncInfoType(GSM_Subtract_int, 
				       porINTEGER, 2, xy_Int));
  FuncObj->SetFuncInfo(2, FuncInfoType(GSM_Subtract_gRational, 
				       porRATIONAL, 2, xy_Rational));
  FuncObj->SetFuncInfo(3, FuncInfoType(GSM_Subtract_MixedFloat, 
				       porMIXED_FLOAT, 2, xy_MixedFloat));
  FuncObj->SetFuncInfo(4, FuncInfoType(GSM_Subtract_MixedRational, 
				       porMIXED_RATIONAL, 2,xy_MixedRational));
  FuncObj->SetFuncInfo(5, FuncInfoType(GSM_Subtract_BehavFloat, 
				       porBEHAV_FLOAT, 2, xy_BehavFloat));
  FuncObj->SetFuncInfo(6, FuncInfoType(GSM_Subtract_BehavRational, 
				       porBEHAV_RATIONAL, 2,xy_BehavRational));
  gsm->AddFunction(FuncObj);


  //----------------------- Times ----------------------------

  FuncObj = new FuncDescObj("Times", 7);

  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Multiply_double, 
				       porFLOAT, 2, xy_Float));
  FuncObj->SetFuncInfo(1, FuncInfoType(GSM_Multiply_int, 
				       porINTEGER, 2, xy_Int));
  FuncObj->SetFuncInfo(2, FuncInfoType(GSM_Multiply_gRational, 
				       porRATIONAL, 2, xy_Rational));

  FuncObj->SetFuncInfo(3, FuncInfoType(GSM_Multiply_MixedFloat, 
				       porMIXED_FLOAT, 2));
  FuncObj->SetParamInfo(3, 0, ParamInfoType("x", porFLOAT));
  FuncObj->SetParamInfo(3, 1, ParamInfoType("y", porMIXED_FLOAT));

  FuncObj->SetFuncInfo(4, FuncInfoType(GSM_Multiply_MixedRational, 
				       porMIXED_RATIONAL, 2));
  FuncObj->SetParamInfo(4, 0, ParamInfoType("x", porRATIONAL));
  FuncObj->SetParamInfo(4, 1, ParamInfoType("y", porMIXED_RATIONAL));

  FuncObj->SetFuncInfo(5, FuncInfoType(GSM_Multiply_BehavFloat, 
				       porBEHAV_FLOAT, 2));
  FuncObj->SetParamInfo(5, 0, ParamInfoType("x", porFLOAT));
  FuncObj->SetParamInfo(5, 1, ParamInfoType("y", porBEHAV_FLOAT));

  FuncObj->SetFuncInfo(6, FuncInfoType(GSM_Multiply_BehavRational, 
				       porBEHAV_RATIONAL, 2));
  FuncObj->SetParamInfo(6, 0, ParamInfoType("x", porRATIONAL));
  FuncObj->SetParamInfo(6, 1, ParamInfoType("y", porBEHAV_RATIONAL));

  gsm->AddFunction(FuncObj);


  //----------------------- Divide -------------------------

  FuncObj = new FuncDescObj("Divide", 3);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Divide_double, porFLOAT, 
				       2, xy_Float));
  FuncObj->SetFuncInfo(1, FuncInfoType(GSM_Divide_int, 
				       porRATIONAL, 2, xy_Int));
  FuncObj->SetFuncInfo(2, FuncInfoType(GSM_Divide_gRational, 
				       porRATIONAL, 2, xy_Rational));
  gsm->AddFunction(FuncObj);  

  //----------------------- Power -------------------------

  FuncObj = new FuncDescObj("Power", 6);

  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Power_Int_Int, 
				       porINTEGER, 2, xy_Int));
  FuncObj->SetFuncInfo(1, FuncInfoType(GSM_Power_Float_Int, 
				       porFLOAT, 2));
  FuncObj->SetParamInfo(1, 0, ParamInfoType("x", porFLOAT));
  FuncObj->SetParamInfo(1, 1, ParamInfoType("y", porINTEGER));
  FuncObj->SetFuncInfo(2, FuncInfoType(GSM_Power_Rational_Int, 
				       porRATIONAL, 2));
  FuncObj->SetParamInfo(2, 0, ParamInfoType("x", porRATIONAL));
  FuncObj->SetParamInfo(2, 1, ParamInfoType("y", porINTEGER));

  FuncObj->SetFuncInfo(3, FuncInfoType(GSM_Power_Int_Float, 
				       porFLOAT, 2));
  FuncObj->SetParamInfo(3, 0, ParamInfoType("x", porINTEGER));
  FuncObj->SetParamInfo(3, 1, ParamInfoType("y", porFLOAT));
  FuncObj->SetFuncInfo(4, FuncInfoType(GSM_Power_Float_Float, 
				       porFLOAT, 2));
  FuncObj->SetParamInfo(4, 0, ParamInfoType("x", porFLOAT));
  FuncObj->SetParamInfo(4, 1, ParamInfoType("y", porFLOAT));
  FuncObj->SetFuncInfo(5, FuncInfoType(GSM_Power_Rational_Float, 
				       porFLOAT, 2));
  FuncObj->SetParamInfo(5, 0, ParamInfoType("x", porRATIONAL));
  FuncObj->SetParamInfo(5, 1, ParamInfoType("y", porFLOAT));

  gsm->AddFunction(FuncObj);


  //----------------------- Exp, Log ---------------------

  FuncObj = new FuncDescObj("Exp", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Exp, porFLOAT, 1, x_Float));
  gsm->AddFunction(FuncObj);

  FuncObj = new FuncDescObj("Log", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Log, porFLOAT, 1, x_Float));
  gsm->AddFunction(FuncObj);


  //----------------------- Negate -----------------------

  FuncObj = new FuncDescObj("Negate", 3);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Negate_double, 
				       porFLOAT, 1, x_Float));
  FuncObj->SetFuncInfo(1, FuncInfoType(GSM_Negate_int, porINTEGER, 1));
  FuncObj->SetParamInfo(1, 0, ParamInfoType("x", porINTEGER));
  FuncObj->SetFuncInfo(2, FuncInfoType(GSM_Negate_gRational, 
				       porRATIONAL, 1));
  FuncObj->SetParamInfo(2, 0, ParamInfoType("x", porRATIONAL));
  gsm->AddFunction(FuncObj);



  //----------------------- IntegerDivide ------------------------

  FuncObj = new FuncDescObj("IntegerDivide", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_IntegerDivide_int, 
				       porINTEGER, 2, xy_Int));
  gsm->AddFunction(FuncObj);



  //----------------------- Modulus ------------------------

  FuncObj = new FuncDescObj("Modulus", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Modulus_int, 
				       porINTEGER, 2, xy_Int));
  gsm->AddFunction(FuncObj);


  //-------------------------- Equal ---------------------------

  FuncObj = new FuncDescObj("Equal", 20);

  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Equal_Float, porBOOL, 2 ) );
  FuncObj->SetParamInfo(0, 0, ParamInfoType( "x", 
                              PortionSpec(porFLOAT, 0, porNULLSPEC)) );
  FuncObj->SetParamInfo(0, 1, ParamInfoType( "y", 
                              PortionSpec(porFLOAT, 0, porNULLSPEC)) );

  FuncObj->SetFuncInfo(1, FuncInfoType(GSM_Equal_Integer, porBOOL, 2));
  FuncObj->SetParamInfo(1, 0, ParamInfoType( "x", 
                              PortionSpec(porINTEGER, 0, porNULLSPEC)) );
  FuncObj->SetParamInfo(1, 1, ParamInfoType( "y", 
                              PortionSpec(porINTEGER, 0, porNULLSPEC)) );

  FuncObj->SetFuncInfo(2, FuncInfoType(GSM_Equal_Rational, porBOOL, 2));
  FuncObj->SetParamInfo(2, 0, ParamInfoType( "x", 
                              PortionSpec(porRATIONAL, 0, porNULLSPEC)) );
  FuncObj->SetParamInfo(2, 1, ParamInfoType( "y", 
                              PortionSpec(porRATIONAL, 0, porNULLSPEC)) );

  FuncObj->SetFuncInfo(3, FuncInfoType(GSM_Equal_Text, porBOOL, 2));
  FuncObj->SetParamInfo(3, 0, ParamInfoType( "x", 
                              PortionSpec(porTEXT, 0, porNULLSPEC)) );
  FuncObj->SetParamInfo(3, 1, ParamInfoType( "y", 
                              PortionSpec(porTEXT, 0, porNULLSPEC)) );

  FuncObj->SetFuncInfo(4, FuncInfoType(GSM_Equal_Boolean, porBOOL, 2));
  FuncObj->SetParamInfo(4, 0, ParamInfoType( "x", 
                              PortionSpec(porBOOL, 0, porNULLSPEC)) );
  FuncObj->SetParamInfo(4, 1, ParamInfoType( "y", 
                              PortionSpec(porBOOL, 0, porNULLSPEC)) );

  FuncObj->SetFuncInfo(5, FuncInfoType(GSM_Equal_Efg, porBOOL, 2));
  FuncObj->SetParamInfo(5, 0, ParamInfoType( "x", 
                              PortionSpec(porEFG, 0, porNULLSPEC)) );
  FuncObj->SetParamInfo(5, 1, ParamInfoType( "y", 
                              PortionSpec(porEFG, 0, porNULLSPEC)) );

  FuncObj->SetFuncInfo(6, FuncInfoType(GSM_Equal_EfPlayer, porBOOL, 2));
  FuncObj->SetParamInfo(6, 0, ParamInfoType( "x", 
                              PortionSpec(porEFPLAYER, 0, porNULLSPEC)) );
  FuncObj->SetParamInfo(6, 1, ParamInfoType( "y", 
                              PortionSpec(porEFPLAYER, 0, porNULLSPEC)) );

  FuncObj->SetFuncInfo(7, FuncInfoType(GSM_Equal_Node, porBOOL, 2));
  FuncObj->SetParamInfo(7, 0, ParamInfoType( "x", 
                              PortionSpec(porNODE, 0, porNULLSPEC)) );
  FuncObj->SetParamInfo(7, 1, ParamInfoType( "y", 
                              PortionSpec(porNODE, 0, porNULLSPEC)) );

  FuncObj->SetFuncInfo(8, FuncInfoType(GSM_Equal_Infoset, porBOOL, 2));
  FuncObj->SetParamInfo(8, 0, ParamInfoType( "x", 
                              PortionSpec(porINFOSET, 0, porNULLSPEC)) );
  FuncObj->SetParamInfo(8, 1, ParamInfoType( "y", 
                              PortionSpec(porINFOSET, 0, porNULLSPEC)) );

  FuncObj->SetFuncInfo(9, FuncInfoType(GSM_Equal_Outcome, porBOOL, 2));
  FuncObj->SetParamInfo(9, 0, ParamInfoType( "x", 
                              PortionSpec(porEFOUTCOME, 0, porNULLSPEC)) );
  FuncObj->SetParamInfo(9, 1, ParamInfoType( "y", 
                              PortionSpec(porEFOUTCOME, 0, porNULLSPEC)) );

  FuncObj->SetFuncInfo(10, FuncInfoType(GSM_Equal_Action, porBOOL, 2));
  FuncObj->SetParamInfo(10, 0, ParamInfoType( "x", 
                              PortionSpec(porACTION, 0, porNULLSPEC)) );
  FuncObj->SetParamInfo(10, 1, ParamInfoType( "y", 
                              PortionSpec(porACTION, 0, porNULLSPEC)) );

  FuncObj->SetFuncInfo(11, FuncInfoType(GSM_Equal_EfSupport, porBOOL, 2));
  FuncObj->SetParamInfo(11, 0, ParamInfoType( "x", 
                              PortionSpec(porEFSUPPORT, 0, porNULLSPEC)) );
  FuncObj->SetParamInfo(11, 1, ParamInfoType( "y", 
                              PortionSpec(porEFSUPPORT, 0, porNULLSPEC)) );

  FuncObj->SetFuncInfo(12, FuncInfoType(GSM_Equal_BehavFloat, porBOOL, 2));
  FuncObj->SetParamInfo(12, 0, ParamInfoType( "x", 
                              PortionSpec(porBEHAV_FLOAT, 0, porNULLSPEC)) );
  FuncObj->SetParamInfo(12, 1, ParamInfoType( "y", 
                              PortionSpec(porBEHAV_FLOAT, 0, porNULLSPEC)) );

  FuncObj->SetFuncInfo(13, FuncInfoType(GSM_Equal_BehavRational, porBOOL, 2));
  FuncObj->SetParamInfo(13, 0, ParamInfoType( "x", 
                            PortionSpec(porBEHAV_RATIONAL, 0, porNULLSPEC)) );
  FuncObj->SetParamInfo(13, 1, ParamInfoType( "y", 
                            PortionSpec(porBEHAV_RATIONAL, 0, porNULLSPEC)) );

  FuncObj->SetFuncInfo(14, FuncInfoType(GSM_Equal_Nfg, porBOOL, 2));
  FuncObj->SetParamInfo(14, 0, ParamInfoType( "x", 
                              PortionSpec(porNFG, 0, porNULLSPEC)) );
  FuncObj->SetParamInfo(14, 1, ParamInfoType( "y", 
                              PortionSpec(porNFG, 0, porNULLSPEC)) );

  FuncObj->SetFuncInfo(15, FuncInfoType(GSM_Equal_NfPlayer, porBOOL, 2));
  FuncObj->SetParamInfo(15, 0, ParamInfoType( "x", 
                              PortionSpec(porNFPLAYER, 0, porNULLSPEC)) );
  FuncObj->SetParamInfo(15, 1, ParamInfoType( "y", 
                              PortionSpec(porNFPLAYER, 0, porNULLSPEC)) );

  FuncObj->SetFuncInfo(16, FuncInfoType(GSM_Equal_Strategy, porBOOL, 2));
  FuncObj->SetParamInfo(16, 0, ParamInfoType( "x", 
                              PortionSpec(porSTRATEGY, 0, porNULLSPEC)) );
  FuncObj->SetParamInfo(16, 1, ParamInfoType( "y", 
                              PortionSpec(porSTRATEGY, 0, porNULLSPEC)) );

  FuncObj->SetFuncInfo(17, FuncInfoType(GSM_Equal_NfSupport, porBOOL, 2));
  FuncObj->SetParamInfo(17, 0, ParamInfoType( "x", 
                              PortionSpec(porNFSUPPORT, 0, porNULLSPEC)) );
  FuncObj->SetParamInfo(17, 1, ParamInfoType( "y", 
                              PortionSpec(porNFSUPPORT, 0, porNULLSPEC)) );

  FuncObj->SetFuncInfo(18, FuncInfoType(GSM_Equal_MixedFloat, porBOOL, 2));
  FuncObj->SetParamInfo(18, 0, ParamInfoType( "x", 
                              PortionSpec(porMIXED_FLOAT, 0, porNULLSPEC)) );
  FuncObj->SetParamInfo(18, 1, ParamInfoType( "y", 
                              PortionSpec(porMIXED_FLOAT, 0, porNULLSPEC)) );

  FuncObj->SetFuncInfo(19, FuncInfoType(GSM_Equal_MixedRational, porBOOL, 2));
  FuncObj->SetParamInfo(19, 0, ParamInfoType( "x", 
                            PortionSpec(porMIXED_RATIONAL, 0, porNULLSPEC)) );
  FuncObj->SetParamInfo(19, 1, ParamInfoType( "y", 
                            PortionSpec(porMIXED_RATIONAL, 0, porNULLSPEC)) );
  gsm->AddFunction(FuncObj);


  //-------------------------- NotEqual ---------------------------

  FuncObj = new FuncDescObj("NotEqual", 20);

  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_NotEqual_Float, 
				       porBOOL, 2, xy_Float));
  FuncObj->SetFuncInfo(1, FuncInfoType(GSM_NotEqual_Integer, 
				       porBOOL, 2, xy_Int));
  FuncObj->SetFuncInfo(2, FuncInfoType(GSM_NotEqual_Rational, 
				       porBOOL, 2, xy_Rational));
  FuncObj->SetFuncInfo(3, FuncInfoType(GSM_NotEqual_Text, 
				       porBOOL, 2, xy_Text));
  FuncObj->SetFuncInfo(4, FuncInfoType(GSM_NotEqual_Boolean, 
				       porBOOL, 2, xy_Bool));

  FuncObj->SetFuncInfo(5, FuncInfoType(GSM_NotEqual_Efg,
				       porBOOL, 2, xy_Efg));
  FuncObj->SetFuncInfo(6, FuncInfoType(GSM_NotEqual_EfPlayer, 
				       porBOOL, 2, xy_EfPlayer));
  FuncObj->SetFuncInfo(7, FuncInfoType(GSM_NotEqual_Node, 
				       porBOOL, 2, xy_Node));
  FuncObj->SetFuncInfo(8, FuncInfoType(GSM_NotEqual_Infoset, 
				       porBOOL, 2, xy_Infoset));
  FuncObj->SetFuncInfo(9, FuncInfoType(GSM_NotEqual_Outcome, 
				       porBOOL, 2, xy_Outcome));
  FuncObj->SetFuncInfo(10, FuncInfoType(GSM_NotEqual_Action, 
				       porBOOL, 2, xy_Action));
  FuncObj->SetFuncInfo(11, FuncInfoType(GSM_NotEqual_EfSupport,
					porBOOL, 2, xy_EfSupport));
  FuncObj->SetFuncInfo(12, FuncInfoType(GSM_NotEqual_BehavFloat, 
					porBOOL, 2, xy_BehavFloat));
  FuncObj->SetFuncInfo(13, FuncInfoType(GSM_NotEqual_BehavRational, 
					porBOOL, 2, xy_BehavRational));

  FuncObj->SetFuncInfo(14, FuncInfoType(GSM_NotEqual_Nfg,
					porBOOL, 2, xy_Nfg));
  FuncObj->SetFuncInfo(15, FuncInfoType(GSM_NotEqual_NfPlayer,
					porBOOL, 2, xy_NfPlayer));
  FuncObj->SetFuncInfo(16, FuncInfoType(GSM_NotEqual_Strategy,
					porBOOL, 2, xy_Strategy));
  FuncObj->SetFuncInfo(17, FuncInfoType(GSM_NotEqual_NfSupport,
					porBOOL, 2, xy_NfSupport));
  FuncObj->SetFuncInfo(18, FuncInfoType(GSM_NotEqual_MixedFloat, 
					porBOOL, 2, xy_MixedFloat));
  FuncObj->SetFuncInfo(19, FuncInfoType(GSM_NotEqual_MixedRational, 
					porBOOL, 2, xy_MixedRational));
  gsm->AddFunction(FuncObj);


  //-------------------------- Greater ---------------------------

  FuncObj = new FuncDescObj("Greater", 4);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_GreaterThan_double, 
				       porBOOL, 2, xy_Float));
  FuncObj->SetFuncInfo(1, FuncInfoType(GSM_GreaterThan_int, 
				       porBOOL, 2, xy_Int));
  FuncObj->SetFuncInfo(2, FuncInfoType(GSM_GreaterThan_gRational, 
				       porBOOL, 2, xy_Rational));
  FuncObj->SetFuncInfo(3, FuncInfoType(GSM_GreaterThan_gString, 
				       porBOOL, 2, xy_Text));
  gsm->AddFunction(FuncObj);


  //-------------------------- Less ---------------------------

  FuncObj = new FuncDescObj("Less", 4);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_LessThan_double, 
				       porBOOL, 2, xy_Float));
  FuncObj->SetFuncInfo(1, FuncInfoType(GSM_LessThan_int, 
				       porBOOL, 2, xy_Int));
  FuncObj->SetFuncInfo(2, FuncInfoType(GSM_LessThan_gRational, 
				       porBOOL, 2, xy_Rational));
  FuncObj->SetFuncInfo(3, FuncInfoType(GSM_LessThan_gString, 
				       porBOOL, 2, xy_Text));
  gsm->AddFunction(FuncObj);


  //---------------------- GreaterThanOrEqualTo ---------------------------

  FuncObj = new FuncDescObj("GreaterEqual", 4);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_GreaterThanOrEqualTo_double, 
				       porBOOL, 2, xy_Float));
  FuncObj->SetFuncInfo(1, FuncInfoType(GSM_GreaterThanOrEqualTo_int,
				       porBOOL, 2, xy_Int));
  FuncObj->SetFuncInfo(2, FuncInfoType(GSM_GreaterThanOrEqualTo_gRational, 
				       porBOOL, 2, xy_Rational));
  FuncObj->SetFuncInfo(3, FuncInfoType(GSM_GreaterThanOrEqualTo_gString, 
				       porBOOL, 2, xy_Text));
  gsm->AddFunction(FuncObj);


  //----------------------- LessThanOrEqualTo ---------------------------

  FuncObj = new FuncDescObj("LessEqual", 4);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_LessThanOrEqualTo_double, 
				       porBOOL, 2, xy_Float));
  FuncObj->SetFuncInfo(1, FuncInfoType(GSM_LessThanOrEqualTo_int, 
				       porBOOL, 2, xy_Int));
  FuncObj->SetFuncInfo(2, FuncInfoType(GSM_LessThanOrEqualTo_gRational, 
				       porBOOL, 2, xy_Rational));
  FuncObj->SetFuncInfo(3, FuncInfoType(GSM_LessThanOrEqualTo_gString, 
				       porBOOL, 2, xy_Text));
  gsm->AddFunction(FuncObj);




  //--------------------- And -------------------------------

  FuncObj = new FuncDescObj("And", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_AND, porBOOL, 2, xy_Bool));
  gsm->AddFunction(FuncObj);


  //--------------------- Or -------------------------------

  FuncObj = new FuncDescObj("Or", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_OR, porBOOL, 2, xy_Bool));
  gsm->AddFunction(FuncObj);


  //--------------------- Not -------------------------------

  FuncObj = new FuncDescObj("Not", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_NOT, porBOOL, 1));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("x", porBOOL));
  gsm->AddFunction(FuncObj);



  FuncObj = new FuncDescObj("Parentheses", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Parentheses, porANYTYPE, 1));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("x", porANYTYPE));
  gsm->AddFunction(FuncObj);

  //-------------------- NewStream -------------------------

  FuncObj = new FuncDescObj("Output", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_NewOutputStream, 
				       porOUTPUT, 2));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("file", porTEXT));
  FuncObj->SetParamInfo(0, 1, ParamInfoType("append", porBOOL,
					    new BoolValPortion( false )));
  gsm->AddFunction(FuncObj);
  
  FuncObj = new FuncDescObj("Input", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_NewInputStream, 
				       porINPUT, 1, file_Text));
  gsm->AddFunction(FuncObj);

  //------------------- Formatted Output -------------------

  GSM_SetWriteOptions();

  FuncObj = new FuncDescObj("Print", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Print, porANYTYPE, 1));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("x", porANYTYPE));
  gsm->AddFunction(FuncObj);


  FuncObj = new FuncDescObj("Write", 11);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Write_numerical, 
				       porOUTPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("output", porOUTPUT,
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(0, 1, ParamInfoType
			("x", porBOOL | porINTEGER | porFLOAT | porRATIONAL));

  FuncObj->SetFuncInfo(1, FuncInfoType(GSM_Write_gString, 
				       porOUTPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(1, 0, ParamInfoType("output", porOUTPUT,
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(1, 1, ParamInfoType("x", porTEXT));

  FuncObj->SetFuncInfo(2, FuncInfoType(GSM_Write_Mixed, 
				       porOUTPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(2, 0, ParamInfoType("output", porOUTPUT,
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(2, 1, ParamInfoType("x", porMIXED));

  FuncObj->SetFuncInfo(3, FuncInfoType(GSM_Write_Behav, 
				       porOUTPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(3, 0, ParamInfoType("output", porOUTPUT,
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(3, 1, ParamInfoType("x", porBEHAV));
  
  FuncObj->SetFuncInfo(4, FuncInfoType(GSM_Write_Nfg, 
				       porOUTPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(4, 0, ParamInfoType("output", porOUTPUT,
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(4, 1, ParamInfoType("x", porNFG,
					    REQUIRED ));
  
  FuncObj->SetFuncInfo(5, FuncInfoType(GSM_Write_EfgFloat, 
				       porOUTPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(5, 0, ParamInfoType("output", porOUTPUT,
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(5, 1, ParamInfoType("x", porEFG_FLOAT));

  FuncObj->SetFuncInfo(6, FuncInfoType(GSM_Write_EfgRational, 
				       porOUTPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(6, 0, ParamInfoType("output", porOUTPUT,
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(6, 1, ParamInfoType("x", porEFG_RATIONAL));

  
  FuncObj->SetFuncInfo(7, FuncInfoType(GSM_Write_list, 
				       porOUTPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(7, 0, ParamInfoType("output", porOUTPUT,
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(7, 1, ParamInfoType
			("x", PortionSpec(porBOOL | porINTEGER | porFLOAT | 
					  porTEXT | porRATIONAL | porMIXED |
					  porBEHAV, 1)));

  FuncObj->SetFuncInfo(8, FuncInfoType(GSM_Write_NfSupport, 
				       porOUTPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(8, 0, ParamInfoType("output", porOUTPUT,
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(8, 1, ParamInfoType("x", porNFSUPPORT));

  FuncObj->SetFuncInfo(9, FuncInfoType(GSM_Write_EfSupport, 
				       porOUTPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(9, 0, ParamInfoType("output", porOUTPUT,
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(9, 1, ParamInfoType("x", porEFSUPPORT));

  FuncObj->SetFuncInfo(10, FuncInfoType(GSM_Write_Strategy, 
				       porOUTPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(10, 0, ParamInfoType("output", porOUTPUT,
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(10, 1, ParamInfoType("x", porSTRATEGY));

  gsm->AddFunction(FuncObj);


  //---------------
  // Set*Format
  //---------------

  FuncObj = new FuncDescObj("SetListFormat", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_SetListFormat, porBOOL, 4));
  FuncObj->SetParamInfo(0, 0, ParamInfoType
			("braces", porBOOL,
			 new BoolRefPortion(_WriteListBraces)));
  FuncObj->SetParamInfo(0, 1, ParamInfoType
			("commas", porBOOL,
			 new BoolRefPortion(_WriteListCommas)));
  FuncObj->SetParamInfo(0, 2, ParamInfoType
			("lf", porINTEGER,
			 new IntRefPortion(_WriteListLF)));
  FuncObj->SetParamInfo(0, 3, ParamInfoType
			("indent", porINTEGER,
			 new IntRefPortion(_WriteListIndent)));
  gsm->AddFunction(FuncObj);


  FuncObj = new FuncDescObj("SetNumericFormat", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_SetNumericFormat, porBOOL, 3));
  FuncObj->SetParamInfo(0, 0, ParamInfoType
			("width", porINTEGER, 
			 new IntRefPortion(_WriteWidth)));
  FuncObj->SetParamInfo(0, 1, ParamInfoType
			("precis", porINTEGER,
			 new IntRefPortion(_WritePrecis)));
  FuncObj->SetParamInfo(0, 2, ParamInfoType
			("expmode", porBOOL,
			 new BoolRefPortion(_WriteExpmode)));
  gsm->AddFunction(FuncObj);


  FuncObj = new FuncDescObj("SetTextFormat", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_SetTextFormat, porBOOL, 1));
  FuncObj->SetParamInfo(0, 0, ParamInfoType
			("quote", porBOOL,
			 new BoolRefPortion(_WriteQuoted)));
  gsm->AddFunction(FuncObj);


  FuncObj = new FuncDescObj("SetSolutionFormat", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_SetSolutionFormat, porBOOL, 1));
  FuncObj->SetParamInfo(0, 0, ParamInfoType
			("info", porINTEGER,
			 new IntRefPortion(_WriteSolutionInfo)));
  gsm->AddFunction(FuncObj);





  //---------------
  // Get*Format
  //---------------

  FuncObj = new FuncDescObj("GetListFormat", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_GetListFormat, porBOOL, 4));
  FuncObj->SetParamInfo(0, 0, ParamInfoType
			("braces", porBOOL,
			 new BoolRefPortion(_WriteListBraces),
			 BYREF ));
  FuncObj->SetParamInfo(0, 1, ParamInfoType
			("commas", porBOOL,
			 new BoolRefPortion(_WriteListCommas),
			 BYREF ));
  FuncObj->SetParamInfo(0, 2, ParamInfoType
			("lf", porINTEGER,
			 new IntRefPortion(_WriteListLF),
			 BYREF ));
  FuncObj->SetParamInfo(0, 3, ParamInfoType
			("indent", porINTEGER,
			 new IntRefPortion(_WriteListIndent),
			 BYREF ));
  gsm->AddFunction(FuncObj);


  FuncObj = new FuncDescObj("GetNumericFormat", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_GetNumericFormat, porBOOL, 3));
  FuncObj->SetParamInfo(0, 0, ParamInfoType
			("width", porINTEGER, 
			 new IntRefPortion(_WriteWidth),
			 BYREF ));
  FuncObj->SetParamInfo(0, 1, ParamInfoType
			("precis", porINTEGER,
			 new IntRefPortion(_WritePrecis),
			 BYREF ));
  FuncObj->SetParamInfo(0, 2, ParamInfoType
			("expmode", porBOOL,
			 new BoolRefPortion(_WriteExpmode),
			 BYREF ));
  gsm->AddFunction(FuncObj);


  FuncObj = new FuncDescObj("GetTextFormat", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_GetTextFormat, porBOOL, 1));
  FuncObj->SetParamInfo(0, 0, ParamInfoType
			("quote", porBOOL,
			 new BoolRefPortion(_WriteQuoted),
			 BYREF ));
  gsm->AddFunction(FuncObj);


  FuncObj = new FuncDescObj("GetSolutionFormat", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_GetSolutionFormat, porBOOL, 1));
  FuncObj->SetParamInfo(0, 0, ParamInfoType
			("info", porINTEGER,
			 new IntRefPortion(_WriteSolutionInfo),
			 BYREF ));
  gsm->AddFunction(FuncObj);






  FuncObj = new FuncDescObj("SetVerbose", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_SetVerbose, porBOOL, 1));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("x", porBOOL) );
  gsm->AddFunction(FuncObj);



  //-------------------- Read --------------------------

  FuncObj = new FuncDescObj("Read", 15);

  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Read_Bool, 
				       porINPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("input", porINPUT, 
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(0, 1, ParamInfoType("x", porBOOL, 
					    REQUIRED, BYREF));

  FuncObj->SetFuncInfo(1, FuncInfoType(GSM_Read_List_Bool, 
				       porINPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(1, 0, ParamInfoType("input", porINPUT,
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(1, 1, ParamInfoType("x", PortionSpec(porBOOL,1), 
					    REQUIRED, BYREF));


  FuncObj->SetFuncInfo(2, FuncInfoType(GSM_Read_Integer, 
				       porINPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(2, 0, ParamInfoType("input", porINPUT,
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(2, 1, ParamInfoType("x", porINTEGER, 
					    REQUIRED, BYREF));
  
  FuncObj->SetFuncInfo(3, FuncInfoType(GSM_Read_List_Integer, 
				       porINPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(3, 0, ParamInfoType("input", porINPUT,
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(3, 1, ParamInfoType("x", PortionSpec(porINTEGER,1), 
					    REQUIRED, BYREF));
  
  
  FuncObj->SetFuncInfo(4, FuncInfoType(GSM_Read_Float, 
				       porINPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(4, 0, ParamInfoType("input", porINPUT,
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(4, 1, ParamInfoType("x", porFLOAT,  
					    REQUIRED, BYREF));
  
  FuncObj->SetFuncInfo(5, FuncInfoType(GSM_Read_List_Float, 
				       porINPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(5, 0, ParamInfoType("input", porINPUT,
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(5, 1, ParamInfoType("x", PortionSpec(porFLOAT,1), 
					    REQUIRED, BYREF));
  
  
  FuncObj->SetFuncInfo(6, FuncInfoType(GSM_Read_Rational, 
				       porINPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(6, 0, ParamInfoType("input", porINPUT,
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(6, 1, ParamInfoType("x", porRATIONAL, 
					    REQUIRED, BYREF));
  
  FuncObj->SetFuncInfo(7, FuncInfoType(GSM_Read_List_Rational, 
				       porINPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(7, 0, ParamInfoType("input", porINPUT,
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(7, 1, ParamInfoType("x", PortionSpec(porRATIONAL,1), 
					    REQUIRED, BYREF));
  
  
  FuncObj->SetFuncInfo(8, FuncInfoType(GSM_Read_Text, 
				       porINPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(8, 0, ParamInfoType("input", porINPUT,
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(8, 1, ParamInfoType("x", porTEXT, 
					    REQUIRED, BYREF));
  
  FuncObj->SetFuncInfo(9, FuncInfoType(GSM_Read_List_Text, 
				       porINPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(9, 0, ParamInfoType("input", porINPUT,
					    REQUIRED, BYREF));
  FuncObj->SetParamInfo(9, 1, ParamInfoType("x", PortionSpec(porTEXT,1), 
					    REQUIRED, BYREF));


  FuncObj->SetFuncInfo(10, FuncInfoType(GSM_Read_MixedFloat, 
					porINPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(10, 0, ParamInfoType("input", porINPUT,
					     REQUIRED, BYREF));
  FuncObj->SetParamInfo(10, 1, ParamInfoType("x", porMIXED_FLOAT, 
					     REQUIRED, BYREF));

  FuncObj->SetFuncInfo(11, FuncInfoType(GSM_Read_MixedRational, 
					porINPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(11, 0, ParamInfoType("input", porINPUT,
					     REQUIRED, BYREF));
  FuncObj->SetParamInfo(11, 1, ParamInfoType("x", porMIXED_RATIONAL, 
					     REQUIRED, BYREF));


  FuncObj->SetFuncInfo(12, FuncInfoType(GSM_Read_BehavFloat, 
					porINPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(12, 0, ParamInfoType("input", porINPUT,
					     REQUIRED, BYREF));
  FuncObj->SetParamInfo(12, 1, ParamInfoType("x", porBEHAV_FLOAT, 
					     REQUIRED, BYREF));

  FuncObj->SetFuncInfo(13, FuncInfoType(GSM_Read_BehavRational,
					porINPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(13, 0, ParamInfoType("input", porINPUT,
					     REQUIRED, BYREF));
  FuncObj->SetParamInfo(13, 1, ParamInfoType("x", porBEHAV_RATIONAL, 
					     REQUIRED, BYREF));


  FuncObj->SetFuncInfo(14, FuncInfoType(GSM_Read_Undefined, 
					porINPUT, 2, 0, funcNONLISTABLE));
  FuncObj->SetParamInfo(14, 0, ParamInfoType("input", porINPUT,
					     REQUIRED, BYREF));
  FuncObj->SetParamInfo(14, 1, ParamInfoType("x", porUNDEFINED, 
					     REQUIRED, BYREF));
  gsm->AddFunction(FuncObj);

  

  //---------------- faked functions -----------------//

  FuncObj = new FuncDescObj("Help", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Help, PortionSpec(porTEXT, 1), 3));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("x", porTEXT));
  FuncObj->SetParamInfo(0, 1, ParamInfoType("udf", porBOOL,
					    new BoolValPortion( true )));
  FuncObj->SetParamInfo(0, 2, ParamInfoType("bif", porBOOL, 
					    new BoolValPortion( true )));
  gsm->AddFunction(FuncObj);

  FuncObj = new FuncDescObj("Manual", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Manual, porBOOL, 2));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("x", porTEXT));
  FuncObj->SetParamInfo(0, 1, ParamInfoType("y", porOUTPUT,
					    new OutputRefPortion(gout)));
  gsm->AddFunction(FuncObj);
  
  FuncObj = new FuncDescObj("HelpVars", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_HelpVars, PortionSpec(porTEXT, 1),
				       1));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("x", porTEXT));
  gsm->AddFunction(FuncObj);

  FuncObj = new FuncDescObj("Clear", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Clear, porBOOL, 0));
  gsm->AddFunction(FuncObj);






  FuncObj = new FuncDescObj("Assign", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Assign, porANYTYPE, 2));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("name", porTEXT));
  FuncObj->SetParamInfo(0, 1, ParamInfoType("value", porANYTYPE, 
					    REQUIRED, BYREF));
  gsm->AddFunction(FuncObj);

  FuncObj = new FuncDescObj("UnAssign", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_UnAssign, porANYTYPE, 1));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("name", porTEXT));
  gsm->AddFunction(FuncObj);


  FuncObj = new FuncDescObj("Date", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Date, porTEXT, 0));
  gsm->AddFunction(FuncObj);


  FuncObj = new FuncDescObj("Randomize", 3);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Randomize_Integer, porINTEGER, 2));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("x", porINTEGER));
  FuncObj->SetParamInfo(0, 1, ParamInfoType("seed", porINTEGER,
					    new IntValPortion(0)));
  FuncObj->SetFuncInfo(1, FuncInfoType(GSM_Randomize_Float, porFLOAT, 2));
  FuncObj->SetParamInfo(1, 0, ParamInfoType("x", porFLOAT));
  FuncObj->SetParamInfo(1, 1, ParamInfoType("seed", porINTEGER,
					    new IntValPortion(0)));
  FuncObj->SetFuncInfo(2, FuncInfoType(GSM_Randomize_Rational, porRATIONAL,2));
  FuncObj->SetParamInfo(2, 0, ParamInfoType("x", porRATIONAL));
  FuncObj->SetParamInfo(2, 1, ParamInfoType("seed", porINTEGER,
					    new IntValPortion(0)));
  gsm->AddFunction(FuncObj);



  FuncObj = new FuncDescObj("IsNull", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_IsNull, porBOOL, 1, 0, 
				       funcLISTABLE));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("x", 
					    PortionSpec(porANYTYPE, 0, 
							porNULLSPEC )));
  gsm->AddFunction(FuncObj);


  FuncObj = new FuncDescObj("Null", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Null, 
				       PortionSpec(porANYTYPE, 0, 
						   porNULLSPEC ),
				       1, 0, funcLISTABLE));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("x", porANYTYPE));
  gsm->AddFunction(FuncObj);



  FuncObj = new FuncDescObj("Shell", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Shell, porINTEGER, 2 ));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("command", porTEXT,
					    new TextValPortion("")));
  FuncObj->SetParamInfo(0, 1, ParamInfoType("spawn", porBOOL, 
					    new BoolValPortion(true)));
  gsm->AddFunction(FuncObj);


  FuncObj = new FuncDescObj("GetEnv", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_GetEnv, porTEXT, 1 ));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("name", porTEXT ) );
  gsm->AddFunction(FuncObj);

  FuncObj = new FuncDescObj("SetEnv", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_SetEnv, porBOOL, 2 ));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("name", porTEXT ) );
  FuncObj->SetParamInfo(0, 1, ParamInfoType("value", porTEXT ) );
  gsm->AddFunction(FuncObj);

  FuncObj = new FuncDescObj("UnSetEnv", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_UnSetEnv, porBOOL, 1 ));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("name", porTEXT ) );
  gsm->AddFunction(FuncObj);


  FuncObj = new FuncDescObj("ExePath", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_ExePath, porTEXT, 2));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("file", porBOOL,
					    new BoolValPortion( true ) ) );
  FuncObj->SetParamInfo(0, 1, ParamInfoType("path", porBOOL,
					    new BoolValPortion( true ) ) );
  gsm->AddFunction(FuncObj);

  FuncObj = new FuncDescObj("Platform", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_Platform, porTEXT, 0));
  gsm->AddFunction(FuncObj);

  FuncObj = new FuncDescObj("GetPath", 1);
  FuncObj->SetFuncInfo(0, FuncInfoType(GSM_GetPath, porTEXT, 2));
  FuncObj->SetParamInfo(0, 0, ParamInfoType("file", porBOOL,
					    new BoolValPortion( true ) ) );
  FuncObj->SetParamInfo(0, 1, ParamInfoType("path", porBOOL,
					    new BoolValPortion( true ) ) );
  gsm->AddFunction(FuncObj);
}







