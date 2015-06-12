#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <math.h>
using namespace std;


int main(int argc, char* argv[])
{
    ifstream input(argv[1]);
    if (!input.is_open()) {
        cerr << "File not valid.";
        return 0;
    }

    ofstream output(argv[2]);
    if (!output.is_open()) {
        cerr << "File not valid";
        return 0;
    }
    int address;
    string line, instr;
    int totalNum = 0;

    /** 
     * Set Associative Section
     * http://www.cs.umd.edu/class/sum2003/cmsc311/Notes/Memory/set.html
     * http://www.csbio.unc.edu/mcmillan/index.py?run=Wiki&page=%24Comp411S12.Lab+9
     * A set-associative cache scheme is a combination of fully associative and direct mapped schemes. 
     * You group slots into sets. 
     * You find the appropriate set for a given address (which is like the direct mapped scheme),
     * and within the set you find the appropriate slot (which is like the fully associative scheme).
     */
    //if 2w works, then i can just copy and paste the rest
    //counter for set associative hit rates
    int set2W = 0;
    int set4W = 0;
    int set8W = 0;
    int set16W = 0;
    
    //16KB / 32 / 2,4,8,16 = 256, 128, 64, 32
    //intialize parts of a cache for respective n-ways
    int cacheline2W = 256;
    int cacheline4W = 128;
    int cacheline8W = 64;
    int cacheline16W = 32;
    
    int cacheset2W = 0;
    int cacheset4W = 0;
    int cacheset8W = 0;
    int cacheset16W = 0;
    
    int cachetag2W = 0;
    int cachetag4W = 0;
    int cachetag8W = 0;
    int cachetag16W = 0;
    
    int **cache2W = new int*[256];
    int **cache4W = new int*[128];
    int **cache8W = new int*[64];
    int **cache16W = new int*[32];
    
    int **MRUcache2W = new int*[256];
    int **MRUcache4W = new int*[128];
    int **MRUcache8W = new int*[64];
    int **MRUcache16W = new int*[32];
    for (int i = 0; i < 256; i++)
    {
        MRUcache2W[i] = new int[2];
        cache2W[i] = new int[2];
    }
    for (int i = 0; i < 128; i++)
    {
        MRUcache4W[i] = new int[4];
        cache4W[i] = new int[4];
    }
    for (int i = 0; i < 64; i++)
    {
        MRUcache8W[i] = new int[8];
        cache8W[i] = new int[8];
    }
    for (int i = 0; i < 32; i++)
    {
        MRUcache16W[i] = new int[16];
        cache16W[i] = new int[16];
    }
    for (int i = 0; i < 256; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            MRUcache2W[i][j] = j;
            cache2W[i][j] = -1;
        }
    }
        for (int i = 0; i < 128; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            MRUcache4W[i][j] = j;
            cache4W[i][j] = -1;
        }
    }
        for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            MRUcache8W[i][j] = j;
            cache8W[i][j] = -1;
        }
    }
        for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            MRUcache16W[i][j] = j;
            cache16W[i][j] = -1;
        }
    }
    while(getline(input, line))
    {
        stringstream ss(line);
        ss>>instr>>std::hex>>address;
        totalNum++;
        
        /**
         * 
         * Set Associative section
         */
         //2-Way
        cacheset2W = (address>>5)%cacheline2W;
        cachetag2W = address >> ((int) log2(cacheline2W));
        int way2W = 0;
        bool found2W = false;
        for (int i = 2; i >= 0; i--)
        {
            if (cache2W[cacheset2W][i] == cachetag2W)
            {
                found2W = true;
                way2W = i;
            }
        }
        //if it is found in the cache
        //go backwards
        if (found2W)
        {
            int MRUindex2W = -1;
            //this checks where it is regards to MRU
            for (int i = 1; i >= 0; i--)
            {
                if (MRUcache2W[cacheset2W][i] == way2W)
                {
                    MRUindex2W = i;
                }
            }
            //shift MRU?
            for (int i = MRUindex2W; i >= 0; i--)
            {
                MRUcache2W[cacheset2W][MRUindex2W-i] = MRUcache2W[cacheset2W][(MRUindex2W-1)-i];
            }
            MRUcache2W[cacheset2W][0] = way2W;
            set2W++;
        }
        //if it's not in the cache, we update the spot with the tag
        else
        {
            int miss = MRUcache2W[cacheset2W][2-1];
            for (int i = 2; i > 0; i--)
            {
                MRUcache2W[cacheset2W][2-i] = MRUcache2W[cacheset2W][(2 - 1)+i];
            }
            MRUcache2W[cacheset2W][0] = miss;
            cache2W[cacheset2W][MRUcache2W[cacheset2W][0]] = cachetag2W;
        }
        //4-Way
        cacheset4W = (address>>5)%cacheline4W;
        cachetag4W = address >> ((int) log2(cacheline4W));
        int way4W = 0;
        bool found4W = false;
        for (int i = 4; i >= 0; i--)
        {
            if (cache4W[cacheset4W][i] == cachetag4W)
            {
                found4W = true;
                way4W = i;
            }
        }
        //if it is found in the cache
        //go backwards
        if (found4W)
        {
            int MRUindex4W = -1;
            //this checks where it is regards to MRU
            for (int i = 1; i >= 0; i--)
            {
                if (MRUcache4W[cacheset4W][i] == way4W)
                {
                    MRUindex4W = i;
                }
            }
            //shift MRU?
            for (int i = MRUindex4W; i >= 0; i--)
            {
                MRUcache4W[cacheset4W][MRUindex4W-i] = MRUcache4W[cacheset4W][(MRUindex4W-1)-i];
            }
            MRUcache4W[cacheset4W][0] = way4W;
            set4W++;
        }
        //if it's not in the cache, we update the spot with the tag
        else
        {
            int miss = MRUcache4W[cacheset4W][4-1];
            for (int i = 4; i > 0; i--)
            {
                MRUcache4W[cacheset4W][4-i] = MRUcache4W[cacheset4W][(4 - 1)+i];
            }
            MRUcache4W[cacheset4W][0] = miss;
            cache4W[cacheset4W][MRUcache4W[cacheset4W][0]] = cachetag4W;
        }
        //8-Way
        cacheset8W = (address>>5)%cacheline8W;
        cachetag8W = address >> ((int) log2(cacheline8W));
        int way8W = 0;
        bool found8W = false;
        for (int i = 8; i >= 0; i--)
        {
            if (cache8W[cacheset8W][i] == cachetag8W)
            {
                found8W = true;
                way8W = i;
            }
        }
        //if it is found in the cache
        //go backwards
        if (found8W)
        {
            int MRUindex8W = -1;
            //this checks where it is regards to MRU
            for (int i = 1; i >= 0; i--)
            {
                if (MRUcache8W[cacheset8W][i] == way8W)
                {
                    MRUindex8W = i;
                }
            }
            //shift MRU?
            for (int i = MRUindex8W; i >= 0; i--)
            {
                MRUcache8W[cacheset8W][MRUindex8W-i] = MRUcache8W[cacheset8W][(MRUindex8W-1)-i];
            }
            MRUcache8W[cacheset8W][0] = way8W;
            set8W++;
        }
        //if it's not in the cache, we update the spot with the tag
        else
        {
            int miss = MRUcache8W[cacheset8W][8-1];
            for (int i = 8; i > 0; i--)
            {
                MRUcache8W[cacheset8W][8-i] = MRUcache8W[cacheset8W][(8 - 1)+i];
            }
            MRUcache8W[cacheset8W][0] = miss;
            cache8W[cacheset8W][MRUcache8W[cacheset8W][0]] = cachetag8W;
        }
        //16-Way
        cacheset16W = (address>>5)%cacheline16W;
        cachetag16W = address >> ((int) log2(cacheline16W));
        int way16W = 0;
        bool found16W = false;
        for (int i = 16; i >= 0; i--)
        {
            if (cache16W[cacheset16W][i] == cachetag16W)
            {
                found16W = true;
                way16W = i;
            }
        }
        //if it is found in the cache
        //go backwards
        if (found16W)
        {
            int MRUindex16W = -1;
            //this checks where it is regards to MRU
            for (int i = 1; i >= 0; i--)
            {
                if (MRUcache16W[cacheset16W][i] == way16W)
                {
                    MRUindex16W = i;
                }
            }
            //shift MRU?
            for (int i = MRUindex16W; i >= 0; i--)
            {
                MRUcache16W[cacheset16W][MRUindex16W-i] = MRUcache16W[cacheset16W][(MRUindex16W-1)-i];
            }
            MRUcache16W[cacheset16W][0] = way16W;
            set16W++;
        }
        //if it's not in the cache, we update the spot with the tag
        else
        {
            int miss = MRUcache16W[cacheset16W][16-1];
            for (int i = 16; i > 0; i--)
            {
                MRUcache16W[cacheset16W][16-i] = MRUcache16W[cacheset16W][(16 - 1)+i];
            }
            MRUcache16W[cacheset16W][0] = miss;
            cache16W[cacheset16W][MRUcache16W[cacheset16W][0]] = cachetag16W;
        }
    }
    cout << set2W << endl;
    cout << set4W << endl;
    cout << set8W << endl;
    cout << set16W << endl;
    output << set2W << "," << totalNum << "; " << set4W << "," << totalNum << "; " << 
    set8W << "," << totalNum << "; " << set16W << "," << totalNum << ";" << endl;
    output.close();
}