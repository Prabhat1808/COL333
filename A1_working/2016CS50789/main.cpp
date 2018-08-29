
#include <cstdlib>

#include "SessionOrganizer.h"
#include <time.h>

using namespace std;

/*
 * 
 */
int main ( int argc, char** argv )
{
    clock_t start = clock();
    // Parse the input.
    if ( argc < 3 )
    {   
        cout<<"Missing arguments\n";
        cout<<"Correct format : \n";
        cout << "./main <input_filename> <output_filename>";
        exit ( 0 );
    }
    string inputfilename ( argv[1] );
    
    // Initialize the conference organizer.
    SessionOrganizer *organizer  = new SessionOrganizer( inputfilename );

    // Organize the papers into tracks based on similarity.
    organizer->organizePapers ((float)start);

    organizer->printSessionOrganiser ( argv[2]);

    // Score the organization against the gold standard.
    // double score = organizer->scoreOptimal ( );
    // cout<< "score:"<<score<<"\n";

    return 0;
}

