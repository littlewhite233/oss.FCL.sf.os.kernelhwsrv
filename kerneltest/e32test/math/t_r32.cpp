// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// e32test\math\t_r32.cpp
// T_R32.CPP - Test routines for TReal32
// Also note that these tests do not generally include testing of special values.  This is done
// in T_R96 tests and conversions are tested thoroughly, so explicit tests are unnecessary here.
// Overview:
// Test functionality of operations on 32bit real numbers.
// API Information:
// TReal32. 
// Details:
// - Test the conversion from TReal to TReal32 is as expected.
// - Check addition, subtraction and multiplication of 32-bit floating point
// numbers are as expected.
// - Check division of 32-bit floating-point numbers and verify that it is
// panicked when divided by zero.
// - Test arithmetic exceptions are raised for
// - overflow error during addition, subtraction.
// - overflow, underflow errors during multiplication.
// - overflow, underflow, divided by zero errors during division.
// - overflow, underflow, invalid operation errors during conversion
// from double to float.
// - Check unary operator, equalities and inequalities operators, pre/post 
// increment, decrement operators with TReal32 are as expected.
// Platforms/Drives/Compatibility:
// All 
// Assumptions/Requirement/Pre-requisites:
// Failures and causes:
// Base Port information:
// 
//

#include "t_math.h"
#include "t_real32.h"

#if defined(__VC32__)
// Some symbols generated by the VC++ compiler for floating point stuff.
extern "C" {
GLDEF_D TInt _adj_fdiv_m32;
}
#endif

// Data for tests from T_R32DTA.cpp 
GLREF_D TReal32 addInput[];
GLREF_D TReal32 subInput[];
GLREF_D TReal32 multInput[];
GLREF_D TReal32 divInput[];
GLREF_D TReal32 unaryInput[];
GLREF_D TReal32 incDecInput[];
GLREF_D TInt sizeAdd;
GLREF_D TInt sizeSub;
GLREF_D TInt sizeMult;
GLREF_D TInt sizeDiv;
GLREF_D TInt sizeUnary;
GLREF_D TInt sizeIncDec;

#if defined (__WINS__) || defined (__X86__)
// Functions from EMGCC32.CPP
GLREF_C TReal32 __addsf3(TReal32 a1,TReal32 a2);
GLREF_C TReal32 __subsf3(TReal32 a1,TReal32 a2);
GLREF_C TReal32 __mulsf3(TReal32 a1,TReal32 a2);
GLREF_C TReal32 __divsf3(TReal32 a1,TReal32 a2);
GLREF_C TReal32 __truncdfsf2(TReal64 a1);
#endif

GLDEF_D TReal32 NaNTReal32;
GLDEF_D TReal32 posInfTReal32;
GLDEF_D TReal32 negInfTReal32;
GLDEF_D const TReal32 minDenormalTReal32=1.4E-45f;
GLDEF_D TReal NaNTReal;
GLDEF_D TReal posInfTReal;
GLDEF_D TReal negInfTReal;

enum TOrder
	{
	ELessThan,
	EEqual,
	EGreaterThan
	};

LOCAL_D RTest test(_L("T_R32"));

LOCAL_C void initSpecialValues()
//
// Initialise special values
//
	{

	SReal32 *p32=(SReal32*)&NaNTReal32;
	p32->sign=0;
	p32->exp=KTReal32SpecialExponent;
	p32->man=0x7fffff;

	p32=(SReal32*)&posInfTReal32;
	p32->sign=0;
	p32->exp=KTReal32SpecialExponent;
	p32->man=0;

	p32=(SReal32*)&negInfTReal32;
	p32->sign=1;
	p32->exp=KTReal32SpecialExponent;
	p32->man=0;

	SReal64 *p64=(SReal64*)&NaNTReal;
	p64->sign=0;
	p64->exp=KTReal64SpecialExponent;
	p64->lsm=0xffffffffu;
	p64->msm=0xfffff;

	p64=(SReal64*)&posInfTReal;
	p64->sign=0;
	p64->exp=KTReal64SpecialExponent;
	p64->lsm=0;
	p64->msm=0;
	
	p64=(SReal64*)&negInfTReal;
	p64->sign=1;
	p64->exp=KTReal64SpecialExponent;
	p64->lsm=0;
	p64->msm=0;
	}

