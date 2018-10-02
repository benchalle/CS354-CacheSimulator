#include <stdio.h>
////////////////////////////////////////////////////////////////////////////////
// Main File:        cache2Dcols.c
// This File:        cache2Dcols.c
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
  int arr2D[3000][500]; //create a 2D array that is 3000 by 500
  
  
int main(){
//for each cols go through the rows and save its index in the element (row index + column index)
 for(int col  = 0; col<3000; col++){
   for(int row = 0; row<500; row++){
        arr2D[row][col] = row + col;
   
   }
 } 
  
  
  
  return 0; 
}
