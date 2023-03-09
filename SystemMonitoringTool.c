#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <utmp.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

void simple_memory_info(int num_samples, int sec_delay, int i)
{
    struct sysinfo mem;
    sysinfo(&mem);
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    long mem_used = usage.ru_maxrss;

    printf("Nbr of samples: %d -- every %d secs\n", num_samples, sec_delay);
    printf(" Memory usage: %ld kilobytes\n", mem_used); //print memory usage

    printf("---------------------------------------\n");

    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
    
    long int tot_mem = mem.totalram; //get the total usuable main memory size
    long int free_mem = mem.freeram; //get the available memory size

    float phys_tot = mem.totalram; //get the total usuable physical memory size
    float phys_free = mem.freeram; //get the available physical memory size
    float phys_used = tot_mem - free_mem; //get the physical memory usage

    float calc_tot_phys = (float)(phys_tot / 1073741824) * mem.mem_unit; //convert to GB
    float calc_used_phys = (float)(phys_used / 1073741824) * mem.mem_unit; //convert to GB

    float tot_swap = mem.totalswap; //get the total swap space size
    float free_swap = mem.freeswap; //get the swap space still available
    float used_swap = tot_swap - free_swap; //get the used swap space
    float virtual_tot = tot_swap + phys_tot; //get the total virtual memory
    float virtual_used = used_swap + phys_used; //get the virtual memory usage

    float calc_tot_virtual = (virtual_tot / 1073741824) * mem.mem_unit; //convert to GB
    float calc_used_virtual = (virtual_used / 1073741824) * mem.mem_unit; //convert to GB

    char storage[num_samples + 1][1024];

    storage[i][0] = '\0';
    sprintf(storage[i], "%.2f / %.2f -- %.2f / %.2f", calc_used_phys, calc_tot_phys, calc_used_virtual, calc_tot_virtual);

    for(int j = 0; j <= i; j++)
    {
        printf("%s\n", storage[j]);
    }

    return;

}

void get_sample(float* samp_usage)
{
   FILE *ps;
   //the following variables represent the stats we need from the first line
   float user; 
   float nice; 
   float system; 
   float idle; 
   float iowait;
   float irq;
   float softirq;
   float steal;
   char cpu[255];
   ps = fopen("/proc/stat", "r");

   fscanf(ps, "%s %f %f %f %f %f %f %f %f", cpu, &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal); //read and store values from file

   float sum1 = user + nice + system + idle + iowait + irq + softirq + steal;
   float idle1 = idle + iowait;
   float cpu_usage = sum1 - idle1;

   fclose(ps);
   *samp_usage = cpu_usage;
}


void simple_cpu_info(int tdelay)
{

   float sample_1;

   get_sample(&sample_1);
   sleep(tdelay);
   
   float sample_2;
   get_sample(&sample_2);
    
   float cpu_use = ((sample_2 - sample_1) / (sample_1)) * 100;
   printf(" total cpu use = %.2f%%\n", cpu_use);

   return;
}

void system_info()
{
    printf("### System Information ###\n");
    struct utsname info;
    uname(&info);
    printf("System Name = %s\n", info.sysname); //print operating system name
    printf("Machine Name = %s\n", info.nodename); //print machine name
    printf("Version = %s\n", info.version); //print operating system version
    printf("Release = %s\n", info.release); //print operating system release
    printf("Architecture = %s\n", info.machine); //print hardware identifier

    return;
}

