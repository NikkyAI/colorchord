//Copyright 2015 <>< Charles Lohr under the ColorChord License.

#include "outdrivers.h"
#include "notefinder.h"
#include <stdio.h>
#include "parameters.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "color.h"
#include "CNFG.h"

//Uses: note_amplitudes2[note] for how many lights to use.
//Uses: note_amplitudes_out[note] for how bright it should be.

#define MAX_LEDS_PER_NOTE 512

extern short screenx, screeny;

struct DRODriver
{
	int index;
	int spokes;
	int leds;
	int * history;
};


static void DROUpdate(void * id, struct NoteFinder*nf)
{
	struct DRODriver * d = (struct DRODriver*)id;
	float cw = ((float)(screenx)) / 2.0;
	float ch = ((float)(screeny)) / 2.0;
	
	// TODO: write current leds colors to history

	int i;
	for( i = 0; i < d->leds; i++ )
	{
		d->history[d->index*d->leds + i] = OutLEDs[i*3+0] | (OutLEDs[i*3+1] <<8)|(OutLEDs[i*3+2] <<16);
	}

	// printf("render %d\n", d->index);

	RDPoint pts[6];
	// render all spokes in history
	for(int i = 0; i < d->leds; i++ )
	{
		float sizeA = sqrtf( screenx * screenx + screeny * screeny ) * ((float)(i+1) / (float)d->leds) / 3.0;
		float sizeB = sqrtf( screenx * screenx + screeny * screeny ) * ((float)(i+2) / (float)d->leds) / 3.0;
		// printf("%d %f %f\n", i, sizeA, sizeB);
		for(int spoke = 0; spoke < d->spokes; spoke++)
		{
			float angA = 6.28318 * (float)spoke / (float)d->spokes;
			float angB = 6.28318 * (float)(spoke+1) / (float)d->spokes + .002;
			pts[0].x = cw + cos(angA) * sizeA;
			pts[0].y = ch + sin(angA) * sizeA;
			pts[1].x = cw + cos(angA) * sizeB;
			pts[1].y = ch + sin(angA) * sizeB;
			pts[2].x = cw + cos(angB) * sizeB;
			pts[2].y = ch + sin(angB) * sizeB;

			pts[3].x = cw + cos(angA) * sizeA;
			pts[3].y = ch + sin(angA) * sizeA;
			pts[4].x = cw + cos(angB) * sizeB;
			pts[4].y = ch + sin(angB) * sizeB;
			pts[5].x = cw + cos(angB) * sizeA;
			pts[5].y = ch + sin(angB) * sizeA;

			CNFGColor( d->history[spoke*d->leds + i] );
			CNFGTackPoly( pts, 3 );
			CNFGTackPoly( pts+3, 3 );
		}
	}

	d->index = (d->index+1) % d->spokes;


	CNFGColor( 0xffffff );
	// CNFGColor( 0x000000 );
}

static void DROParams(void * id )
{
	struct DRODriver * d = (struct DRODriver*)id;

	d->index = 0;
	d->spokes = 9;		RegisterValue(  "spokes", PAINT, &d->spokes, sizeof( d->spokes ) );
	d->leds = 9;		RegisterValue(  "leds", PAINT, &d->leds, sizeof( d->leds ) );
	// d->pie_min = .18;	RegisterValue(  "pie_min", PAFLOAT, &d->pie_min, sizeof( d->pie_min ) );
	// d->pie_max = .3;	RegisterValue(  "pie_max", PAFLOAT, &d->pie_max, sizeof( d->pie_max ) );
}

static struct DriverInstances * DisplayRadar(const char * parameters)
{
	struct DriverInstances * ret = malloc( sizeof( struct DriverInstances ) );
	struct DRODriver * d = ret->id = malloc( sizeof( struct DRODriver ) );
	memset( d, 0, sizeof( struct DRODriver ) );
	ret->Func = DROUpdate;
	ret->Params = DROParams;
	DROParams( d );
	d->history = calloc(d->spokes * d->leds, sizeof(int));
	// TODO: setup vertices for GPU


	CNFGBGColor = 0x000000;
	return ret;
}

REGISTER_OUT_DRIVER(DisplayRadar);


