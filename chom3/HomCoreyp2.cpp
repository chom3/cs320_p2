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
     * Direct Mapped Cache Section 
     * Here is how it works:
     * A read access to the cache takes the middle part of the address that is called index 
     * and use it as the row number. 
     * The data and the tag are looked up at the same time.
     * Next, the tag needs to be compared with the upper part of the address to 
     * decide if the line is from the same address range in memory and is valid. 
     * At the same time, the lower part of the address can be used
     * to select the requested data from cache line
     * http://stackoverflow.com/questions/15937002/how-does-direct-mapped-cache-work
     */
    //counters for direct hit rates
    int direct1KB = 0;
    int direct4KB = 0;
    int direct16KB = 0;
    int direct32KB = 0;
    
    //initialize cache for 1KB
    int cacheline1KB = 32;
    int index1KB = 0;
    int tag1KB = 0;
    int *directCache1KB = new int[32];
    //initialize to 0
    for (int i=0; i<32; i++) {
        directCache1KB[i] = 0;
    }
    //initialize cache for 4KB
    int cacheline4KB = 128;
    int index4KB = 0;
    int tag4KB = 0;
    int *directCache4KB = new int[128];
    //initialize to 0
    for (int i=0; i<128; i++) {
        directCache4KB[i] = 0;
    }
    //initialize cache for 16KB
    int cacheline16KB = 512;
    int index16KB = 0;
    int tag16KB = 0;
    int *directCache16KB = new int[512];
    //initialize to 0
    for (int i=0; i<512; i++) {
        directCache16KB[i] = 0;
    }
    //initialize cache for 32KB
    int cacheline32KB = 1024;
    int index32KB = 0;
    int tag32KB = 0;
    int *directCache32KB = new int[1024];
    //initialize to 0
    for (int i=0; i<1024; i++) {
        directCache32KB[i] = 0;
    }
    
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
    
    int **LRUcache2W = new int*[256];
    int **LRUcache4W = new int*[128];
    int **LRUcache8W = new int*[64];
    int **LRUcache16W = new int*[32];
    for (int i = 0; i < 256; i++)
    {
        LRUcache2W[i] = new int[2];
        cache2W[i] = new int[2];
    }
    for (int i = 0; i < 128; i++)
    {
        LRUcache4W[i] = new int[4];
        cache4W[i] = new int[4];
    }
    for (int i = 0; i < 64; i++)
    {
        LRUcache8W[i] = new int[8];
        cache8W[i] = new int[8];
    }
    for (int i = 0; i < 32; i++)
    {
        LRUcache16W[i] = new int[16];
        cache16W[i] = new int[16];
    }
    for (int i = 0; i < 256; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            LRUcache2W[i][j] = j;
            cache2W[i][j] = -1;
        }
    }
        for (int i = 0; i < 128; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            LRUcache4W[i][j] = j;
            cache4W[i][j] = -1;
        }
    }
        for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            LRUcache8W[i][j] = j;
            cache8W[i][j] = -1;
        }
    }
        for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            LRUcache16W[i][j] = j;
            cache16W[i][j] = -1;
        }
    }
    /**
     * Fully associative cache
     * http://www.cs.umd.edu/class/sum2003/cmsc311/Notes/Memory/fully.html
     * In a fully associative scheme, any slot can store the cache line. 
     * The hardware for finding whether the desired data is in the cache requires comparing
     * the tag bits of the address to the tag bits of every slot (in parallel), 
     * and making sure the valid bit is set. If there is a cache miss, the initial goal 
     * is to pick a slot that's not valid to place the data being searched for. 
     * If there are no valid slots, the hardware must pick a slot to evict based on some eviction policy. If the evicted slot's dirty bit is 1, then the data in the slot must be copied back to RAM. If it's 0, there's no need to copy back, since the data in the cache line should be identical to the data in the corresponding memory locations in RAM.
     */
     
    //Least recently used
    //initialize values much like set-associated cache
    int LRUcount = 0;
    int cachelineLRU = 512;
    int tagLRU = 0;
    int **cacheLRU = new int *[1];
    int **LRUsimFA = new int *[1];
    for (int i = 0; i < 1; i++)
    {
        cacheLRU[i] = new int[512];
        LRUsimFA[i] = new int[512];
    }
    for (int i =0; i < 1; i++)
    {
        for (int j =0; j < 512; j++)
        {
            cacheLRU[i][j] = -1;
            LRUsimFA[i][j] = j;
        }
    }
    
    //Hot cold
    //initialize values much like set-associated cache
    int HCcount = 0;
    int cachelineHC = 512;
    int tagHC = 0;
    int *cacheHC = new int [512];
    int *HCsimFA = new int [512];
    for (int i =0; i < 512; i++)
    {
        HCsimFA[i] = 0;
        cacheHC[i] = -1;
    }

    /** 
     * Set Associative Section with no allocation on a write miss
     * If a store instruction misses into the cache, then the missing line is not written into the cache
     * instead it is written directy to memory.
     * 
     * 
     */

    //counter for set associative hit rates
    int set2NA = 0;
    int set4NA = 0;
    int set8NA = 0;
    int set16NA = 0;
    
    //16KB / 32 / 2,4,8,16 = 256, 128, 64, 32
    //intialize parts of a cache for respective n-ways
    int cacheline2NA = 256;
    int cacheline4NA = 128;
    int cacheline8NA = 64;
    int cacheline16NA = 32;
    
    int cacheset2NA = 0;
    int cacheset4NA = 0;
    int cacheset8NA = 0;
    int cacheset16NA = 0;
    
    int cachetag2NA = 0;
    int cachetag4NA = 0;
    int cachetag8NA = 0;
    int cachetag16NA = 0;
    
    int **cache2NA = new int*[256];
    int **cache4NA = new int*[128];
    int **cache8NA = new int*[64];
    int **cache16NA = new int*[32];
    
    int **LRUcache2NA = new int*[256];
    int **LRUcache4NA = new int*[128];
    int **LRUcache8NA = new int*[64];
    int **LRUcache16NA = new int*[32];
    for (int i = 0; i < 256; i++)
    {
        LRUcache2NA[i] = new int[2];
        cache2NA[i] = new int[2];
    }
    for (int i = 0; i < 128; i++)
    {
        LRUcache4NA[i] = new int[4];
        cache4NA[i] = new int[4];
    }
    for (int i = 0; i < 64; i++)
    {
        LRUcache8NA[i] = new int[8];
        cache8NA[i] = new int[8];
    }
    for (int i = 0; i < 32; i++)
    {
        LRUcache16NA[i] = new int[16];
        cache16NA[i] = new int[16];
    }
    for (int i = 0; i < 256; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            LRUcache2NA[i][j] = j;
            cache2NA[i][j] = -1;
        }
    }
        for (int i = 0; i < 128; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            LRUcache4NA[i][j] = j;
            cache4NA[i][j] = -1;
        }
    }
        for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            LRUcache8NA[i][j] = j;
            cache8NA[i][j] = -1;
        }
    }
        for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            LRUcache16NA[i][j] = j;
            cache16NA[i][j] = -1;
        }
    }
    /** 
     * Set Associative Section with with next line prefetching
     * The next memory line will be brought into the cache with every cache access
     * If current access is to line X, then line (x+1) is also brought into the cache
     * REplacing the cache's previous content
     * PA for prefetch access
     */

    //counter for set associative hit rates
    int set2PA = 0;
    int set4PA = 0;
    int set8PA = 0;
    int set16PA = 0;
    
    //16KB / 32 / 2,4,8,16 = 256, 128, 64, 32
    //intialize parts of a cache for respective n-ways
    int cacheline2PA = 256;
    int cacheline4PA = 128;
    int cacheline8PA = 64;
    int cacheline16PA = 32;
    
    int cacheset2PA = 0;
    int cacheset4PA = 0;
    int cacheset8PA = 0;
    int cacheset16PA = 0;
    
    int cachetag2PA = 0;
    int cachetag4PA = 0;
    int cachetag8PA = 0;
    int cachetag16PA = 0;
    
    int **cache2PA = new int*[256];
    int **cache4PA = new int*[128];
    int **cache8PA = new int*[64];
    int **cache16PA = new int*[32];
    
    int **LRUcache2PA = new int*[256];
    int **LRUcache4PA = new int*[128];
    int **LRUcache8PA = new int*[64];
    int **LRUcache16PA = new int*[32];
    for (int i = 0; i < 256; i++)
    {
        LRUcache2PA[i] = new int[2];
        cache2PA[i] = new int[2];
    }
    for (int i = 0; i < 128; i++)
    {
        LRUcache4PA[i] = new int[4];
        cache4PA[i] = new int[4];
    }
    for (int i = 0; i < 64; i++)
    {
        LRUcache8PA[i] = new int[8];
        cache8PA[i] = new int[8];
    }
    for (int i = 0; i < 32; i++)
    {
        LRUcache16PA[i] = new int[16];
        cache16PA[i] = new int[16];
    }
    for (int i = 0; i < 256; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            LRUcache2PA[i][j] = j;
            cache2PA[i][j] = -1;
        }
    }
        for (int i = 0; i < 128; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            LRUcache4PA[i][j] = j;
            cache4PA[i][j] = -1;
        }
    }
        for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            LRUcache8PA[i][j] = j;
            cache8PA[i][j] = -1;
        }
    }
        for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            LRUcache16PA[i][j] = j;
            cache16PA[i][j] = -1;
        }
    }
    /** 
     * Set Associative Section with prefetching on a miss
     */

    //counter for set associative hit rates
    int set2PM = 0;
    int set4PM = 0;
    int set8PM = 0;
    int set16PM = 0;
    
    //16KB / 32 / 2,4,8,16 = 256, 128, 64, 32
    //intialize PMrts of a cache for respective n-ways
    int cacheline2PM = 256;
    int cacheline4PM = 128;
    int cacheline8PM = 64;
    int cacheline16PM = 32;
    
    int cacheset2PM = 0;
    int cacheset4PM = 0;
    int cacheset8PM = 0;
    int cacheset16PM = 0;
    
    int cachetag2PM = 0;
    int cachetag4PM = 0;
    int cachetag8PM = 0;
    int cachetag16PM = 0;
    
    int **cache2PM = new int*[256];
    int **cache4PM = new int*[128];
    int **cache8PM = new int*[64];
    int **cache16PM = new int*[32];
    
    int **LRUcache2PM = new int*[256];
    int **LRUcache4PM = new int*[128];
    int **LRUcache8PM = new int*[64];
    int **LRUcache16PM = new int*[32];
    for (int i = 0; i < 256; i++)
    {
        LRUcache2PM[i] = new int[2];
        cache2PM[i] = new int[2];
    }
    for (int i = 0; i < 128; i++)
    {
        LRUcache4PM[i] = new int[4];
        cache4PM[i] = new int[4];
    }
    for (int i = 0; i < 64; i++)
    {
        LRUcache8PM[i] = new int[8];
        cache8PM[i] = new int[8];
    }
    for (int i = 0; i < 32; i++)
    {
        LRUcache16PM[i] = new int[16];
        cache16PM[i] = new int[16];
    }
    for (int i = 0; i < 256; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            LRUcache2PM[i][j] = j;
            cache2PM[i][j] = -1;
        }
    }
        for (int i = 0; i < 128; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            LRUcache4PM[i][j] = j;
            cache4PM[i][j] = -1;
        }
    }
        for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            LRUcache8PM[i][j] = j;
            cache8PM[i][j] = -1;
        }
    }
        for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            LRUcache16PM[i][j] = j;
            cache16PM[i][j] = -1;
        }
    }
    /**
     * 
     * 
     * 
     * 
     * CALCULATIONS
     * 
     * 
     * 
     */
    //read memory trace files
    while(getline(input, line))
    {
        stringstream ss(line);
        ss>>instr>>std::hex>>address;
        totalNum++;
        
        /**
         * 
         * Calculate direct mapped section.
         * 
         */
        // 1KB
        index1KB = (address>>5)%cacheline1KB;
        tag1KB = address>>((int)(log2(cacheline1KB))+5);
        // 4KB
        index4KB = (address>>5)%cacheline4KB;
        tag4KB = address>>((int)(log2(cacheline4KB))+5);
        // 16KB
        index16KB = (address>>5)%cacheline16KB;
        tag16KB = address>>((int)(log2(cacheline16KB))+5);
        // 32KB
        index32KB = (address>>5)%cacheline32KB;
        tag32KB = address>>((int)(log2(cacheline32KB))+5);
        

        // direct mapped cache 1KB
        if (directCache1KB[index1KB] == tag1KB)
        {
            direct1KB++;
        }
        else
        {
            directCache1KB[index1KB] = tag1KB;
        }
        
        // direct mapped cache 4KB
        if (directCache4KB[index4KB] == tag4KB)
        {
            direct4KB++;
        }
        else
        {
            directCache4KB[index4KB] = tag4KB;
        }
        
        // direct mapped cache 16KB
        if (directCache16KB[index16KB] == tag16KB)
        {
            direct16KB++;
        }
        else
        {
            directCache16KB[index16KB] = tag16KB;
        }
        
        // direct mapped cache 32KB
        if (directCache32KB[index32KB] == tag32KB)
        {
            direct32KB++;
        }
        else
        {
            directCache32KB[index32KB] = tag32KB;
        }
        
        /**
         * 
         * Set Associative section
         */
         //2-Way
        cacheset2W = (address>>5)%cacheline2W;
        cachetag2W = address >> ((int) log2(cacheline2W));
        int way2W = 0;
        bool found2W = false;
        for (int i =0; i < 2; i++)
        {
            if (cache2W[cacheset2W][i] == cachetag2W)
            {
                found2W = true;
                way2W = i;
            }
        }
        //if it is found in the cache
        if (found2W)
        {
            int LRUindex2W = -1;
            //this checks where it is regards to LRU
            for (int i = 0; i < 2; i++)
            {
                if (LRUcache2W[cacheset2W][i] == way2W)
                {
                    LRUindex2W = i;
                }
            }
            //update the most recently used by shifting the LRU 
            for (int i =0; i < LRUindex2W; i++)
            {
                LRUcache2W[cacheset2W][LRUindex2W-i] = LRUcache2W[cacheset2W][(LRUindex2W-1)-i];
            }
            LRUcache2W[cacheset2W][0] = way2W;
            set2W++;
        }
        //if it's not in the cache, we update the spot with the tag
        else
        {
            int miss = LRUcache2W[cacheset2W][2-1];
            for (int i = 0; i < 2; i++)
            {
                LRUcache2W[cacheset2W][2-i] = LRUcache2W[cacheset2W][(2 - 1)-i];
            }
            LRUcache2W[cacheset2W][0] = miss;
            cache2W[cacheset2W][LRUcache2W[cacheset2W][0]] = cachetag2W;
        }
        //4-Way
        cacheset4W = (address>>5)%cacheline4W;
        cachetag4W = address >> ((int) log2(cacheline4W));
        int way4W = 0;
        bool found4W = false;
        for (int i =0; i < 4; i++)
        {
            if (cache4W[cacheset4W][i] == cachetag4W)
            {
                found4W = true;
                way4W = i;
            }
        }
        //if it is found in the cache
        if (found4W)
        {
            int LRUindex4W = -1;
            //this checks where it is regards to LRU
            for (int i = 0; i < 4; i++)
            {
                if (LRUcache4W[cacheset4W][i] == way4W)
                {
                    LRUindex4W = i;
                }
            }
            //update the most recently used by shifting the LRU 
            for (int i =0; i < LRUindex4W; i++)
            {
                LRUcache4W[cacheset4W][LRUindex4W-i] = LRUcache4W[cacheset4W][(LRUindex4W-1)-i];
            }
            LRUcache4W[cacheset4W][0] = way4W;
            set4W++;
        }
        //if it's not in the cache, we update the spot with the tag
        else
        {
            int miss = LRUcache4W[cacheset4W][4-1];
            for (int i = 0; i < 4; i++)
            {
                LRUcache4W[cacheset4W][4-i] = LRUcache4W[cacheset4W][(4 - 1)-i];
            }
            LRUcache4W[cacheset4W][0] = miss;
            cache4W[cacheset4W][LRUcache4W[cacheset4W][0]] = cachetag4W;
        }
        //8-Way
        cacheset8W = (address>>5)%cacheline8W;
        cachetag8W = address >> ((int) log2(cacheline8W));
        int way8W = 0;
        bool found8W = false;
        for (int i =0; i < 8; i++)
        {
            if (cache8W[cacheset8W][i] == cachetag8W)
            {
                found8W = true;
                way8W = i;
            }
        }
        //if it is found in the cache
        if (found8W)
        {
            int LRUindex8W = -1;
            //this checks where it is regards to LRU
            for (int i = 0; i < 8; i++)
            {
                if (LRUcache8W[cacheset8W][i] == way8W)
                {
                    LRUindex8W = i;
                }
            }
            //update the most recently used by shifting the LRU 
            for (int i =0; i < LRUindex8W; i++)
            {
                LRUcache8W[cacheset8W][LRUindex8W-i] = LRUcache8W[cacheset8W][(LRUindex8W-1)-i];
            }
            LRUcache8W[cacheset8W][0] = way8W;
            set8W++;
        }
        //if it's not in the cache, we update the spot with the tag
        else
        {
            int miss = LRUcache8W[cacheset8W][8-1];
            for (int i = 0; i < 8; i++)
            {
                LRUcache8W[cacheset8W][8-i] = LRUcache8W[cacheset8W][(8 - 1)-i];
            }
            LRUcache8W[cacheset8W][0] = miss;
            cache8W[cacheset8W][LRUcache8W[cacheset8W][0]] = cachetag8W;
        }
        //16-Way
        cacheset16W = (address>>5)%cacheline16W;
        cachetag16W = address >> ((int) log2(cacheline16W));
        int way16W = 0;
        bool found16W = false;
        for (int i =0; i < 16; i++)
        {
            if (cache16W[cacheset16W][i] == cachetag16W)
            {
                found16W = true;
                way16W = i;
            }
        }
        //if it is found in the cache
        if (found16W)
        {
            int LRUindex16W = -1;
            //this checks where it is regards to LRU
            for (int i = 0; i < 16; i++)
            {
                if (LRUcache16W[cacheset16W][i] == way16W)
                {
                    LRUindex16W = i;
                }
            }
            //update the most recently used by shifting the LRU 
            for (int i =0; i < LRUindex16W; i++)
            {
                LRUcache16W[cacheset16W][LRUindex16W-i] = LRUcache16W[cacheset16W][(LRUindex16W-1)-i];
            }
            LRUcache16W[cacheset16W][0] = way16W;
            set16W++;
        }
        //if it's not in the cache, we update the spot with the tag
        else
        {
            int miss = LRUcache16W[cacheset16W][16-1];
            for (int i = 0; i < 16; i++)
            {
                LRUcache16W[cacheset16W][16-i] = LRUcache16W[cacheset16W][(16 - 1)-i];
            }
            LRUcache16W[cacheset16W][0] = miss;
            cache16W[cacheset16W][LRUcache16W[cacheset16W][0]] = cachetag16W;
        }
        /**
         * 
         * Fully Associative LRU
         * 
         */ 
        tagLRU = address>>5;
        int indexLRU = 0;
        bool foundLRU = false;
        for (int i = 0; i < cachelineLRU; i++)
        {
            if(cacheLRU[0][i] == tagLRU)
            {
                foundLRU = true;
                indexLRU = i;
            }
        }
        //if found
        if (foundLRU)
        {
            int tempIndex = -1;
            for (int i = 0; i < cachelineLRU; i++)
            {
                if (LRUsimFA[0][i] == indexLRU)
                {
                    tempIndex = i;
                }
            }
            for (int i = 0; i < tempIndex; i++)
            {
                LRUsimFA[0][tempIndex-i] = LRUsimFA[0][(tempIndex-1)-i];
            }
            LRUsimFA[0][0] = indexLRU;
            LRUcount++;
        }
        else
        {
            int miss = LRUsimFA[0][cachelineLRU-1];
            for (int i = 0; i < cachelineLRU; i++)
            {
                LRUsimFA[0][cachelineLRU-i] = LRUsimFA[0][(cachelineLRU-1)-i];
            }
            LRUsimFA[0][0] = miss;
            cacheLRU[0][LRUsimFA[0][0]] = tagLRU;
        }
        
        /**
         * 
         * Fully Associative HOT COLD
         * INCOMPLETE!
         * 
         * 
         * 
        */ 
        tagHC = address >> 5;
        //int indexHC = 0;
        bool foundHC = false;
        for (int i = 0; i < cachelineHC; i++)
        {
            if(cacheHC[i] == tagHC)
            {
                foundHC = true;
                //indexHC = i;
            }
        }
        //if found
        if (foundHC)
        {
            HCcount++;
        }
        //if it's a miss
        else
        {
            int m = 0;
            while(m < 256)
            {
                //if starting node is 0
                if (HCsimFA[m] == 0)
                {
                    HCsimFA[m] = 1;
                    //check if left is 0
                    if (HCsimFA[2*m+1] == 0)
                    {
                        // //if left is 0, change the spot
                        // HCsimFA[m] = 1;
                        // HCsimFA[2*m+1] = 0;
                        // //HCsimFA[m+1] = 0;
                        m = 2*m + 1;
                    }
                    else if (HCsimFA[2*m+2] == 0)
                    //check if right is 0
                    {
                        // //if right is 0, change the spot
                        // //HCsimFA[m] = 0;
                        // HCsimFA[2*m+1] = 0;
                        // HCsimFA[m] = 1;
                        m = 2*m + 2;
                    }
                    else
                    {
                        HCsimFA[m] = 0;
                    }
                }
                //or continue to next node.. either 0 or 1.
                else
                {
                    m++;
                }
            }
            //change the cache where it is coldest to have the tag
            cacheHC[m] = tagHC;
        }
        /**
         * 
         * Set Associative Cache with no Allocation on a Write Miss SECTION
         * 
         * 
         * 
         */
         //2-Way
        cacheset2NA = (address>>5)%cacheline2NA;
        cachetag2NA = address >> ((int) log2(cacheline2NA));
        int way2NA = 0;
        bool found2NA = false;
        for (int i =0; i < 2; i++)
        {
            if (cache2NA[cacheset2NA][i] == cachetag2NA)
            {
                found2NA = true;
                way2NA = i;
            }
        }
        //if it is found in the cache
        if (found2NA)
        {
            int LRUindex2NA = -1;
            //this checks where it is regards to LRU
            for (int i = 0; i < 2; i++)
            {
                if (LRUcache2NA[cacheset2NA][i] == way2NA)
                {
                    LRUindex2NA = i;
                }
            }
            //update the most recently used by shifting the LRU 
            for (int i =0; i < LRUindex2NA; i++)
            {
                LRUcache2NA[cacheset2NA][LRUindex2NA-i] = LRUcache2NA[cacheset2NA][(LRUindex2NA-1)-i];
            }
            LRUcache2NA[cacheset2NA][0] = way2NA;
            set2NA++;
        }
        else
        {
            //if a store instruction misses the cache then the missing line is written directly to memory
            //so... only L works.
            if (instr == "L")
            {
                int miss = LRUcache2NA[cacheset2NA][1];
                for (int i = 0; i < 2; i++)
                {
                    LRUcache2NA[cacheset2NA][2-i] = LRUcache2NA[cacheset2NA][1-i];
                }
                LRUcache2NA[cacheset2NA][0] = miss;
                cache2NA[cacheset2NA][LRUcache2NA[cacheset2NA][0]] = cachetag2NA;
            }
            // int miss = LRUcache2W[cacheset2W][2-1];
            // for (int i = 0; i < 2; i++)
            // {
            //     LRUcache2W[cacheset2W][2-i] = LRUcache2W[cacheset2W][(2 - 1)-i];
            // }
            // LRUcache2W[cacheset2W][0] = miss;
            // cache2W[cacheset2W][LRUcache2W[cacheset2W][0]] = cachetag2W;
        }
        //4-Way
        cacheset4NA = (address>>5)%cacheline4NA;
        cachetag4NA = address >> ((int) log2(cacheline4NA));
        int way4NA = 0;
        bool found4NA = false;
        for (int i =0; i < 4; i++)
        {
            if (cache4NA[cacheset4NA][i] == cachetag4NA)
            {
                found4NA = true;
                way4NA = i;
            }
        }
        //if it is found in the cache
        if (found4NA)
        {
            int LRUindex4NA = -1;
            //this checks where it is regards to LRU
            for (int i = 0; i < 4; i++)
            {
                if (LRUcache4NA[cacheset4NA][i] == way4NA)
                {
                    LRUindex4NA = i;
                }
            }
            //update the most recently used by shifting the LRU 
            for (int i =0; i < LRUindex4NA; i++)
            {
                LRUcache4NA[cacheset4NA][LRUindex4NA-i] = LRUcache4NA[cacheset4NA][(LRUindex4NA-1)-i];
            }
            LRUcache4NA[cacheset4NA][0] = way4NA;
            set4NA++;
        }
        else
        {
            //if a store instruction misses the cache then the missing line is written directly to memory
            //so... only L works.
            if (instr == "L")
            {
                int miss = LRUcache4NA[cacheset4NA][3];
                for (int i = 0; i < 4; i++)
                {
                    LRUcache4NA[cacheset4NA][4-i] = LRUcache4NA[cacheset4NA][3-i];
                }
                LRUcache4NA[cacheset4NA][0] = miss;
                cache4NA[cacheset4NA][LRUcache4NA[cacheset4NA][0]] = cachetag4NA;
            }
        }
        //8-Way
        cacheset8NA = (address>>5)%cacheline8NA;
        cachetag8NA = address >> ((int) log2(cacheline8NA));
        int way8NA = 0;
        bool found8NA = false;
        for (int i =0; i < 8; i++)
        {
            if (cache8NA[cacheset8NA][i] == cachetag8NA)
            {
                found8NA = true;
                way8NA = i;
            }
        }
        //if it is found in the cache
        if (found8NA)
        {
            int LRUindex8NA = -1;
            //this checks where it is regards to LRU
            for (int i = 0; i < 8; i++)
            {
                if (LRUcache8NA[cacheset8NA][i] == way8NA)
                {
                    LRUindex8NA = i;
                }
            }
            //update the most recently used by shifting the LRU 
            for (int i =0; i < LRUindex8NA; i++)
            {
                LRUcache8NA[cacheset8NA][LRUindex8NA-i] = LRUcache8NA[cacheset8NA][(LRUindex8NA-1)-i];
            }
            LRUcache8NA[cacheset8NA][0] = way8NA;
            set8NA++;
        }
        else
        {
            //if a store instruction misses the cache then the missing line is written directly to memory
            //so... only L works.
            if (instr == "L")
            {
                int miss = LRUcache8NA[cacheset8NA][7];
                for (int i = 0; i < 8; i++)
                {
                    LRUcache8NA[cacheset8NA][8-i] = LRUcache8NA[cacheset8NA][7-i];
                }
                LRUcache8NA[cacheset8NA][0] = miss;
                cache8NA[cacheset8NA][LRUcache8NA[cacheset8NA][0]] = cachetag8NA;
            }
        }
        //16-Way
        cacheset16NA = (address>>5)%cacheline16NA;
        cachetag16NA = address >> ((int) log2(cacheline16NA));
        int way16NA = 0;
        bool found16NA = false;
        for (int i =0; i < 16; i++)
        {
            if (cache16NA[cacheset16NA][i] == cachetag16NA)
            {
                found16NA = true;
                way16NA = i;
            }
        }
        //if it is found in the cache
        if (found16NA)
        {
            int LRUindex16NA = -1;
            //this checks where it is regards to LRU
            for (int i = 0; i < 16; i++)
            {
                if (LRUcache16NA[cacheset16NA][i] == way16NA)
                {
                    LRUindex16NA = i;
                }
            }
            //update the most recently used by shifting the LRU 
            for (int i =0; i < LRUindex16NA; i++)
            {
                LRUcache16NA[cacheset16NA][LRUindex16NA-i] = LRUcache16NA[cacheset16NA][(LRUindex16NA-1)-i];
            }
            LRUcache16NA[cacheset16NA][0] = way16NA;
            set16NA++;
        }
        else
        {
            //if a store instruction misses the cache then the missing line is written directly to memory
            //so... only L works.
            if (instr == "L")
            {
                int miss = LRUcache16NA[cacheset16NA][15];
                for (int i = 0; i < 16; i++)
                {
                    LRUcache16NA[cacheset16NA][16-i] = LRUcache16NA[cacheset16NA][15-i];
                }
                LRUcache16NA[cacheset16NA][0] = miss;
                cache16NA[cacheset16NA][LRUcache16NA[cacheset16NA][0]] = cachetag16NA;
            }
        }
        /**
         * 
         * Set Associative section with next line prefetching
         * 
         * 
         */
         //2-Way
        cacheset2PA = (address>>5)%cacheline2PA;
        cachetag2PA = address >> ((int) log2(cacheline2PA));
        int way2PA = 0;
        int cacheNext2PA = 0;
        bool found2PA = false;
        for (int i =0; i < 2; i++)
        {
            if (cache2PA[cacheset2PA][i] == cachetag2PA)
            {
                found2PA = true;
                way2PA = i;
                cacheNext2PA = i+1;
            }
        }
        //if it is found in the cache
        if (found2PA)
        {
            int LRUindex2PA = -1;
            //this checks where it is regards to LRU
            for (int i = 0; i < 2; i++)
            {
                if (LRUcache2PA[cacheset2PA][i] == way2PA)
                {
                    LRUindex2PA = i;
                }
            }
            //update the most recently used by shifting the LRU 
            for (int i =0; i < LRUindex2PA; i++)
            {
                LRUcache2PA[cacheset2PA][LRUindex2PA-i] = LRUcache2PA[cacheset2PA][(LRUindex2PA-1)-i];
            }
            LRUcache2PA[cacheset2PA][0] = way2PA;
            ss>>instr>>std::hex>>address;
            cachetag2PA = address >> ((int) log2(cacheline2PA));
            cache2PA[cacheset2PA][cacheNext2PA] = cachetag2PA;
            set2PA++;
        }
        //if it's not in the cache, we update the spot with the tag
        else
        {
            int miss = LRUcache2PA[cacheset2PA][2-1];
            for (int i = 0; i < 2; i++)
            {
                LRUcache2PA[cacheset2PA][2-i] = LRUcache2PA[cacheset2PA][(2 - 1)-i];
            }
            LRUcache2PA[cacheset2PA][0] = miss;
            cache2PA[cacheset2PA][LRUcache2PA[cacheset2PA][0]] = cachetag2PA;
        }
        //4-Way
        cacheset4PA = (address>>5)%cacheline4PA;
        cachetag4PA = address >> ((int) log2(cacheline4PA));
        int way4PA = 0;
        int cacheNext4PA = 0;
        bool found4PA = false;
        for (int i =0; i < 4; i++)
        {
            if (cache4PA[cacheset4PA][i] == cachetag4PA)
            {
                found4PA = true;
                way4PA = i;
                cacheNext4PA = i+1;
            }
        }
        //if it is found in the cache
        if (found4PA)
        {
            int LRUindex4PA = -1;
            //this checks where it is regards to LRU
            for (int i = 0; i < 4; i++)
            {
                if (LRUcache4PA[cacheset4PA][i] == way4PA)
                {
                    LRUindex4PA = i;
                }
            }
            //update the most recently used by shifting the LRU 
            for (int i =0; i < LRUindex4PA; i++)
            {
                LRUcache4PA[cacheset4PA][LRUindex4PA-i] = LRUcache4PA[cacheset4PA][(LRUindex4PA-1)-i];
            }
            LRUcache4PA[cacheset4PA][0] = way4PA;
            cache4PA[cacheset4PA][LRUcache4PA[cacheset4PA][0]] = cachetag4PA;
            ss>>instr>>std::hex>>address;
            cachetag4PA = address >> ((int) log2(cacheline4PA));
            cache4PA[cacheset4PA][cacheNext4PA] = cachetag4PA;
            set4PA++;
        }
        //if it's not in the cache, we update the spot with the tag
        else
        {
            int miss = LRUcache4PA[cacheset4PA][4-1];
            for (int i = 0; i < 4; i++)
            {
                LRUcache4PA[cacheset4PA][4-i] = LRUcache4PA[cacheset4PA][(4 - 1)-i];
            }
            LRUcache4PA[cacheset4PA][0] = miss;
            cache4PA[cacheset4PA][LRUcache4PA[cacheset4PA][0]] = cachetag4PA;
        }
        //8-Way
        cacheset8PA = (address>>5)%cacheline8PA;
        cachetag8PA = address >> ((int) log2(cacheline8PA));
        int way8PA = 0;
        int cacheNext8PA = 0;
        bool found8PA = false;
        for (int i =0; i < 8; i++)
        {
            if (cache8PA[cacheset8PA][i] == cachetag8PA)
            {
                found8PA = true;
                way8PA = i;
                cacheNext8PA = i+1;
            }
        }
        //if it is found in the cache
        if (found8PA)
        {
            int LRUindex8PA = -1;
            //this checks where it is regards to LRU
            for (int i = 0; i < 8; i++)
            {
                if (LRUcache8PA[cacheset8PA][i] == way8PA)
                {
                    LRUindex8PA = i;
                }
            }
            //update the most recently used by shifting the LRU 
            for (int i =0; i < LRUindex8PA; i++)
            {
                LRUcache8PA[cacheset8PA][LRUindex8PA-i] = LRUcache8PA[cacheset8PA][(LRUindex8PA-1)-i];
            }
            LRUcache8PA[cacheset8PA][0] = way8PA;
            cache8PA[cacheset8PA][LRUcache8PA[cacheset8PA][0]] = cachetag8PA;
            ss>>instr>>std::hex>>address;
            cachetag8PA = address >> ((int) log2(cacheline8PA));
            cache8PA[cacheset8PA][cacheNext8PA] = cachetag8PA;
            set8PA++;
        }
        //if it's not in the cache, we update the spot with the tag
        else
        {
            int miss = LRUcache8PA[cacheset8PA][8-1];
            for (int i = 0; i < 8; i++)
            {
                LRUcache8PA[cacheset8PA][8-i] = LRUcache8PA[cacheset8PA][(8 - 1)-i];
            }
            LRUcache8PA[cacheset8PA][0] = miss;
            cache8PA[cacheset8PA][LRUcache8PA[cacheset8PA][0]] = cachetag8PA;
        }
        //16-Way
        cacheset16PA = (address>>5)%cacheline16PA;
        cachetag16PA = address >> ((int) log2(cacheline16PA));
        int way16PA = 0;
        int cacheNext16PA = 0;
        bool found16PA = false;
        for (int i =0; i < 16; i++)
        {
            if (cache16PA[cacheset16PA][i] == cachetag16PA)
            {
                found16PA = true;
                way16PA = i;
                cacheNext16PA = i+1;
            }
        }
        //if it is found in the cache
        if (found16PA)
        {
            int LRUindex16PA = -1;
            //this checks where it is regards to LRU
            for (int i = 0; i < 16; i++)
            {
                if (LRUcache16PA[cacheset16PA][i] == way16PA)
                {
                    LRUindex16PA = i;
                }
            }
            //update the most recently used by shifting the LRU 
            for (int i =0; i < LRUindex16PA; i++)
            {
                LRUcache16PA[cacheset16PA][LRUindex16PA-i] = LRUcache16PA[cacheset16PA][(LRUindex16PA-1)-i];
            }
            LRUcache16PA[cacheset16PA][0] = way16PA;
            cache16PA[cacheset16PA][LRUcache16PA[cacheset16PA][0]] = cachetag16PA;
            ss>>instr>>std::hex>>address;
            cachetag16PA = address >> ((int) log2(cacheline16PA));
            cache16PA[cacheset16PA][cacheNext16PA] = cachetag16PA;
            set16PA++;
        }
        //if it's not in the cache, we update the spot with the tag
        else
        {
            int miss = LRUcache16PA[cacheset16PA][16-1];
            for (int i = 0; i < 16; i++)
            {
                LRUcache16PA[cacheset16PA][16-i] = LRUcache16PA[cacheset16PA][(16 - 1)-i];
            }
            LRUcache16PA[cacheset16PA][0] = miss;
            cache16PA[cacheset16PA][LRUcache16PA[cacheset16PA][0]] = cachetag16PA;
        }
        /**
         * 
         * Set Associative section prefetch on a miss
         */
         //2-way
        cacheset2PM = (address>>5)%cacheline2PM;
        cachetag2PM = address >> ((int) log2(cacheline2PM));
        int cacheNext2PM = 0;
        int way2PM = 0;
        bool found2PM = false;
        for (int i =0; i < 2; i++)
        {
            if (cache2PM[cacheset2PM][i] == cachetag2PM)
            {
                found2PM = true;
                way2PM = i;
                cacheNext2PM = i+1;
            }
        }
        //if it is found in the cache
        if (found2PM)
        {
            int LRUindex2PM = -1;
            //this checks where it is regards to LRU
            for (int i = 0; i < 2; i++)
            {
                if (LRUcache2PM[cacheset2PM][i] == way2PM)
                {
                    LRUindex2PM = i;
                }
            }
            //update the most recently used by shifting the LRU 
            for (int i =0; i < LRUindex2PM; i++)
            {
                LRUcache2PM[cacheset2PM][LRUindex2PM-i] = LRUcache2PM[cacheset2PM][(LRUindex2PM-1)-i];
            }
            LRUcache2PM[cacheset2PM][0] = way2PM;
            set2PM++;
        }
        //if it's not in the cache, we update the spot with the tag
        else
        {
            int miss = LRUcache2PM[cacheset2PM][2-1];
            for (int i = 0; i < 2; i++)
            {
                LRUcache2PM[cacheset2PM][2-i] = LRUcache2PM[cacheset2PM][(2 - 1)-i];
            }
            LRUcache2PM[cacheset2PM][0] = miss;
            cache2PM[cacheset2PM][LRUcache2PM[cacheset2PM][0]] = cachetag2PM;
            ss>>instr>>std::hex>>address;
            cachetag2PM = address >> ((int) log2(cacheline2PM));
            cache2PM[cacheset2PM][cacheNext2PM] = cachetag2PM;
        }
         //4-way
        cacheset4PM = (address>>5)%cacheline4PM;
        cachetag4PM = address >> ((int) log2(cacheline4PM));
        int cacheNext4PM = 0;
        int way4PM = 0;
        bool found4PM = false;
        for (int i =0; i < 4; i++)
        {
            if (cache4PM[cacheset4PM][i] == cachetag4PM)
            {
                found4PM = true;
                way4PM = i;
                cacheNext4PM = i+1;
            }
        }
        //if it is found in the cache
        if (found4PM)
        {
            int LRUindex4PM = -1;
            //this checks where it is regards to LRU
            for (int i = 0; i < 4; i++)
            {
                if (LRUcache4PM[cacheset4PM][i] == way4PM)
                {
                    LRUindex4PM = i;
                }
            }
            //update the most recently used by shifting the LRU 
            for (int i =0; i < LRUindex4PM; i++)
            {
                LRUcache4PM[cacheset4PM][LRUindex4PM-i] = LRUcache4PM[cacheset4PM][(LRUindex4PM-1)-i];
            }
            LRUcache4PM[cacheset4PM][0] = way4PM;
            set4PM++;
        }
        //if it's not in the cache, we update the spot with the tag
        else
        {
            int miss = LRUcache4PM[cacheset4PM][4-1];
            for (int i = 0; i < 4; i++)
            {
                LRUcache4PM[cacheset4PM][4-i] = LRUcache4PM[cacheset4PM][(4 - 1)-i];
            }
            LRUcache4PM[cacheset4PM][0] = miss;
            cache4PM[cacheset4PM][LRUcache4PM[cacheset4PM][0]] = cachetag4PM;
            ss>>instr>>std::hex>>address;
            cachetag4PM = address >> ((int) log2(cacheline4PM));
            cache4PM[cacheset4PM][cacheNext4PM] = cachetag4PM;
        }
         //8-way
        cacheset8PM = (address>>5)%cacheline8PM;
        cachetag8PM = address >> ((int) log2(cacheline8PM));
        int cacheNext8PM = 0;
        int way8PM = 0;
        bool found8PM = false;
        for (int i =0; i < 8; i++)
        {
            if (cache8PM[cacheset8PM][i] == cachetag8PM)
            {
                found8PM = true;
                way8PM = i;
                cacheNext8PM = i+1;
            }
        }
        //if it is found in the cache
        if (found8PM)
        {
            int LRUindex8PM = -1;
            //this checks where it is regards to LRU
            for (int i = 0; i < 8; i++)
            {
                if (LRUcache8PM[cacheset8PM][i] == way8PM)
                {
                    LRUindex8PM = i;
                }
            }
            //update the most recently used by shifting the LRU 
            for (int i =0; i < LRUindex8PM; i++)
            {
                LRUcache8PM[cacheset8PM][LRUindex8PM-i] = LRUcache8PM[cacheset8PM][(LRUindex8PM-1)-i];
            }
            LRUcache8PM[cacheset8PM][0] = way8PM;
            set8PM++;
        }
        //if it's not in the cache, we update the spot with the tag
        else
        {
            int miss = LRUcache8PM[cacheset8PM][8-1];
            for (int i = 0; i < 8; i++)
            {
                LRUcache8PM[cacheset8PM][8-i] = LRUcache8PM[cacheset8PM][(8 - 1)-i];
            }
            LRUcache8PM[cacheset8PM][0] = miss;
            cache8PM[cacheset8PM][LRUcache8PM[cacheset8PM][0]] = cachetag8PM;
            ss>>instr>>std::hex>>address;
            cachetag8PM = address >> ((int) log2(cacheline8PM));
            cache8PM[cacheset8PM][cacheNext8PM] = cachetag8PM;
        }
         //16-way
        cacheset16PM = (address>>5)%cacheline16PM;
        cachetag16PM = address >> ((int) log2(cacheline16PM));
        int cacheNext16PM = 0;
        int way16PM = 0;
        bool found16PM = false;
        for (int i =0; i < 16; i++)
        {
            if (cache16PM[cacheset16PM][i] == cachetag16PM)
            {
                found16PM = true;
                way16PM = i;
                cacheNext16PM = i+1;
            }
        }
        //if it is found in the cache
        if (found16PM)
        {
            int LRUindex16PM = -1;
            //this checks where it is regards to LRU
            for (int i = 0; i < 16; i++)
            {
                if (LRUcache16PM[cacheset16PM][i] == way16PM)
                {
                    LRUindex16PM = i;
                }
            }
            //update the most recently used by shifting the LRU 
            for (int i =0; i < LRUindex16PM; i++)
            {
                LRUcache16PM[cacheset16PM][LRUindex16PM-i] = LRUcache16PM[cacheset16PM][(LRUindex16PM-1)-i];
            }
            LRUcache16PM[cacheset16PM][0] = way16PM;
            set16PM++;
        }
        //if it's not in the cache, we update the spot with the tag
        else
        {
            int miss = LRUcache16PM[cacheset16PM][16-1];
            for (int i = 0; i < 16; i++)
            {
                LRUcache16PM[cacheset16PM][16-i] = LRUcache16PM[cacheset16PM][(16 - 1)-i];
            }
            LRUcache16PM[cacheset16PM][0] = miss;
            cache16PM[cacheset16PM][LRUcache16PM[cacheset16PM][0]] = cachetag16PM;
            ss>>instr>>std::hex>>address;
            cachetag16PM = address >> ((int) log2(cacheline16PM));
            cache16PM[cacheset16PM][cacheNext16PM] = cachetag16PM;
        }
    }

    // direct mapped results
    // cout << direct1KB << endl;
    // cout << direct4KB << endl;
    // cout << direct16KB << endl;
    // cout << direct32KB << endl;
    // cout << totalNum << endl;
    output << direct1KB << "," << totalNum << "; " << direct4KB << "," << totalNum << "; " << 
    direct16KB << "," << totalNum << "; " << direct32KB << "," << totalNum << ";" << endl;
    // cout << set2W << endl;
    // cout << set4W << endl;
    // cout << set8W << endl;
    // cout << set16W << endl;
    output << set2W << "," << totalNum << "; " << set4W << "," << totalNum << "; " << 
    set8W << "," << totalNum << "; " << set16W << "," << totalNum << ";" << endl;
    // cout << LRUcount << endl;
    output << LRUcount << "," << totalNum <<";" << endl;
    // cout << HCcount << endl;
    output << HCcount << ";" << totalNum << ";" << endl;
    // cout << set2NA << endl;
    // cout << set4NA << endl;
    // cout << set8NA << endl;
    // cout << set16NA << endl;
    output << set2NA << "," << totalNum << "; " << set4NA << "," << totalNum << "; " << 
    set8NA << "," << totalNum << "; " << set16NA << "," << totalNum << ";" << endl;
    // cout << set2PA << endl;
    // cout << set4PA << endl;
    // cout << set8PA << endl;
    // cout << set16PA << endl;
    output << set2PA << "," << totalNum << "; " << set4PA << "," << totalNum << "; " << 
    set8PA << "," << totalNum << "; " << set16PA << "," << totalNum << ";" << endl;
    // cout << set2PM << endl;
    // cout << set4PM << endl;
    // cout << set8PM << endl;
    // cout << set16PM << endl;
    output << set2PM << "," << totalNum << "; " << set4PM << "," << totalNum << "; " << 
    set8PM << "," << totalNum << "; " << set16PM << "," << totalNum << ";" << endl;
    output.close();
}