void graphical_memory_info(int num_samples, int sec_delay, int i)
{
   printf("Nbr of samples: %d -- every %d secs\n", num_samples, sec_delay);
   struct sysinfo mem;
   sysinfo(&mem);
   long int tot_mem = mem.totalram; //get the total usuable main memory size
   long int free_mem = mem.freeram; //get the available memory size
   struct rusage usage;
   getrusage(RUSAGE_SELF, &usage);
   long mem_used = usage.ru_maxrss;
   printf("Memory usage: %ld kilobytes\n", mem_used); //print memory usage

   printf("---------------------------------------\n");

   printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");

   float phys_tot = mem.totalram; //get the total usuable physical memory size
   float phys_free = mem.freeram; //get the available physical memory size
   float phys_used = tot_mem - free_mem; //get the physical memory usage

   float calc_tot_phys = (float)(phys_tot / 1073741824) * mem.mem_unit; //convert to GB
   float calc_used_phys = (float)(phys_used / 1073741824) * mem.mem_unit; //convert to GB

   float tot_swap = mem.totalswap; //get the total swap space size
   float free_swap = mem.freeswap; //get the swap space still available
   float used_swap = tot_swap - free_swap; //get the used swap space
   float virtual_tot = tot_swap + phys_tot; //get the total virtual memory
   float virtual_used = used_swap + phys_used; //get the virtual memory usage

   float calc_tot_virtual = (virtual_tot / 1073741824) * mem.mem_unit; //convert to GB
   float calc_used_virtual = (virtual_used / 1073741824) * mem.mem_unit; //convert to GB

   char storage[num_samples][1024];
   char comp_virtual[num_samples][1024];
   char holder1[1024];
   float change = 0;
   
   sprintf(comp_virtual[i], "%.2f", calc_used_virtual);
   float comp_f1 = atof(comp_virtual[i]);
   sprintf(storage[i], "%.2f / %.2f -- %.2f / %.2f     |", calc_used_phys, calc_tot_phys, calc_used_virtual, calc_tot_virtual);

   if(i == 0)
   {
      sprintf(holder1, " %.2f (%.2f)\n", change, comp_f1);
      strcat(storage[i], holder1);
   }

   else
   {
      char holder2[1024];
      char holder3[1024];
      holder2[0] = '\0';
      holder3[0] = '\0';
      float comp_f2 = atof(comp_virtual[i-1]);
      float temp1 = comp_f1 - comp_f2;
      char temp2[1024];
      temp2[0] = '\0';
      sprintf(temp2, "%.2f", temp1);
      change = atof(temp2);
      
      if(change >= 0) 
      {
         for(float k = 0;k < change;k+=0.01)
         {
            strcat(holder2, "#");
         }
         strcat(holder2, "*");
         sprintf(holder3, " %.2f (%.2f)\n", change, comp_f1);
         strcat(holder2, holder3);
         strcat(storage[i], holder2);

      }

      else if(change < 0) 
      {
         for(float k = 0;k > change;k-=0.01)
         {
            strcat(holder2, ":");
         }
         strcat(holder2, "@");
         sprintf(holder3, " %.2f (%.2f)\n", change, comp_f1);
         strcat(holder2, holder3);
         strcat(storage[i], holder2);
      }

   }

   for(int j = 0; j <= i; j++)
   {
      printf("%s", storage[j]);
   }

   return;

}

void graphical_cpu_info(int num_samples, int i, int tdelay)
{
   float sample_1;

   get_sample(&sample_1);
   sleep(tdelay);
   
   float sample_2;
   get_sample(&sample_2);
    
   float cpu_use = ((sample_2 - sample_1) / sample_1) * 100;
   char cpu_line[1024];
   cpu_line[0] = '\0';
   char bars[1024];
   bars[0] = '\0';

   char store_cpu[num_samples][1024];
   //store_cpu[1024][0] = '\0';

   sprintf(store_cpu[i], "%.2f", cpu_use);
   float comp_1 = atof(store_cpu[i]);
   sprintf(cpu_line, "total cpu use = %.2f%%\n", cpu_use);
   printf("%s", cpu_line);

   if(i == 0)
   {
      sprintf(bars, "     ||| %.2f", cpu_use);
   }

   else{

      float comp_2 = atof(store_cpu[i-1]);
      int new_samp = (int)(comp_1 + 0.5);
      int prev_samp = (int)(comp_2 + 0.5);

      int diff = new_samp - prev_samp;
      char str[1024];
      str[0] = '\0';
      strcpy(bars, "     |||");
      if(diff >= 0)
      {
         for(int f = 0; f < diff; f++)
         {
            strcat(bars, "|");
         }

         sprintf(str, " %.2f", cpu_use);

         strcat(bars, str);

         
      }
      else 
      {
            for(int f = 0; f > diff; f--)
            {
                strcat(bars, "|");
            }
      }
   }
   
   char bob[1024][1024];
   strcpy(bob[i], bars);
   for(int k = 0; k <= i; k++)
   {
      printf("%s\n", bob[k]);
   }

   return;
}

void sequential_memory_info(int num_samples, int sec_delay, int i)
{
    struct sysinfo mem;
    sysinfo(&mem);
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    long mem_used = usage.ru_maxrss;
    printf(" Memory usage: %ld kilobytes\n", mem_used); //print memory usage

    printf("---------------------------------------\n");

    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
    
    long int tot_mem = mem.totalram; //get the total usuable main memory size
    long int free_mem = mem.freeram; //get the available memory size

    float phys_tot = mem.totalram; //get the total usuable physical memory size
    float phys_free = mem.freeram; //get the available physical memory size
    float phys_used = tot_mem - free_mem; //get the physical memory usage

    float calc_tot_phys = (float)(phys_tot / 1073741824) * mem.mem_unit; //convert to GB
    float calc_used_phys = (float)(phys_used / 1073741824) * mem.mem_unit; //convert to GB

    float tot_swap = mem.totalswap; //get the total swap space size
    float free_swap = mem.freeswap; //get the swap space still available
    float used_swap = tot_swap - free_swap; //get the used swap space
    float virtual_tot = tot_swap + phys_tot; //get the total virtual memory
    float virtual_used = used_swap + phys_used; //get the virtual memory usage

    float calc_tot_virtual = (virtual_tot / 1073741824) * mem.mem_unit; //convert to GB
    float calc_used_virtual = (virtual_used / 1073741824) * mem.mem_unit; //convert to GB

    for(int j = 0; j <= i; j++)
    {
      printf("\n");
    }

    printf("%.2f / %.2f -- %.2f / %.2f\n", calc_used_phys, calc_tot_phys, calc_used_virtual, calc_tot_virtual);

    return;
}