LOCAL_C void testConvert()
//
//	Conversion tests
//
	{

	TRealX f;

	TReal input[]=
		{
		KMaxTReal32inTReal,KMinTReal32inTReal,-KMaxTReal32inTReal,-KMinTReal32inTReal,
		KMaxTReal32inTReal,KMinTReal32inTReal,-KMaxTReal32inTReal,-KMinTReal32inTReal,
		3.4027E+38,1.1755E-38,-3.4027E+38,-1.1755E-38,
		0.0,64.5,-64.5,1.54E+18,-1.54E+18,4.72E-22,-4.72E-22,
		posInfTReal,negInfTReal,KNegZeroTReal,
		1.4E-45,-1.4E-45,2E-41,-2E-41,1E-38,-1E-38		
		};

	TReal32 expect[]=
		{
		KMaxTReal32,KMinTReal32,-KMaxTReal32,-KMinTReal32,
		KMaxTReal32,KMinTReal32,-KMaxTReal32,-KMinTReal32,
		3.4027E+38f,1.17550E-38f,-3.40270E+38f,-1.17550E-38f,
		0.0f,64.5f,-64.5f,1.54E+18f,-1.54E+18f,4.72E-22f,-4.72E-22f,
		posInfTReal32,negInfTReal32,KNegZeroTReal32,
		1.4E-45f,-1.4E-45f,2E-41f,-2E-41f,1E-38f,-1E-38f				
		};

	TInt size=sizeof(input)/sizeof(TReal);

	for (TInt ii=0; ii<size; ii++)
		{
		f=TRealX(expect[ii]);
		test(f==TRealX(TReal32(input[ii])));
		}

	// NaN
//	TReal a=NaNTReal;
	TReal32 b=NaNTReal32;
	f=TRealX(b);
//	test(f!=TRealX(TReal32(a)));
	test(f.IsNaN());
		
	// See EON Software Defects Bug Report no. HA-287
	// There is a bug in MSDev compiler which means comparing TReal32's directly
	// does not always work, hence...
	/*
	test(BitTest(TReal32(3.40270E+38),3.40270E+38f));	// this works
	// (BitTest() checks for all 32 bits being identical
	
	TReal32 a=TReal32(3.40270E+38);
	TReal32 b=3.40270E+38f;
	TReal64 c=3.40270E+38;
	TReal32 d=TReal32(c);

	test(a==b);									// this works
	test(d==b);									// this works
	test(TRealX(TReal32(c))==TRealX(b));		// this works
	test(TReal64(TReal32(c))==TReal64(b));		// this panics
	test(TReal32(c)==b);						// this panics					  
 	test(TReal32(3.40270E+38)==3.40270E+38f);	// this panics

	// As expected, all these work fine under ARM.
	*/
	}

LOCAL_C void testAdd()
//
//	Addition tests
//
	{
	TReal32 f,g,h;
	TRealX ff,gg,hh;
	
	for (TInt ii=0; ii<sizeAdd-1; ii++)
		{										  
		f=addInput[ii];
		g=addInput[ii+1];
		ff=TRealX(f);
		gg=TRealX(g);
		// Test commute
		test(f+g == g+f);
		// Test PC real addition using fp-hardware same as TRealX addition
		test(TRealX(f+g)==TRealX(TReal32(ff+gg)));
		test(TRealX(g+f)==TRealX(TReal32(ff+gg)));
		// Test hex-encoded constants for TReal32s generated on PC using fp-hardware same as 
		// TRealX addition
		test(TRealX(*(TReal32*)&addArray[ii])==TRealX(f+g));
		test(TRealX(*(TReal32*)&addArray[ii])==TRealX(g+f));
		// similarly to tests above ...
		h=g;
		hh=gg;
		hh+=ff;
		test(TRealX(h+=f)==TRealX(TReal32(hh)));
		test(TRealX(h)==TRealX(TReal32(hh)));
		test(TRealX(*(TReal32*)&addArray[ii])==TRealX(h));
		//
		h=f;
		hh=ff;
		hh+=gg;
		test(TRealX(h+=g)==TRealX(TReal32(hh)));
		test(h==TReal32(hh));
		test(TRealX(*(TReal32*)&addArray[ii])==TRealX(h));
		}
	}

