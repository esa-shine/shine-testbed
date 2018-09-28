/** @file heapSort.cpp
 *  \brief A file to generate heap struct
 *    */
#include <stdio.h>
#include <math.h>
#include "heapSort.h"

int h,arr_size;

/* MAX HEAP - Order Ascending */
void maxheap_property(NODE a[],int i)
{
	int l,r,largest;
	NODE temp;
	l=2*i+1; r=2*i+2;

	if(l<h && a[l].degree>a[i].degree) largest=l;
	else  largest=i;

	if(r<h && a[r].degree>a[largest].degree) largest=r;

	if(largest!=i)
	{
	    temp.id=a[i].id;
		temp.degree=a[i].degree;
		temp.arcList=a[i].arcList;

		a[i].id=a[largest].id;
		a[i].degree=a[largest].degree;
		a[i].arcList=a[largest].arcList;

		a[largest].id=temp.id;
		a[largest].degree=temp.degree;
		a[largest].arcList=temp.arcList;

		maxheap_property(a,largest);
	}
}

void build_maxheap(NODE a[])
{
	int i,j;
	h=arr_size;
	j=floor((arr_size/2));

	for(i=j-1;i>=0;i--)
		maxheap_property(a,i);
}

void max_heap_sort(NODE a[])
{
	int i;
	NODE temp;

	build_maxheap(a);

	for(i=arr_size-1;i>=1;i--)
	{
	    temp.id=a[0].id;
		temp.degree=a[0].degree;
		temp.arcList=a[0].arcList;

		a[0].id=a[i].id;
        a[0].degree=a[i].degree;
        a[0].arcList=a[i].arcList;

		a[i].id=temp.id;
		a[i].degree=temp.degree;
		a[i].arcList=temp.arcList;

		h--;

		maxheap_property(a,0);
	}
}
/***********************************************************************/

/* MIN HEAP - Order Descending */
void minheap_property(NODE a[],int i)
{
	int l,r,smallest;
	NODE temp;
	l=2*i+1; r=2*i+2;

	if(l<h && a[l].degree<a[i].degree) smallest=l;
	else  smallest=i;

	if(r<h && a[r].degree<a[smallest].degree) smallest=r;

	if(smallest!=i)
	{
		temp.id=a[i].id;
		temp.degree=a[i].degree;
		temp.arcList=a[i].arcList;

		a[i].id=a[smallest].id;
		a[i].degree=a[smallest].degree;
		a[i].arcList=a[smallest].arcList;

		a[smallest].id=temp.id;
		a[smallest].degree=temp.degree;
		a[smallest].arcList=temp.arcList;

		minheap_property(a,smallest);
	}
}

void build_minheap(NODE a[])
{
	int i,j;
	h=arr_size;
	j=floor((arr_size/2));

	for(i=j-1;i>=0;i--)
		minheap_property(a,i);
}

void min_heap_sort(NODE a[])
{
	int i;
	NODE temp;

	build_minheap(a);

	for(i=arr_size-1;i>=1;i--)
	{
		temp.id=a[0].id;
		temp.degree=a[0].degree;
		temp.arcList=a[0].arcList;

		a[0].id=a[i].id;
        a[0].degree=a[i].degree;
        a[0].arcList=a[i].arcList;

		a[i].id=temp.id;
		a[i].degree=temp.degree;
		a[i].arcList=temp.arcList;

		h--;

		minheap_property(a,0);
	}
}
/**********************************************************************/

void descendingHeapSort(NODE *a, int n){
	arr_size = n;

	min_heap_sort(a);
}

void ascendingHeapSort(NODE *a, int n){
	arr_size = n;

	max_heap_sort(a);
}
