/// @file   pt-cruisers.c
/// @brief  Program which initializes a set of threads to race
///         a terminal window
/// @author Daniel Tregea

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include "racer.h"
#include <string.h>
#include "display.h"
#include <pthread.h>

/// Determine if the command line contains only valid racer names
/// @param delay_override Whether the user entered a speed delay value
/// @param argv Command line arguments
/// @param argc Number of command line arguments
/// @return 0 or 1 on whether command line contains only valid racer names
int valid_racers( int delay_override, char * argv[], int argc ){
    if( delay_override && argc < 4 ) // Number of racer < 2
        return -1;
    int start = delay_override ? 2 : 1;
    char * endptr = NULL;
    for ( int i = start; i < argc; i++ ){
        strtol( argv[i], &endptr, 10 );
        if ( endptr != argv[i] ) // Racer name not a string
            return 0;
        else if ( strlen( argv[i] ) > MAX_NAME_LEN ) // Name > 6 characters
            return -2;
    }
    return 1;
}

/// Main function to initialize and execute racer threads
/// @param argv Command line arguments
/// @param argc Number of command line arguments
/// @return 0 on program success, 1 on program failure
int main( int argc, char * argv[] ){
    if ( argc < 3 ){
        fprintf( stderr, "Usage: pt-cruisers [max-speed-delay]"
                        " name1 name2 [name3...]\n" );
        return EXIT_FAILURE;
    }
    
    srand( time( NULL ) ); // Set seed
    
    // Determine maximum delay from command line
    long max_delay = DEFAULT_DELAY;
    char * endptr = NULL;
    long user_delay = strtol( argv[1], &endptr, 10 );
    int delay_override = 0;
    if ( endptr != argv[1] && user_delay > 0 ){ // User didnt input a new delay
      max_delay = user_delay;
      delay_override = 1;
    }
    
    // Validate racer names
    init_racers( max_delay );
    int valid_names = valid_racers( delay_override, argv, argc );
    if ( valid_names <= 0 ){
        if ( valid_names >= -1 ) 
            fprintf( stderr, "Usage: pt-cruisers [max-speed-delay] "
                            "name1 name2 [name3...]\n" );
        else if ( valid_names == -2 ) 
            fprintf( stderr, "Error: racer names must not exceed"
                          " length %d.\n", MAX_NAME_LEN );
        return EXIT_FAILURE;
    }
    
    // Create racers
    int num_racers;
    int first_racer_index = delay_override ? 2 : 1;
    num_racers = argc - first_racer_index;
    Racer ** racers = malloc( sizeof( Racer* ) * num_racers );
    for ( int i = 0; i < num_racers; i++ )
        racers[i] = make_racer( argv[first_racer_index + i], i + 1 );
    
    // Clear screen
    clear();
    
    // Print finish line
    for ( int i = 1; i <= num_racers; i++ ){
        set_cur_pos( i, FINISH_LINE );
        put( '|' );
    }
    
    // Start racer threads
    void * thread_return;
    pthread_t * race_threads = ( pthread_t * ) malloc( num_racers *
                                               sizeof( pthread_t ) );
    
    for ( int i = 0; i  < num_racers; i++ ){
        int err_num = pthread_create( &race_threads[i], NULL, run,
                                    ( void* ) racers[i] );
        if ( err_num ) 
            fprintf( stderr, "pthread_create failed for racer %d", i );
    }
    
    // Join racers to main thread
    for ( int i = 0; i < num_racers; i++ )
        pthread_join( race_threads[i], &thread_return );
    
    // Set cursor below all racers
    set_cur_pos( racers[num_racers - 1]->row + 1, 1 );
    
    // Free memory
    for ( int i = 0; i < num_racers; i++ )
        destroy_racer( racers[i] );
    free( racers );
    free( race_threads );
    
    return EXIT_SUCCESS;
}