LOCAL_C void testSubt()
//
// Subtraction tests
//
	{
	TReal32 f,g,h;
	TRealX ff,gg,hh;

	for (TInt ii=0; ii<sizeSub-1; ii++)
		{
		f=subInput[ii];
		g=subInput[ii+1];
		ff=TRealX(f);
		gg=TRealX(g);
		//

// This test fails on GCC (with -O1 switch). The reason is that
// comparing two intermediate floats is unpredictable.
// See http://www.parashift.com/c++-faq-lite/newbie.html#faq-29.18
#ifndef __GCC32__	
		test(f-g == -(g-f));
#endif
		//
		test(TRealX(f-g)==TRealX(TReal32(ff-gg)));
		test(TRealX(g-f)==TRealX(TReal32(gg-ff)));
		test(TRealX(*(TReal32*)&subArray[ii])==TRealX(f-g));
		test(TRealX(*(TReal32*)&subArray[ii])==TRealX(-(g-f)));
		//
		h=g;
		hh=gg;
		hh-=ff;
		test(TRealX(h-=f)==TRealX(TReal32(hh)));
		test(TRealX(h)==TRealX(TReal32(hh)));
		test(TRealX(*(TReal32*)&subArray[ii])==TRealX(-h));
		//
		h=f;
		hh=ff;
		hh-=gg;
		test(TRealX(h-=g)==TRealX(TReal32(hh)));
		test(TRealX(h)==TRealX(TReal32(hh)));
		test(TRealX(*(TReal32*)&subArray[ii])==TRealX(h));
		}
	}

LOCAL_C void testMult()
//
//	Multiplication test
//
	{
	TReal32 f,g,h;
	TRealX ff,gg,hh;
	
	for (TInt ii=0; ii<sizeMult-1; ii++)
		{
		f=multInput[ii];
		g=multInput[ii+1];
		ff=TRealX(f);
		gg=TRealX(g);
		//
		test(f*g == g*f);
		//
		test(TRealX(f*g)==TRealX(TReal32(ff*gg)));
		test(TRealX(g*f)==TRealX(TReal32(gg*ff)));
		test(TRealX(*(TReal32*)&multArray[ii])==TRealX(f*g));
		test(TRealX(*(TReal32*)&multArray[ii])==TRealX(g*f));
		//
		h=f;		
		hh=ff;
		hh*=gg;
		test(TRealX(h*=g)==TRealX(TReal32(hh)));
		test(TRealX(h)==TRealX(TReal32(hh)));
		test(TRealX(*(TReal32*)&multArray[ii])==TRealX(h));
		//
		h=g;
		hh=gg;
		hh*=ff;
		test(TRealX(h*=f)==TRealX(TReal32(hh)));
		test(TRealX(h)==TRealX(TReal32(hh)));
		test(TRealX(*(TReal32*)&multArray[ii])==TRealX(h));
		}
	}

