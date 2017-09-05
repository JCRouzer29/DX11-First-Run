//*********************************************************
//********** Header for including all lab files ***********
//*********************************************************

#include "../../DX11 Work Base\Source\Lab.h"

#define LAB_CODE 1

#if LAB_CODE == 1
#include "Lab1\Lab1.h"
#define LAB Lab1

#elif LAB_CODE == 2
//#include "Lab2\Lab2.h"
#include "Lab2_3\Lab2_3.h"
#define LAB Lab2

#elif LAB_CODE == 3
#include "Lab3\Lab3.h"
#define LAB Lab3

#elif LAB_CODE == 4
#include "Lab4\Lab4.h"
#define LAB Lab4

#elif LAB_CODE == 5
#include "Lab5\Lab5.h"
#define LAB Lab5

#elif LAB_CODE == 6
#include "Lab6\Lab6.h"
#define LAB Lab6

#endif