void get_users()
{
   struct utmp *ut;
   setutent();
   while((ut = getutent()) != NULL) {
      if(ut->ut_type == USER_PROCESS) { //check type of record
         printf("%s      %s (%s)\n", ut->ut_user, ut->ut_line, ut->ut_host); //print username, device name, and hostname
      } 
   }
   endutent();
   return;
}

void print_stats(bool system_flag, bool user_flag, bool graphics_flag, bool sequential_flag, int samples, int delay)
{
   if(system_flag && graphics_flag)
   {
      int num_samples = samples; //default value for samples
      int sec_delay =  delay; //default value for seconds
   
      for(int i = 0; i < num_samples; i++)
      {
         printf("\033[H \033[2J \n"); //clears screen

         graphical_memory_info(num_samples, sec_delay, i);

         printf("---------------------------------------\n");

         int online_prcrs = sysconf(_SC_NPROCESSORS_ONLN);
         printf("Number of Cores: %d\n", online_prcrs);

         graphical_cpu_info(num_samples, i, sec_delay);
         
         sleep(sec_delay);   
      }
      

      printf("---------------------------------------\n");

      system_info();

   }

   else if(system_flag && !sequential_flag)
   {
      int num_samples = samples; //default value for samples
      int sec_delay =  delay; //default value for seconds
   
      for(int i = 0; i < num_samples; i++)
      {
         printf("\033[H \033[2J \n"); //clears screen
         simple_memory_info(num_samples, sec_delay, i);

         printf("---------------------------------------\n");

         int online_prcrs = sysconf(_SC_NPROCESSORS_ONLN);
         printf("Number of Cores: %d\n", online_prcrs);

      
         simple_cpu_info(sec_delay);
         

         sleep(sec_delay);   
      }
      printf("---------------------------------------\n");

      system_info();
   }

   else if(user_flag && graphics_flag)
   {
      int num_samples = samples; //default value for samples
      int sec_delay =  delay; //default value for seconds
   
      for(int i = 0; i < num_samples; i++)
      {
         printf("\033[H \033[2J \n"); //clears screen
         printf("Nbr of samples: %d -- every %d secs\n", num_samples, sec_delay);
         printf("---------------------------------------\n");

         printf("### Sessions/users\n");
      
         get_users();

         printf("---------------------------------------\n");
         sleep(sec_delay);   
      }

      system_info();
   }

   else if(user_flag && !sequential_flag)
   {
      int num_samples = samples; //default value for samples
      int sec_delay =  delay; //default value for seconds
   
      for(int i = 0; i < num_samples; i++)
      {
         printf("\033[H \033[2J \n"); //clears screen
         printf("Nbr of samples: %d -- every %d secs\n", num_samples, sec_delay);
         printf("---------------------------------------\n");

         printf("### Sessions/users\n");
      
         get_users();

         printf("---------------------------------------\n");
         sleep(sec_delay);   
      }

      system_info();
   }

   else if(graphics_flag)
   {
      int num_samples = samples; //default value for samples
      int sec_delay =  delay; //default value for seconds
   
      for(int i = 0; i < num_samples; i++)
      {
         printf("\033[H \033[2J \n"); //clears screen
         //graphical_memory_info(num_samples, sec_delay, i);
         graphical_memory_info(num_samples, sec_delay, i);

         printf("---------------------------------------\n");

         printf("### Sessions/users\n");
      
         get_users();

         printf("---------------------------------------\n");

         int online_prcrs = sysconf(_SC_NPROCESSORS_ONLN);
         printf("Number of Cores: %d\n", online_prcrs);

      
         graphical_cpu_info(num_samples, i, sec_delay);
         

         sleep(sec_delay);   
      }
      

      printf("---------------------------------------\n");

      system_info();
   }

   else if(sequential_flag && system_flag)
   {
      int num_samples = samples; //default value for samples
      int sec_delay =  delay; //default value for seconds
   
      for(int i = 0; i < num_samples; i++)
      {
         printf("\n>>> iteration %d\n", i);

         sequential_memory_info(num_samples, sec_delay, i);

         printf("---------------------------------------\n");

         int online_prcrs = sysconf(_SC_NPROCESSORS_ONLN);
         printf("Number of Cores: %d\n", online_prcrs);

         simple_cpu_info(sec_delay);

         sleep(sec_delay);   
      }

      printf("---------------------------------------\n");

      system_info();     
   }

   else if(sequential_flag && user_flag)
   { 
      int num_samples = samples; //default value for samples
      int sec_delay =  delay; //default value for seconds
   
      for(int i = 0; i < num_samples; i++)
      {
         printf("\n>>> iteration %d\n", i);
         printf("Nbr of samples: %d -- every %d secs\n", num_samples, sec_delay);

         printf("### Sessions/users\n");

         get_users();

         sleep(sec_delay);   
      }

      printf("---------------------------------------\n");

      system_info();
         
   }

   else if(sequential_flag)
   {
      int num_samples = samples; //default value for samples
      int sec_delay =  delay; //default value for seconds
   
      for(int i = 0; i < num_samples; i++)
      {
            printf("\n>>> iteration %d\n", i);

            sequential_memory_info(num_samples, sec_delay, i);

            printf("---------------------------------------\n");

            printf("### Sessions/users\n");

            get_users();

            printf("---------------------------------------\n");

            int online_prcrs = sysconf(_SC_NPROCESSORS_ONLN);
            printf("Number of Cores: %d\n", online_prcrs);

            simple_cpu_info(sec_delay);

            sleep(sec_delay);   
      }

      printf("---------------------------------------\n");

      system_info();
         
   }

   else 
   {
      int num_samples = samples; //default value for samples
      int sec_delay = delay; //default value for seconds
   
      for(int i = 0; i < num_samples; i++)
      {
         printf("\033[H \033[2J \n"); //clears screen
         //graphical_memory_info(num_samples, sec_delay, i);
         simple_memory_info(num_samples, sec_delay, i);

         printf("---------------------------------------\n");

         printf("### Sessions/users\n");

         get_users();

         printf("---------------------------------------\n");

         int online_prcrs = sysconf(_SC_NPROCESSORS_ONLN);
         printf("Number of Cores: %d\n", online_prcrs);

         simple_cpu_info(sec_delay);

         sleep(sec_delay);   
      }

      printf("---------------------------------------\n");

      system_info();
   }

}