LOCAL_C void testDiv()
//
//	Division test
//
	{
	TReal32 f,g,h;
	TRealX ff,gg,hh;
	TInt count=0;
	
	// Panic: Divide by Zero
	// f=1.0;
	// g=0.0;
	// f/=g;

	for (TInt ii=0; ii<sizeDiv-1; ii++)
		{
		f=divInput[ii];
		g=divInput[ii+1];
		ff=TRealX(f);
		gg=TRealX(g);
		if (g!=0.0)
			{
			test(TRealX(f/g)==TRealX(TReal32(ff/gg)));
			test(TRealX(*(TReal32*)&divArray[count])==TRealX(f/g));
			//
			h=f;
			hh=ff;
			hh/=gg;
			test(TRealX(h/=g)==TRealX(TReal32(hh)));
			test(TRealX(h)==TRealX(TReal32(hh)));
			test(TRealX(*(TReal32*)&divArray[count])==TRealX(h));
			++count;
			}
		if (f!=0.0)
			{
			test(TRealX(g/f)==TRealX(TReal32(gg/ff)));
			h=g;
			hh=gg;
			hh/=ff;
			test(TRealX(h/=f)==TRealX(TReal32(hh)));
			test(h==TReal32(hh));
			}
		};

	//Additional test
	f=3.9999f;
	g=KMinTReal32;
	ff=TRealX(f);
	gg=TRealX(g);
	test(TRealX(f/g)==TRealX(TReal32(ff/gg)));
	h=f;
	hh=ff;
	hh/=gg;
	test(TRealX(h/=g)==TRealX(TReal32(hh)));
	test(TRealX(h)==TRealX(TReal32(hh)));
	}

#if defined (__WINS__) || defined (__X86__)

LOCAL_C	 void testArithmeticExceptionRaising()
//
// Test that UP_GCC.CPP raise exceptions correctly by calling functions from EMGCC32.CPP which
// are copies of those in UP_GCC.CPP.  To be used in debugger only.
// Added by AnnW, December 1996
//
	{
	TReal32 f,g,h;

	// Addition - possible errors are overflow, argument or none
	// NB no underflow

	f=NaNTReal32;
	h=__addsf3(f,f);	// argument

	f=KMaxTReal32;
	h=__addsf3(f,f);	// overflow
	
	f=1.0f;
	g=2.0f;
	h=__addsf3(f,g);	// none
	test(h==3.0f);

	// Subtraction - possible errors are overflow, argument or none
	// NB no underflow

	f=NaNTReal32;
	h=__subsf3(f,f);	// argument

	f=KMaxTReal32;
	g=-KMaxTReal32;
	h=__subsf3(f,g);	// overflow
  
	f=1.0f;
	g=2.0f;
	h=__subsf3(f,g);	// none
	test(h==-1.0f);

	// Multiplication - possible errors are argument, overflow, underflow or none

	f=NaNTReal32;
	h=__mulsf3(f,f);	// argument

	f=KMaxTReal32;
	g=2.0f;
	h=__mulsf3(f,g);	// overflow

	f=minDenormalTReal32;
	g=0.1f;
	h=__mulsf3(f,g);	// underflow

	f=1.0f;
	g=2.0f;
	h=__mulsf3(f,g);	// none
	test(h==2.0f);

	// Division - possible errors are overflow, underflow, divide by zero, argument or none

	f=KMaxTReal32;
	g=0.5f;
	h=__divsf3(f,g);	// overflow

	f=minDenormalTReal32;
	g=10.0f;
	h=__divsf3(f,g);	// underflow
	
	f=4.0f;
	g=0.0f;
	h=__divsf3(f,g);	// divide by zero
	
	f=0.0f;
	g=0.0f;
	h=__divsf3(f,g);	// argument

	f=1.0f;
	g=2.0f;
	h=__divsf3(f,g);	// none
	test(h==0.5f);

	// Converting double to float - possible errors are overflow, underflow, invalid operation or none
	TReal64 d;

	d=1.0E+50;
	f=__truncdfsf2(d);	// overflow

	d=1.0E-50;
	f=__truncdfsf2(d);	// underflow

	d=KNaNTReal64;
	f=__truncdfsf2(d);	// invalid operation

	d=4.0;	
	f=__truncdfsf2(d);	// none
	}

#endif

LOCAL_C void testUnary()
//
//	Unary operator tests
//
	{
	TReal32 f;
	TRealX g;
	
	for (TInt ii=0; ii<sizeUnary-1; ii++)
		{
		f=unaryInput[ii];
		g=TRealX(f);
		test(TRealX(-f)==TRealX(TReal32(-g)));
		test(TRealX(-f)==TRealX(0.0f-f));
		test(TRealX(+f)==TRealX(TReal32(g)));
		test(TRealX(+f)==TRealX(0.0f+f));
		test(TRealX(*(TReal32*)&unaryArray[ii])==TRealX(-f));
		}
	}

