/* 
 * File:   SessionOrganizer.cpp
 * Author: Kapil Thakkar
 * 
 */

#include "SessionOrganizer.h"
#include "Util.h"
#include <algorithm>

SessionOrganizer::SessionOrganizer ( )
{
    parallelTracks = 0;
    papersInSession = 0;
    sessionsInTrack = 0;
    processingTimeInMinutes = 0;
    tradeoffCoefficient = 1.0;
}

SessionOrganizer::SessionOrganizer ( string filename )
{
    readInInputFile ( filename );
    conference = new Conference ( parallelTracks, sessionsInTrack, papersInSession );
    optimal = new Conference ( parallelTracks, sessionsInTrack, papersInSession );
}


double getProbability(double diff, int t)
{
    double temp = 0.00001/(pow(1.01,t));
    return exp(diff/temp);
}

void SessionOrganizer::updateOptimal()
{
    Session temp;
    for ( int i = 0; i < conference->getSessionsInTrack ( ); i++ )
    {
        for ( int j = 0; j < conference->getParallelTracks ( ); j++ )
        {
            temp = conference->getTrack(j).getSession(i);
            for ( int k = 0; k < conference->getPapersInSession ( ); k++ )
            {
                // papers.push_back(paperCounter);
                optimal->setPaper ( j, i, k, temp.getPaper(k));
            }
        }
    }
}

void SessionOrganizer::shufflePapers()
{
    random_shuffle(papers.begin(),papers.end());
    int paperCounter = 0;
    // std::vector<int> papers;
    for ( int i = 0; i < conference->getSessionsInTrack ( ); i++ )
    {
        for ( int j = 0; j < conference->getParallelTracks ( ); j++ )
        {
            for ( int k = 0; k < conference->getPapersInSession ( ); k++ )
            {
                // papers.push_back(paperCounter);
                conference->setPaper ( j, i, k, papers.at(paperCounter) );
                paperCounter++;
            }
        }
    }
}

void SessionOrganizer::organizePapers ( )
{
    int paperCounter = 0;
    // std::vector<int> papers;
    for ( int i = 0; i < conference->getSessionsInTrack ( ); i++ )
    {
        for ( int j = 0; j < conference->getParallelTracks ( ); j++ )
        {
            for ( int k = 0; k < conference->getPapersInSession ( ); k++ )
            {
                papers.push_back(paperCounter);
                // conference->setPaper ( j, i, k, papers[paperCounter] );
                paperCounter++;
            }
        }
    }
    shufflePapers();
    updateOptimal();

    int k = papersInSession;
    // int tp = parallelTracks*sessionsInTrack;
    int t = parallelTracks;
    int s = sessionsInTrack;
    double score = scoreOrganization();

    srand(time(NULL));
    clock_t start = clock();
    long count = 0;
    int pos = 0, neg = 0;
    // int tolerance = 0;
    while(true)
    {
        count++;
        double max_del = 0.0;
        int t1o,t2o,s1o,s2o,p1o,p2o;

        for (int i =0;i<10;i++)
        {
            int p1 = rand() % k;
            int p2 = rand() % k;
            int s1 = rand() % s;
            int s2 = rand() % s;
            int t1 = rand() % t;
            int t2 = rand() % t;

            double del = scoreChange(t1,s1,p1,t2,s2,p2);
            if (del > max_del)
            {
                t1o = t1;
                t2o = t2;
                s1o = s1;
                s2o = s2;
                p1o = p1;
                p2o = p2;
                max_del = del;
            }
        }
        if(max_del > 0)
        {
            int ptmp1 = conference->getTrack(t1o).getSession(s1o).getPaper(p1o);
            int ptmp2 = conference->getTrack(t2o).getSession(s2o).getPaper(p2o);
            conference->setPaper(t1o,s1o,p1o,ptmp2);
            conference->setPaper(t2o,s2o,p2o,ptmp1);
            score = score + max_del;
            pos++;
        }
        else
        {
            // tolerance++;
            neg++;
        }

        if(neg > 20*pos)
        {
            pos =0.0;
            neg = 0.0;
            cout << "Random Restarting at iteration : " << count << "\n";
            if(scoreOrganization() > scoreOptimal())
                updateOptimal();
            shufflePapers();
            score = scoreOrganization();
            cout << "Current Optimal: " << scoreOptimal() << "\n";
            cout << "New Starting Score: " << score << "\n";
        }
        
        if(double(clock() - start)/CLOCKS_PER_SEC > 60)
        {
            cout << scoreOptimal(); 
            if(scoreOrganization() > scoreOptimal())
                updateOptimal();
            break;
        }
    }
    cout << "Iterations: " << count << "\n";
}