int main(int argc, char **argv) {

   bool system_flag = false;
   bool user_flag = false;
   bool graphics_flag = false;
   bool sequential_flag = false;
   bool samples_flag = false;
   bool tdelay_flag = false;
   bool positional = false;
   int num_samples;
   int sec_delay;

    if(argc >= 2)
    {
      
      int arr[2];
      int pos[2];

      for(int i = 1; i < argc; i++) //loop through all command line arguments
      {

         const char s[2] = "=";

         char *arg = NULL;
         arg = strtok(argv[i], s); //splits the string at "=", if not found string remains unchanged


         if(strcmp(arg, "--system") == 0) 
         {
            system_flag = true;
         }

         else if(strcmp(arg, "--user") == 0) 
         {
            user_flag = true;
         }
         
         else if(strcmp(arg, "--graphics") == 0) 
         {
            graphics_flag = true;
         }

         else if(strcmp(arg, "--sequential") == 0) 
         {
            sequential_flag = true;
         }

         else if(strcmp(arg, "--samples") == 0)
         {
            samples_flag = true;
            char *samples = NULL;
            samples = strtok(NULL, "=");
            int num_samples = atoi(samples);
            arr[0] = num_samples;
         }

         else if(strcmp(arg, "--tdelay") == 0)
         {
            tdelay_flag = true;
            char *delay = NULL;
            delay = strtok(NULL, "=");
            int sec_delay = atoi(delay);
            arr[1] = sec_delay;
         }

         else{
            
            positional = true;
            pos[0] = atoi(argv[1]);
            pos[1] = atoi(argv[2]);
         }

      }

      if(tdelay_flag && samples_flag)
      {
         print_stats(system_flag, user_flag, graphics_flag, sequential_flag, arr[0], arr[1]);
      }

      else if(tdelay_flag)
      {
         print_stats(system_flag, user_flag, graphics_flag, sequential_flag, 10, arr[1]);
      }

      else if(samples_flag)
      {
         print_stats(system_flag, user_flag, graphics_flag, sequential_flag, arr[0], 1);
      }

      else if(positional)
      {
         print_stats(system_flag, user_flag, graphics_flag, sequential_flag, pos[0], pos[1]);
      }
      else
      {
         
         print_stats(system_flag, user_flag, graphics_flag, sequential_flag, 10, 1);
      }
    }

    else {
      
      print_stats(system_flag, user_flag, graphics_flag, sequential_flag, 10, 1);
    }

    return 0;
}