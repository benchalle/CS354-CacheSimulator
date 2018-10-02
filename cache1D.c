#include <stdio.h>
////////////////////////////////////////////////////////////////////////////////
// Main File:        cache1D.c
// This File:        cache1D.c
// Other Files:      none
// Semester:         CS 354 Spring 2018
//
// Author:           Benjamin Challe
// Email:            bchalle@wisc.edu
// CS Login:         challe
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of 
//                   of any information you find.
////////////////////////////////////////////////////////////////////////////////

  int arr[100000]; //create an array of 100000 ints
  
int main(){
//for each element save in it its index
  for(int i =0; i<100000; i++){
    arr[i] = i;
  }

  return 0;
}