void SessionOrganizer::readInInputFile ( string filename )
{
    vector<string> lines;
    string line;
    ifstream myfile ( filename.c_str () );
    if ( myfile.is_open ( ) )
    {
        while ( getline ( myfile, line ) )
        {
            lines.push_back ( line );
        }
        myfile.close ( );
    }
    else
    {
        cout << "Unable to open input file";
        exit ( 0 );
    }

    if ( 6 > lines.size ( ) )
    {
        cout << "Not enough information given, check format of input file";
        exit ( 0 );
    }

    processingTimeInMinutes = atof ( lines[0].c_str () );
    papersInSession = atoi ( lines[1].c_str () );
    parallelTracks = atoi ( lines[2].c_str () );
    sessionsInTrack = atoi ( lines[3].c_str () );
    tradeoffCoefficient = atof ( lines[4].c_str () );

    int n = lines.size ( ) - 5;
    double ** tempDistanceMatrix = new double*[n];
    for ( int i = 0; i < n; ++i )
    {
        tempDistanceMatrix[i] = new double[n];
    }


    for ( int i = 0; i < n; i++ )
    {
        string tempLine = lines[ i + 5 ];
        string elements[n];
        splitString ( tempLine, " ", elements, n );

        for ( int j = 0; j < n; j++ )
        {
            tempDistanceMatrix[i][j] = atof ( elements[j].c_str () );
        }
    }
    distanceMatrix = tempDistanceMatrix;

    int numberOfPapers = n;
    int slots = parallelTracks * papersInSession*sessionsInTrack;
    if ( slots != numberOfPapers )
    {
        cout << "More papers than slots available! slots:" << slots << " num papers:" << numberOfPapers << endl;
        exit ( 0 );
    }
}

double** SessionOrganizer::getDistanceMatrix ( )
{
    return distanceMatrix;
}

void SessionOrganizer::printSessionOrganiser ( char * filename)
{
    conference->printConference ( filename);
}

double SessionOrganizer :: scoreChange (int t1, int s1, int p1, int t2, int s2, int p2 )
{  
    Track track1 = conference->getTrack(t1);
    Track track2 = conference->getTrack(t2);
    Session sess1 = track1.getSession(s1);
    Session sess2 = track2.getSession(s2);
    int pap1 = conference->getTrack(t1).getSession(s1).getPaper(p1);
    int pap2 = conference->getTrack(t2).getSession(s2).getPaper(p2);
    double s1i = 0.0, s2i = 0.0, s1n = 0.0, s2n = 0.0, d1i = 0.0, d2i = 0.0, d1n = 0.0, d2n = 0.0;

    //s1i
    for(int i = 0; i < sess1.getNumberOfPapers();i++)
    {
        int currPap = sess1.getPaper(i);
        if(i != p1)
            s1i += 1 - distanceMatrix[pap1][currPap];
    }
    //s2i
    for(int i = 0; i < sess2.getNumberOfPapers();i++)
    {
        int currPap = sess2.getPaper(i);
        if(i != p2)
            s2i += 1 - distanceMatrix[pap2][currPap];
    }
    //d1i
    for(int i = 0;i<conference->getParallelTracks();i++)
    {
        if(i != t1)
        {
            Track tmpTrack = conference->getTrack(i);
            Session tmpSess = tmpTrack.getSession(s1);
            for(int j =0;j<tmpSess.getNumberOfPapers();j++)
            {
                int currPap = tmpSess.getPaper(j);
                d1i += distanceMatrix[pap1][currPap];
            }
        }
    }
    //d2i
    for(int i = 0;i<conference->getParallelTracks();i++)
    {
        if(i != t2)
        {
            Track tmpTrack = conference->getTrack(i);
            Session tmpSess = tmpTrack.getSession(s2);
            for(int j =0;j<tmpSess.getNumberOfPapers();j++)
            {
                int currPap = tmpSess.getPaper(j);
                d2i += distanceMatrix[currPap][pap2];
            }
        }
    }

    conference->setPaper(t1,s1,p1,pap2);
    conference->setPaper(t2,s2,p2,pap1);

    track1 = conference->getTrack(t1);
    track2 = conference->getTrack(t2);
    sess1 = track1.getSession(s1);
    sess2 = track2.getSession(s2);
    pap1 = conference->getTrack(t1).getSession(s1).getPaper(p1);
    pap2 = conference->getTrack(t2).getSession(s2).getPaper(p2);

    //s1n
    for(int i = 0; i < sess1.getNumberOfPapers();i++)
    {
        int currPap = sess1.getPaper(i);
        if(i != p1)
            s1n += 1 - distanceMatrix[pap1][currPap];
    }
    //s2n
    for(int i = 0; i < sess2.getNumberOfPapers();i++)
    {
        int currPap = sess2.getPaper(i);
        if(i != p2)
            s2n += 1 - distanceMatrix[pap2][currPap];
    }
    //d1n
    for(int i = 0;i<conference->getParallelTracks();i++)
    {
        if(i != t1)
        {
            Track tmpTrack = conference->getTrack(i);
            Session tmpSess = tmpTrack.getSession(s1);
            for(int j =0;j<tmpSess.getNumberOfPapers();j++)
            {
                int currPap = tmpSess.getPaper(j);
                d1n += distanceMatrix[pap1][currPap];
            }
        }
    }
    //d2n
    for(int i = 0;i<conference->getParallelTracks();i++)
    {
        if(i != t2)
        {
            Track tmpTrack = conference->getTrack(i);
            Session tmpSess = tmpTrack.getSession(s2);
            for(int j =0;j<tmpSess.getNumberOfPapers();j++)
            {
                int currPap = tmpSess.getPaper(j);
                d2n += distanceMatrix[currPap][pap2];
            }
        }
    }

    conference->setPaper(t1,s1,p1,pap2);
    conference->setPaper(t2,s2,p2,pap1);
    
    double delta = (s1n + s2n - s1i - s2i) + tradeoffCoefficient*(d1n + d2n - d1i - d2i);
    return delta;
}


