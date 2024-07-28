/// @file   racer.c
/// @brief  Implementation file for a Racer thread execution
/// @author Daniel Tregea

#define _DEFAULT_SOURCE
#include "racer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "display.h"
#include <pthread.h>
#include <unistd.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int max_delay = DEFAULT_DELAY;

/// Print a racer's graphic on the terminal window
/// @param racer Racer to print graphic of
static void print_car( Racer * racer ){
    // Lock to outside threads
    pthread_mutex_lock( &mutex );
    set_cur_pos( racer->row, racer->dist - 1 );
    // Clear character before car if not at start
    if ( racer->dist - 1 != -1 )
        put( ' ' );
    else // Give valid column if tire flat on first turn
        set_cur_pos( racer->row, 0 );
    
    for ( int i = 0; i < MAX_CAR_LEN; i++ )
        put( racer->graphic[i] );
    
    // Open to outside threads
    pthread_mutex_unlock( &mutex );
}

/// Initialize data common to all racers
/// @param milliseconds Millisecond delay for racers after moving
void init_racers( long milliseconds ){
    max_delay = milliseconds;
}

/// Construct a Racer
/// @param name Name of the racer
/// @param position Row of the racer
/// @return Pointer to newly created Racer
Racer * make_racer( char * name, int position ){
    Racer * new_racer = ( Racer* ) malloc( sizeof( Racer ) );
    new_racer->dist = 0;
    new_racer->row = position;
    
    new_racer->graphic = malloc( MAX_CAR_LEN + 1 );
    strcpy( new_racer->graphic, "~O=" );
    strcat( new_racer->graphic, name );
    int num_hyphens = MAX_NAME_LEN - strlen( name ) + 1;
    for ( int i = 0; i < num_hyphens; i++ )
        strcat( new_racer->graphic, "-" );
    
    strcat( new_racer->graphic,"o>" );
    return new_racer;
}

/// Deconstruct a Racer and free memory associated with it
/// @param racer Racer to deconstruct
void destroy_racer( Racer * racer ){
    free( racer->graphic );
    free( racer );
}

/// Execute a Racer's racing sequence
/// @param racer Racer to start racing
/// @return NULL
void * run ( void * racer ){
    Racer * this_racer = ( Racer * ) racer;
    int racing = 1;
    while( racing ){
        int sleep = rand(  ) % max_delay;
        if( sleep <= 3 ){ // Flat tire
            this_racer->graphic[1] = 'X';
            racing = 0;
        } else { // Sleep and increase distance
            usleep( sleep * 1000 );
            if( ++this_racer->dist == FINISH_LINE )
                racing = 0;
        }
        print_car( this_racer );
    }
    return NULL;
}