LOCAL_C void testEqualities(const TReal& aA, TOrder aOrder, const TReal& aB)
//
//	Test equality/inequality functions on aA and aB
//	aOrder specifies the operand's relative sizes
//
	{

	//	Tautologies
	test((aA>aA) ==FALSE);
	test((aA<aA) ==FALSE);
	test((aA>=aA)==TRUE);
	test((aA<=aA)==TRUE);
	test((aA==aA)==TRUE);
	test((aA!=aA)==FALSE);
	if (aOrder!=EEqual)
		{
		test((aA==aB)==FALSE);
  		test((aA!=aB)==TRUE);
		}
	if (aOrder==ELessThan)
		{
		test((aA<aB) ==TRUE);
		test((aA<=aB)==TRUE);
		test((aA>aB) ==FALSE);
		test((aA>=aB)==FALSE);
		}
	if (aOrder==EEqual)
		{
		test((aA==aB)==TRUE);
		test((aA!=aB)==FALSE);
		test((aA>=aB)==TRUE);
		test((aA<=aB)==TRUE);
		test((aA>aB)==FALSE);
		test((aA<aB)==FALSE);
		}
	if (aOrder==EGreaterThan)
		{
		test((aA>aB) ==TRUE);
		test((aA>=aB)==TRUE);
		test((aA<aB) ==FALSE);
		test((aA<=aB)==FALSE);
		}
	}
	 
LOCAL_C void testEqualities()
//
//	Test >, <, >=, <=, ==, !=
//
	{
	TInt i, size;
	TReal32 lessThanMax = KMaxTReal32-TReal32(1.0E+32);
	TReal32 greaterThanMin = 1.17550E-38f;
	TReal32 zero(0.0f);
	
	TReal32 positive[] =
	{KMinTReal32,5.3824705E-26f,1.0f,2387501.0f,5.3824705E+28f,KMaxTReal32};

	TReal32 large[] =
	{2.0f,KMaxTReal32,-lessThanMax,greaterThanMin,-KMinTReal32,10.4058482f,-10.4058482f,
	1.2443345E+14f,1.2443345E+14f,-1.3420344E-16f,132435.97f,5.0E-6f,9.6f,-8.0f}; 
	
	TReal32 small[] =
	{1.0f,lessThanMax,-KMaxTReal32,KMinTReal32,-greaterThanMin,10.4058474f,-10.4058496f,
	5.0E-10f,1.2443345E+10f,-5.0382470E+25f,-132435.97f,-5.1E-6f,8.0f,-9.6f};
	
	TReal32 equal[] =							  // Same as large[]
	{2.0f,KMaxTReal32,-lessThanMax,greaterThanMin,-KMinTReal32,10.4058482f,-10.4058482f,
	1.2443345E+14f,1.2443345E+14f,-1.3420344E-16f,132435.97f,5.0E-6f,9.6f,-8.0f}; 


	// Tests with zero

	size = sizeof(positive)/sizeof(TReal32);
	
	test.Start(_L("Zero"));
	testEqualities(zero, EEqual, zero);
	for (i=0; i<size; i++)
		{
		testEqualities(positive[i], EGreaterThan, zero);
		testEqualities(-positive[i], ELessThan, zero);
		testEqualities(zero, ELessThan, positive[i]);
		testEqualities(zero, EGreaterThan, -positive[i]);
		}

	// Test boundary and other numbers
	
	size = sizeof(large)/sizeof(TReal32);
	
	test.Next(_L("Nonzero"));
	for (i=0; i<size; i++)
		{
		testEqualities(large[i], EGreaterThan, small[i]);
		testEqualities(small[i], ELessThan, large[i]);
		testEqualities(large[i], EEqual, equal[i]);
		}

	test.End();
	}

