/* 
 * File:   SessionOrganizer.h
 * Author: Kapil Thakkar
 *
 */

#ifndef SESSIONORGANIZER_H
#define	SESSIONORGANIZER_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <time.h>


#include "Conference.h"
#include "Track.h"
#include "Session.h"

using namespace std;


/**
 * SessionOrganizer reads in a similarity matrix of papers, and organizes them
 * into sessions and tracks.
 * 
 * @author Kapil Thakkar
 *
 */
class SessionOrganizer {
private:
    double ** distanceMatrix;

    int parallelTracks ;
    int papersInSession ;
    int sessionsInTrack ;

    Conference *conference;
    Conference *optimal;
    std::vector<int> papers;

    double processingTimeInMinutes ;
    double tradeoffCoefficient ; // the tradeoff coefficient


public:
    SessionOrganizer();
    SessionOrganizer(string filename);
    
    double scoreChange (int t1, int s1, int p1, int t2, int s2, int p2 );
    /**
     * Read in the number of parallel tracks, papers in session, sessions
     * in a track, and the similarity matrix from the specified filename.
     * @param filename is the name of the file containing the matrix.
     * @return the similarity matrix.
     */
    void readInInputFile(string filename);

    void shufflePapers();   
    
    /**
     * Organize the papers according to some algorithm.
     */
    void organizePapers();
    
    
    /**
     * Get the distance matrix.
     * @return the distance matrix.
     */
    double** getDistanceMatrix();
    
    
    /**
     * Score the organization.
     * @return the score.
     */
    double scoreOrganization();
    
    
    void printSessionOrganiser(char *);
};

#endif	/* SESSIONORGANIZER_H */

