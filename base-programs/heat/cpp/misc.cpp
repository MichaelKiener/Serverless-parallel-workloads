#include "heat.hpp"
#include "stdlib.h"
#include <stdio.h>
#include <math.h>
#include <float.h>

int initialize_params( algoparam_t *param)
{
	// Set parameters
	param->maxiter = 10;
	//param->initial_res = 5200;
	param->act_res = param->initial_res;
	//param->max_res = 5200;
	param->res_step_size = 1000;
	param->numsrcs = 2;
	(param->heatsrcs) = (heatsrc_t*) malloc( sizeof(heatsrc_t) * (param->numsrcs) );
	param->heatsrcs[0].posx = 0.0;
	param->heatsrcs[0].posy = 0.0;
	param->heatsrcs[0].range = 1.0;
	param->heatsrcs[0].temp = 1.0;
	param->heatsrcs[1].posx = 1.0;
	param->heatsrcs[1].posy = 1.0;
	param->heatsrcs[1].range = 1.0;
	param->heatsrcs[1].temp = 0.5;
	return 1;

}

int initialize( algoparam_t *param )
{

    int i, j;
    double dist;

    // total number of points (including border)
    const int np = param->act_res + 2;

    //
    // allocate memory
    //
    (param->u)     = (double*)malloc( sizeof(double)* np*np );
    (param->uhelp) = (double*)malloc( sizeof(double)* np*np );
    (param->uvis)  = (double*)calloc( sizeof(double),
				      (param->visres+2) *
				      (param->visres+2) );

	#pragma omp parallel for
    for (i=0;i<np;i++){
    	for (j=0;j<np;j++){
    		param->u[i*np+j]=0;
			param->uhelp[i*np+j]=0;
    	}
    }

    if( !(param->u) || !(param->uhelp) || !(param->uvis) )
    {
	fprintf(stderr, "Error: Cannot allocate memory\n");
	return 0;
    }

    for( i=0; i<param->numsrcs; i++ )
    {
	/* top row */
	for( j=0; j<np; j++ )
	{
	    dist = sqrt( pow((double)j/(double)(np-1) -
			     param->heatsrcs[i].posx, 2)+
			 pow(param->heatsrcs[i].posy, 2));

	    if( dist <= param->heatsrcs[i].range )
	    {
		(param->u)[j] +=
		    (param->heatsrcs[i].range-dist) /
		    param->heatsrcs[i].range *
		    param->heatsrcs[i].temp;
	    }
	}

	/* bottom row */
	for( j=0; j<np; j++ )
	{
	    dist = sqrt( pow((double)j/(double)(np-1) -
			     param->heatsrcs[i].posx, 2)+
			 pow(1-param->heatsrcs[i].posy, 2));

	    if( dist <= param->heatsrcs[i].range )
	    {
		(param->u)[(np-1)*np+j]+=
		    (param->heatsrcs[i].range-dist) /
		    param->heatsrcs[i].range *
		    param->heatsrcs[i].temp;
	    }
	}

	/* leftmost column */
	for( j=1; j<np-1; j++ )
	{
	    dist = sqrt( pow(param->heatsrcs[i].posx, 2)+
			 pow((double)j/(double)(np-1) -
			     param->heatsrcs[i].posy, 2));

	    if( dist <= param->heatsrcs[i].range )
	    {
		(param->u)[ j*np ]+=
		    (param->heatsrcs[i].range-dist) /
		    param->heatsrcs[i].range *
		    param->heatsrcs[i].temp;
	    }
	}

	/* rightmost column */
	for( j=1; j<np-1; j++ )
	{
	    dist = sqrt( pow(1-param->heatsrcs[i].posx, 2)+
			 pow((double)j/(double)(np-1) -
			     param->heatsrcs[i].posy, 2));

	    if( dist <= param->heatsrcs[i].range )
	    {
		(param->u)[ j*np+(np-1) ]+=
		    (param->heatsrcs[i].range-dist) /
		    param->heatsrcs[i].range *
		    param->heatsrcs[i].temp;
	    }
	}
    }

    return 1;
}

/*
 * free used memory
 */
int finalize( algoparam_t *param )
{
    if( param->u ) {
	free(param->u);
	param->u = 0;
    }

    if( param->uhelp ) {
	free(param->uhelp);
	param->uhelp = 0;
    }

    if( param->uvis ) {
	free(param->uvis);
	param->uvis = 0;
    }

    return 1;
}