LOCAL_C void testIncDec()
//
//	Test Pre/Post - increment/decrement
//
	{

	TInt ii;
	TReal32 f;
	TRealX g;
	
	test.Start(_L("Pre-increment"));
	
	for (ii=0; ii<sizeIncDec; ii++)
		{
		f=incDecInput[ii];
		g=TRealX(f);
		test(TRealX(f)==TRealX(TReal32(g)));
		test(TRealX(++f)==TRealX(TReal32(++g)));
		test(TRealX(*(TReal32*)&preIncArray1[ii])==TRealX(f));
		test(TRealX(f)==TRealX(TReal32(g)));
		test(TRealX(++f)==TRealX(TReal32(++g)));
		test(TRealX(*(TReal32*)&preIncArray2[ii])==TRealX(f));
		test(TRealX(f)==TRealX(TReal32(g)));
		}
	
	test.Next(_L("Post-increment"));

	for (ii=0; ii<sizeIncDec; ii++)
		{
		f=incDecInput[ii];
		g=TRealX(f);
		test(TRealX(f)==TRealX(TReal32(g)));
		test(TRealX(f++)==TRealX(TReal32(g++)));
		test(TRealX(*(TReal32*)&postIncArray1[ii])==TRealX(f));
		test(TRealX(f)==TRealX(TReal32(g)));
		test(TRealX(f++)==TRealX(TReal32(g++)));
		test(TRealX(*(TReal32*)&postIncArray2[ii])==TRealX(f));
		test(TRealX(f)==TRealX(TReal32(g)));
		}
	
	test.Next(_L("Pre-decrement"));

	for (ii=0; ii<sizeIncDec; ii++)
		{
		f=incDecInput[ii];
		g=TRealX(f);
		test(TRealX(f)==TRealX(TReal32(g)));
		test(TRealX(--f)==TRealX(TReal32(--g)));
		test(TRealX(*(TReal32*)&preDecArray1[ii])==TRealX(f));
		test(TRealX(f)==TRealX(TReal32(g)));
		test(TRealX(--f)==TRealX(TReal32(--g)));
		test(TRealX(*(TReal32*)&preDecArray2[ii])==TRealX(f));
		test(TRealX(f)==TRealX(TReal32(g)));
		}
	
	test.Next(_L("Post-decrement"));

	for	(ii=0; ii<sizeIncDec; ii++)
		{
		f=incDecInput[ii];
		g=TRealX(f);
		test(TRealX(f)==TRealX(TReal32(g)));
		test(TRealX(f--)==TRealX(TReal32(g--)));
		test(TRealX(*(TReal32*)&postDecArray1[ii])==TRealX(f));
		test(TRealX(f)==TRealX(TReal32(g)));
		test(TRealX(f--)==TRealX(TReal32(g--)));
		test(TRealX(*(TReal32*)&postDecArray2[ii])==TRealX(f));
		test(TRealX(f)==TRealX(TReal32(g)));
		}
	test.End();
	}

LOCAL_C void _matherr(TExcType aType)
	{
	test.Printf(_L("_matherr: Exception type %u handled\n"),TUint(aType));
	}

GLDEF_C TInt E32Main()
//
//	Test TReal32
//
    {	  
	
	test.Title();

	User::SetExceptionHandler(_matherr,KExceptionFpe);

	initSpecialValues();

	test.Start(_L("Conversion from TReal to TReal32"));
	testConvert();
	test.Next(_L("Addition"));
	testAdd();
	test.Next(_L("Subtraction"));	
	testSubt();
	test.Next(_L("Multiplication"));
	testMult();
	test.Next(_L("Division"));
	testDiv();
#if defined (__WINS__) || defined (__X86__)
	test.Next(_L("Arithmetic which emulates UP_GCC and raises an exception"));
	testArithmeticExceptionRaising();
#endif
	test.Next(_L("Unary Operators"));
	testUnary();
	test.Next(_L("Equalities and Inequalities"));
	testEqualities();
	test.Next(_L("Increment and Decrement"));
	testIncDec();

	test.End();
	return(KErrNone);
    }