double SessionOrganizer::scoreOrganization ( )
{
    // Sum of pairwise similarities per session.
    double score1 = 0.0;
    for ( int i = 0; i < conference->getParallelTracks ( ); i++ )
    {
        Track tmpTrack = conference->getTrack ( i );
        for ( int j = 0; j < tmpTrack.getNumberOfSessions ( ); j++ )
        {
            Session tmpSession = tmpTrack.getSession ( j );
            for ( int k = 0; k < tmpSession.getNumberOfPapers ( ); k++ )
            {
                int index1 = tmpSession.getPaper ( k );
                for ( int l = k + 1; l < tmpSession.getNumberOfPapers ( ); l++ )
                {
                    int index2 = tmpSession.getPaper ( l );
                    score1 += 1 - distanceMatrix[index1][index2];
                }
            }
        }
    }

    // Sum of distances for competing papers.
    double score2 = 0.0;
    for ( int i = 0; i < conference->getParallelTracks ( ); i++ )
    {
        Track tmpTrack1 = conference->getTrack ( i );
        for ( int j = 0; j < tmpTrack1.getNumberOfSessions ( ); j++ )
        {
            Session tmpSession1 = tmpTrack1.getSession ( j );
            for ( int k = 0; k < tmpSession1.getNumberOfPapers ( ); k++ )
            {
                int index1 = tmpSession1.getPaper ( k );

                // Get competing papers.
                for ( int l = i + 1; l < conference->getParallelTracks ( ); l++ )
                {
                    Track tmpTrack2 = conference->getTrack ( l );
                    Session tmpSession2 = tmpTrack2.getSession ( j );
                    for ( int m = 0; m < tmpSession2.getNumberOfPapers ( ); m++ )
                    {
                        int index2 = tmpSession2.getPaper ( m );
                        score2 += distanceMatrix[index1][index2];
                    }
                }
            }
        }
    }
    double score = score1 + tradeoffCoefficient*score2;
    return score;
}

double SessionOrganizer::scoreOptimal ( )
{
    // Sum of pairwise similarities per session.
    double score1 = 0.0;
    for ( int i = 0; i < optimal->getParallelTracks ( ); i++ )
    {
        Track tmpTrack = optimal->getTrack ( i );
        for ( int j = 0; j < tmpTrack.getNumberOfSessions ( ); j++ )
        {
            Session tmpSession = tmpTrack.getSession ( j );
            for ( int k = 0; k < tmpSession.getNumberOfPapers ( ); k++ )
            {
                int index1 = tmpSession.getPaper ( k );
                for ( int l = k + 1; l < tmpSession.getNumberOfPapers ( ); l++ )
                {
                    int index2 = tmpSession.getPaper ( l );
                    score1 += 1 - distanceMatrix[index1][index2];
                }
            }
        }
    }

    // Sum of distances for competing papers.
    double score2 = 0.0;
    for ( int i = 0; i < optimal->getParallelTracks ( ); i++ )
    {
        Track tmpTrack1 = optimal->getTrack ( i );
        for ( int j = 0; j < tmpTrack1.getNumberOfSessions ( ); j++ )
        {
            Session tmpSession1 = tmpTrack1.getSession ( j );
            for ( int k = 0; k < tmpSession1.getNumberOfPapers ( ); k++ )
            {
                int index1 = tmpSession1.getPaper ( k );

                // Get competing papers.
                for ( int l = i + 1; l < optimal->getParallelTracks ( ); l++ )
                {
                    Track tmpTrack2 = optimal->getTrack ( l );
                    Session tmpSession2 = tmpTrack2.getSession ( j );
                    for ( int m = 0; m < tmpSession2.getNumberOfPapers ( ); m++ )
                    {
                        int index2 = tmpSession2.getPaper ( m );
                        score2 += distanceMatrix[index1][index2];
                    }
                }
            }
        }
    }
    double score = score1 + tradeoffCoefficient*score2;
    return score;
}
