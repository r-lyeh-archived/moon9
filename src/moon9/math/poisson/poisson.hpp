/*
 * Copyright (c) Bo Zhou<bo.schwarzstein@gmail.com>
 * An 2D implementation of "Fast Poisson Disk Sampling in Arbitrary Dimensions, R. Bridson, ACM SIGGRAPH 2007 Sketches Program".
 * Anybody could use this code freely, if you feel it good or helpful, please tell me, thank you very much.
 * 
 * Removed boost dependency (rlyeh)
 */
 
#pragma once

#include <cmath>
#include <cstdlib>
#include <vector>

namespace moon9
{
    template<typename T>
    float poisson(const float r, std::vector< std::pair<T,T> >& PointList)
    {
        // Define our basis data structure.
        typedef std::pair<int,int> Index2D;
        typedef std::pair<T,T> Point2D;

        struct detail
        {
            // Use C standard rand() to get uniform random number in (0,1)
            static inline float URand()
            {
                return (float)std::rand() / (float)(RAND_MAX+1);
            }

            // Use C standard rand() to get uniform random number in (0,1]
            static inline float URandClosed()
            {
                return (float)std::rand() / (float)(RAND_MAX);
            }

            // Calculate distance between 2 points.
            static inline float LengthSqrt(const Point2D& X, const Point2D& Y)
            {
                float a = X.first - Y.first;
                float b = X.second - Y.second;
                return std::sqrtf( a*a + b*b );
            }        
        };

        PointList.clear();
        
        const int N = 2;
        const float SqrtN = std::sqrtf((float)N);
        const float R = r*SqrtN; // This R should be used to draw circle.
        const float RDivSqrtN = R / SqrtN;
        const int K = 30; // Paper suggested.
        const float CellSize = (float)N*RDivSqrtN;
        const int GridSize = (int)std::ceilf(1.0f/CellSize);
        const int INVALID_IDX = -1;
        
        // Allocate a grid to accelerate.
        // Each cell could have one and only one sampler.
        std::vector< std::vector<int> > Grid(GridSize);
        for(int i = 0; i < GridSize; ++i ) Grid[i] = std::vector<int>( GridSize, INVALID_IDX );
        
        std::vector<int> ActiveList;
        
        Point2D P0;
        Index2D I0;
        
        P0.first = detail::URand();
        P0.second = detail::URand();
        I0.first = (int)std::floorf( P0.first*GridSize );
        I0.second = (int)std::floorf( P0.second*GridSize );
        
        ActiveList.push_back(0);
        PointList.push_back(P0);
        Grid[I0.first][I0.second] = 0;
        
        int Done = 1;
        while( ActiveList.size() != 0 )
        {
            // Initialize a sampler.
            float ix = detail::URand();
            size_t sz = ActiveList.size();
            size_t MagicIdx = (size_t)std::floorf( ix * sz );
            size_t StartIdx = ActiveList[MagicIdx];
            Point2D StartPoint = PointList[ StartIdx ];

            bool Found = false;

            for( size_t i=0; i<K; ++i )
            {
                // Generate point in radius (R,2R)
                float t = detail::URand()*3.1415926535897932384626433832795*2.0;
                float r = detail::URand()*R+R;
                float X = r*std::cosf( t );
                float Y = r*std::sinf( t );

                // Move to current center
                Point2D CurrentPoint;
                CurrentPoint.first = X+StartPoint.first;  
                CurrentPoint.second = Y+StartPoint.second;

                // Discard if out of domain
                if( CurrentPoint.first < 0.0f || CurrentPoint.first >= 1.0f )
                    continue;

                if( CurrentPoint.second < 0.0f || CurrentPoint.second >= 1.0f )
                    continue;

                // Which cell the test point located in
                Index2D TargetCell;
                TargetCell.first = (int)std::floorf( CurrentPoint.first / CellSize );
                TargetCell.second = (int)std::floorf( CurrentPoint.second / CellSize );

                if( TargetCell.first < 0 || TargetCell.first >= GridSize )
                    continue;

                if( TargetCell.second < 0 || TargetCell.second >= GridSize )
                    continue;


                if( Grid[TargetCell.first][TargetCell.second] != INVALID_IDX )
                    continue;

                Index2D TCLeft( TargetCell );
                Index2D TCRight( TargetCell );
                Index2D TCDown( TargetCell );
                Index2D TCUp( TargetCell );

                Index2D TCLU( TargetCell );
                Index2D TCRU( TargetCell );
                Index2D TCLD( TargetCell );
                Index2D TCRD( TargetCell );

                bool A = false, B = false, C = false, D = false;
                bool E = false, F = false, G = false, H = false;

                TCLeft.first--;
                if( TCLeft.first > -1 )
                {
                    int Idx2 = Grid[TCLeft.first][TCLeft.second];
                    if( Idx2 >= 0 )
                    {
                        if( detail::LengthSqrt( PointList[Idx2],CurrentPoint ) > R )
                        {
                            A = true;
                        }
                    }else
                    {
                        A = true;
                    }
                }else
                {
                    A = true;
                }

                TCRight.first++;
                if( TCRight.first < GridSize )
                {
                    int Idx2 = Grid[TCRight.first][TCRight.second];
                    if( Idx2 >= 0 )
                    {
                        if( detail::LengthSqrt( PointList[Idx2],CurrentPoint ) > R )
                        {
                            B = true;
                        }
                    }else
                    {
                        B = true;
                    }
                }else
                {
                    B = true;
                }

                TCDown.second--;
                if( TCDown.second > -1 )
                {
                    int Idx2 = Grid[TCDown.first][TCDown.second];
                    if( Idx2 >= 0 )
                    {
                        if( detail::LengthSqrt( PointList[Idx2],CurrentPoint ) > R )
                        {
                            C = true;
                        }
                    }else
                    {
                        C = true;
                    }
                }else
                {
                    C = true;
                }

                TCUp.second++;
                if( TCUp.second < GridSize )
                {
                    int Idx2 = Grid[TCUp.first][TCUp.second];
                    if( Idx2 >= 0 )
                    {
                        if( detail::LengthSqrt( PointList[Idx2],CurrentPoint ) > R )
                        {
                            D = true;
                        }
                    }else
                    {
                        D = true;
                    }
                }else
                {
                    D = true;
                }

                // 4 Corner
                // Left Up
                TCLU.first--;TCLU.second++;
                if( TCLU.first > -1 && TCLU.second < GridSize )
                {

                    int Idx2 = Grid[TCLU.first][TCLU.second];
                    
                    if( Idx2 >= 0 )
                    {
                        if( detail::LengthSqrt( PointList[Idx2],CurrentPoint ) > R )
                        {
                            E = true;
                        }
                    }else
                    {
                        E = true;
                    }
                }else
                {
                    E = true;
                }

                // Right Up
                TCRU.first++;TCRU.second++;
                if( TCRU.first < GridSize && TCRU.second < GridSize )
                {

                    int Idx2 = Grid[TCRU.first][TCRU.second];
                    
                    if( Idx2 >= 0 )
                    {
                        if( detail::LengthSqrt( PointList[Idx2],CurrentPoint ) > R )
                        {
                            F = true;
                        }
                    }else
                    {
                        F = true;
                    }
                }else
                {
                    F = true;
                }

                // Left Bottom
                TCLD.first--;TCLD.second--;
                if( TCLD.first > -1 && TCLD.second > -1 )
                {
                    int Idx2 = Grid[TCLD.first][TCLD.second];
                    if( Idx2 >= 0 )
                    {
                        if( detail::LengthSqrt( PointList[Idx2],CurrentPoint ) > R )
                        {
                            G = true;
                        }
                    }else
                    {
                        G = true;
                    }
                }else
                {
                    G = true;
                }

                // Right Bottom
                TCRD.first++;TCRD.second--;
                if( TCRD.first < GridSize && TCRD.second > -1 )
                {
                    int Idx2 = Grid[TCRD.first][TCRD.second];
                    if( Idx2 >= 0 )
                    {
                        if( detail::LengthSqrt( PointList[Idx2],CurrentPoint ) > R )
                        {
                            H = true;
                        }
                    }else
                    {
                        H = true;
                    }
                }else
                {
                    H = true;
                }

                if( A&B&C&D&E&F&G&H )
                {
                    Grid[TargetCell.first][TargetCell.second] = Done;
                    PointList.push_back(CurrentPoint);
                    ActiveList.push_back(Done);            

                    ++Done;
                    Found = true;
                    break;
                }
            }

            // We have to remove this test sampler from active list.
            if( Found == false && ActiveList.size() > 0 )
                ActiveList.erase( ActiveList.begin()+MagicIdx );
        }

        return R;
    }

} //ns
