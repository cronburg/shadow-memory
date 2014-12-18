#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "shadow.c"

/* Future library code for snapshotting allocations */
ShadowMap* my_sm;

void* shim_malloc(size_t size) {
  void* ret = malloc(size);
  int i;
  for (i = 0; i < size; i++) {
    shadow_set_meta_bits(my_sm, (int)ret + i, (U8)0xff);
  }
  return ret;
}

void shim_free(void* ptr) {
  free(ptr);
  int i = 0;
  U8 curr;
  while (1) {
    shadow_get_meta_bits(my_sm, (int)ptr + i, &curr);
    if (curr != 0xff)
      break;
    shadow_set_meta_bits(my_sm, (int)ptr + i, 0x00);
    i++;
  }

}

/**
 * merge() 
 * Merge two sorted arrays, A with a  integers and 
 * B with b integers, into a sorted array C.
 **/
void 
merge (int *A, int a, int *B, int b, int *C) 
{
  int i,j,k;
  i = 0; 
  j = 0;
  k = 0;
  while (i < a && j < b) {
    if (A[i] <= B[j]) {
	  /* copy A[i] to C[k] and move the pointer i and k forward */
	  C[k] = A[i];
	  i++;
	  k++;
    }
    else {
      /* copy B[j] to C[k] and move the pointer j and k forward */
      C[k] = B[j];
      j++;
      k++;
    }
  }
  /* move the remaining elements in A into C */
  while (i < a) {
    C[k]= A[i];
    i++;
    k++;
  }
  /* move the remaining elements in B into C */
  while (j < b)  {
    C[k]= B[j];
    j++;
    k++;
  }
}  

/**
 * merge_sort()
 * Sort array A with n integers, using merge-sort algorithm.
 **/
void 
merge_sort(int *A, int n) 
{
  int i;
  int *A1, *A2;
  int n1, n2;

  // TODO: better snapshot replacement
  //snapshot(my_sm);

  if (n < 2)
    return;   /* the array is sorted when n=1.*/
  
  /* divide A into two array A1 and A2 */
  n1 = n / 2;   /* the number of elements in A1 */
  n2 = n - n1;  /* the number of elements in A2 */
  A1 = (int*)shim_malloc(sizeof(int) * n1);
  A2 = (int*)shim_malloc(sizeof(int) * n2);
  
  /* move the first n/2 elements to A1 */
  for (i =0; i < n1; i++) {
    A1[i] = A[i];
  }
  /* move the rest to A2 */
  for (i = 0; i < n2; i++) {
    A2[i] = A[i+n1];
  }
  /* recursive call */
  merge_sort(A1, n1);
  merge_sort(A2, n2);

  /* conquer */
  merge(A1, n1, A2, n2, A);
  shim_free(A1);
  shim_free(A2);
}

/**
 *  main()
 *  The main function for input and output 
 **/
int
main(int argv, char** args)
{
  int i;
  int A[] = {8,3,6,2,7,4,4,1,9,5};
  int n = 10;

  my_sm = malloc(sizeof(ShadowMap));
  my_sm->shadow_bits = 1;
  my_sm->application_bits = 1;
  my_sm->num_distinguished = 1;
  shadow_initialize_map(my_sm);

  //A = (int *)shim_malloc(sizeof(int) * 10);

  /* print the original array */
  printf("************** Result **************\n");
  printf("The input array is: ");
  for (i = 0; i < n; i++) {
    printf("%d ", A[i]);
  }
  printf("\n");

  /* merge sort A */
  merge_sort(A, n);

  /* print the sorted array */
  printf("The sorted array is: ");
  for (i = 0; i < n; i++) {
    printf("%d ", A[i]);
  }
  printf("\n");
  //shim_free(A);

  shadow_destroy_map(my_sm);
  free(my_sm);
  printf("Exiting main()\n");
  return 0;
} 

