# Systems Programming Assignment #0

We're likely to restart because... John-Austen Francisco.

## Assignment

- Open a comma delimited text file and sort it by number or letter, depending on what it is.
- Give the user the option to choose the sorting algorithm, quicksort or insertion sort, through a command line flag.
- Add useful errors if the program is used incorrectly.
- Write a description about how to use the program and how the program was tested.

## Questions for Franny

- How do we store the list that we're sorting, is it an array or a linked list? This totally matters because the implementing sorting algorithms on a linked list is much more complex.
- The prototype that you mentioned in the assignment description `int quickSort(void* toSort, int (*comparator)(void*, void*))` is peculiar. Answer me:
  1. Why is a sorting function returning a number?
  2. How do we modify the list to make it sorted if the sorting function doesn't return the sorted list?

## Authors

Maanya Tandon and Sam Olagun
