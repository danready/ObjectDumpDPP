
/* Sample UDP client */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include <arpa/inet.h>

#define VERBOSE 0

int main(int argc, char**argv)
{

    FILE *ptr_myfile;

    FILE *log_file;
    FILE *wf_file;
    FILE *time_file;
    
    
    int i, j;
    int offset = 0;
    int event_found = 0;
    
    uint32_t nwords = 0;
    uint32_t word = 0;
    
    int ch_mask = 0;

    uint32_t event_num = 0;
    uint32_t trigger_time_tag = 0;
    uint32_t trigger_time_tag_ch = 0;
    uint32_t baseline = 0;
    uint32_t nsamples = 0;
    uint32_t skipped = 0;
    
    uint16_t sample_value = 0;
    
    wf_file = fopen ("wf.txt", "w+");
    time_file = fopen ("time.txt", "w+");
    log_file = fopen ("log.txt", "w+");
    
    ptr_myfile=fopen("test1.txt","rb");
    if (!ptr_myfile)
    {
        printf("Unable to open file!");
        return 1;
    }


    
    offset = 0;
    event_found = 0;
    
    while (event_found == 0) {
        
    
        //// check event found from the first word of the header
    
        fseek(ptr_myfile,offset,SEEK_SET);
        fread(&nwords,4,1,ptr_myfile);
        
       printf("Header - Offset = %x - %d\n",nwords,offset);
    
        if ((nwords >> 28) == 0xa) {
            nwords = nwords & 0xFFFFFFF;
           if (VERBOSE) printf("Header eventsize - nwords to read = %u - %x\n",nwords,nwords);
            fprintf(log_file, "Header eventsize - nwords to read = %u\n",nwords);
            event_found = 0;
        } else {
            event_found = 1;
        }
    
    
        if (event_found == 0) {
            /// channel mask
            word = 0;
            fseek(ptr_myfile,offset+4,SEEK_SET);
            fread(&word,4,1,ptr_myfile);
    
            ch_mask = word & 0xF;
            if (VERBOSE) printf("Channel Mask = %x\n", ch_mask);
            fprintf(log_file, "Channel Mask = %x\n", ch_mask);
    
            /// event counter
            word = 0;
            fseek(ptr_myfile,offset+8,SEEK_SET);
            fread(&word,4,1,ptr_myfile);
    
            event_num = word & 0xFFFFFF;
            if (VERBOSE) printf("Event Num = %u\n", event_num);
            fprintf(log_file, "Event Num = %u\n", event_num);
    
    
            /// trigger time tag
            trigger_time_tag = 0;
            fseek(ptr_myfile,offset+12,SEEK_SET);
            fread(&trigger_time_tag,4,1,ptr_myfile);
    
            if (VERBOSE) printf("Trigger Time Tag = %u\n", trigger_time_tag);
            fprintf(log_file, "Trigger Time Tag = %u\n", trigger_time_tag);

            /////// qui bisognerebbe fare il loop sui canali
            /////// non inseriamo perche' abbiamo un canale solo
        
            /// words counter channel
            word = 0;
            fseek(ptr_myfile,offset+16,SEEK_SET);
            fread(&word,4,1,ptr_myfile);
    
            if (VERBOSE) printf("Event CH length = %u\n", word);
            fprintf(log_file, "Event CH length = %u\n", word);

            /// trigger time tag channel
            trigger_time_tag_ch = 0;
            fseek(ptr_myfile,offset+20,SEEK_SET);
            fread(&trigger_time_tag_ch,4,1,ptr_myfile);
    
            if (VERBOSE) printf("Trigger Time Tag ch = %u\n", trigger_time_tag_ch);
            fprintf(log_file, "Trigger Time Tag ch = %u\n", trigger_time_tag_ch);
    
            /// baseline
            baseline = 0;
            fseek(ptr_myfile,offset+24,SEEK_SET);
            fread(&baseline,4,1,ptr_myfile);
    
            if (VERBOSE) printf("Baseline value = %u\n", baseline);
            fprintf(log_file, "Baseline value = %u\n", baseline);

    

            nsamples = 0;
            for ( i = 7; i < nwords; i++) {
            
                //printf("Reading sampling words = %d\n",i);
            
                word = 0;
                fseek(ptr_myfile,offset+4*i,SEEK_SET);
                fread(&word,4,1,ptr_myfile);
        
                ////// check if skipped samples words or real sample
                skipped = 0;
                if((word>>30) == 0) /// skipped samples
                {
                    skipped = 8*(word & 0x3FFFFFFF);
                    nsamples += skipped;
                
                    for (j = 0; j < skipped; j++) {
                        fprintf(wf_file, "%u %u %d\n", event_num, baseline, 0);
                    }
                }
        

                //// acquired samples
                if((word>>30) == 3) /// acquired samples
                {
            
                    for (j = 0; j < 3; j++) {
                        sample_value = 0;
                        sample_value = (word & (0x3FF << (j*10)))>>(j*10);
                        fprintf(wf_file, "%u %u %d\n", event_num, sample_value, 1);
                        nsamples++;
                    }
                }
            } //// end loop on channel samples words
        
        
           if (VERBOSE) printf("N samples = %u\n", nsamples);
            fprintf(log_file, "N samples = %u\n", nsamples);
        
            fprintf(time_file, "%u %u %u %u\n", event_num, nsamples, trigger_time_tag, trigger_time_tag_ch);
        
            offset += 4*nwords;
        }
    
    }
    
    
    
    
    
    
    fclose(ptr_myfile);
    fclose(log_file);
    fclose(time_file);
    fclose(wf_file);
    
    return 0;
    
    
    
}



