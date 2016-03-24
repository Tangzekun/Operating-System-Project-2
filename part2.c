#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdint.h>


#define num_publishers  2
#define num_subscribers 2

int pub_pipe[num_publishers][2];
int sub_pipe[num_subscribers][2];
int atpp_pipe[num_publishers][2];
int atps_pipe[num_subscribers][2];

char ap_buf[num_publishers][128];
char as_buf[num_subscribers][128];

char * pub_message[] = {"pub pubid connect", "pub pubid topic k", "end"}; 
char * sub_message[] = {"sub pubid connect", "sub pubid topic k", "end"};

char  terminate[] = "terminate";
char  accept[] = "accept";
char  reject[]= "reject";




void  *pub_connection(void* arg)
{
		int which_pub = *((int*)(&arg));
		int ap_index = 0;
		for(ap_index = 0; ap_index < (3); ap_index++)/// move to pub_connection
		{
			read(pub_pipe[which_pub][0], ap_buf[which_pub], (128));
			printf("Thread_pub reads message '%s' from pipe.\n", ap_buf[which_pub] );
			if(ap_index == 1)
			{	
				printf("**********This publisher's topic is %c. \n", ap_buf[which_pub][16]);
			}

			write(atpp_pipe[which_pub][1], accept, (strlen(accept)+1));
			printf("Thread_pub writes message '%s' to '%s' publisher.\n", accept,"pub" );
		}
		
		read(pub_pipe[which_pub][0], ap_buf[which_pub], (strlen(terminate)+1));
		printf("Thread_pub reads message '%s' from pipe.\n", ap_buf[which_pub] );
		
		
		if(ap_buf[which_pub] != terminate)
			{
				close(atpp_pipe[which_pub][0]);
				close(atpp_pipe[which_pub][1]);
				close(pub_pipe[which_pub][0]);
				close(pub_pipe[which_pub][1]);	
				printf("Close all pub pipes.\n");
				printf("\n");
			}
			
	return NULL;
}


void  *sub_connection(void* arg)

{	
		int which_sub = *((int*)(&arg));
		int as_index = 0;
		for(as_index = 0; as_index < (3); as_index++)
		{	
			read(sub_pipe[which_sub][0], as_buf[which_sub], (128));
			printf("Thread_sub reads message '%s' from pipe.\n", as_buf[which_sub] );
			if(as_index == 1)
			{
				printf("**********This sublisher's topic is %c. \n", as_buf[which_sub][16]);
			}

			write(atps_pipe[which_sub][1], accept, (strlen(accept)+1));
			printf("Thread_sub writes message '%s' to '%s' subscriber.\n", accept,"sub" );
		}

		read(sub_pipe[which_sub][0], as_buf[which_sub], (128));
		printf("Thread_sub reads message '%s' from pipe.\n", as_buf[which_sub] );

		if(as_buf[which_sub] != terminate)
		{	
			close(atps_pipe[which_sub][0]);
			close(atps_pipe[which_sub][1]);
			close(sub_pipe[which_sub][0]);
			close(sub_pipe[which_sub][1]);	
			printf("Close all subscriber pipes.\n");
			printf("\n");
		}

		return NULL;

}


int main(int argc, char *argv[])
{
	char p_buf[num_publishers][128];
	char s_buf[num_subscribers][128];


    printf("%d publishers and %d subscribers .\n", num_publishers, num_subscribers);
	printf("\n");
	
    pid_t pub_pids[num_publishers];
    pid_t sub_pids[num_subscribers];
	pid_t atp_pids;
	pid_t wait_pid;
	
	int status;

	
	pthread_t pub_thread[num_publishers];
	pthread_t sub_thread[num_subscribers];
	
	
	int p_index = 0;
	int s_index = 0;
	
	int atp_pub_pipe[num_publishers];
    int pub_atp_pipe[num_publishers];
    int atp_sub_pipe[num_subscribers];
    int sub_atp_pipe[num_subscribers];
    
    int pipe_num;
    
    for (pipe_num = 0 ; pipe_num < num_publishers; pipe_num ++){
   	 
   	 atp_pub_pipe[pipe_num] = pipe(atpp_pipe[pipe_num]);
   	 pub_atp_pipe[pipe_num] = pipe(pub_pipe[pipe_num]);
   	 
   	 if (atp_pub_pipe[pipe_num] < 0 || pub_atp_pipe[pipe_num] < 0){
   		 perror("pipe");
   		 exit(1);
   	 }
   	 
    }
    
    for (pipe_num = 0 ; pipe_num < num_subscribers; pipe_num ++){
   	 
   	 atp_sub_pipe[pipe_num] = pipe(atps_pipe[pipe_num]);
   	 sub_atp_pipe[pipe_num] = pipe(sub_pipe[pipe_num]);
   	 
   	 if (atp_sub_pipe[pipe_num] < 0 || sub_atp_pipe[pipe_num] < 0){
   		 perror("pipe");
   		 exit(1);
   	 }
    }

	

	
////// Publisher///////////////////////
	
	int which_pub = 0;
    for (which_pub = 0; which_pub < num_publishers; which_pub++) 
    {	
		
        pub_pids[which_pub] = fork();  		

		
		if (pub_pids[which_pub] == -1) /* error */
		{
		   perror("fork");
		   exit(1);
		}
						
		else if (pub_pids[which_pub] == 0)  /* child */
		{	
			char pub_topic[50];
			char ran_str[50];
			srand(getpid());
			int random_num = rand()%9 + 1;
			sprintf(ran_str,"%d",random_num);
			strcpy(pub_topic,"pub pubid topic " );
			strcat(pub_topic, ran_str);		
			pub_message[1] = pub_topic;
			
			char pid_id[100];
			char first_seg[100];
			sprintf(pid_id, "%d", getpid());
			strcpy(first_seg, "pub ");
			strcat(first_seg, pid_id);
			strcat(first_seg, " connect");
			pub_message[0] = first_seg;
			
			
			for (p_index = 0; p_index< 3; p_index++) 
			{	
				write(pub_pipe[which_pub][1], pub_message[p_index], (strlen(pub_message[p_index])+1));
				printf("Publisher writes message '%s' to Pub_thread.\n", pub_message[p_index] ); 
				read(atpp_pipe[which_pub][0], p_buf[which_pub], (128));
				printf("Pubsriber reads message '%s' from Pub_thread.\n", accept );
				
				if(p_buf[which_pub]==reject)
				{
					exit(1);
				}
				
			}
			
			write(pub_pipe[which_pub][1], terminate, (strlen(terminate)+1));
			printf("Pubsriber writes message '%s' to Pub_thread.\n", terminate );
			
			return(1);		
		} 
		
		
		else 
		{
			
		}
		
    }
	



////// Subcriber/////////////
	
	int which_sub = 0;
    for (which_sub=0; which_sub < num_subscribers; which_sub++) 
    {
        
        sub_pids[which_sub] = fork();  		
		
		if (sub_pids[which_sub] == -1) /* error */
		{
		   
		   perror("fork");
		   exit(1);
		}
		
		

		else if (sub_pids[which_sub] == 0)  /* child */
		{		
			char sub_topic[50];
			char ran_str[50];
			srand(getpid());
			int random_num = rand()%8 + 1;
			sprintf(ran_str,"%d",random_num);
			strcpy(sub_topic,"sub pubid topic " );
			strcat(sub_topic, ran_str);			
			sub_message[1] = sub_topic;
			
			char pid_id[100];
			char first_seg[100];
			sprintf(pid_id, "%d", getpid());
			strcpy(first_seg, "sub ");
			strcat(first_seg, pid_id);
			strcat(first_seg, " connect");
			sub_message[0] = first_seg;
			
			
			for (s_index=0; s_index< 3; s_index++) 
			{
				write(sub_pipe[which_sub][1], sub_message[s_index], (strlen(sub_message[s_index])+1));
				printf("Subsriber writes message '%s' to Sub_thread.\n", sub_message[s_index] );
				read(atps_pipe[which_sub][0], s_buf[which_sub], (128));
				printf("Subsriber reads message '%s' from Sub_thread.\n", accept );
				
				if(s_buf[which_sub]==reject)
				{
					exit(1);
				}
			}
			
			write(sub_pipe[which_sub][1], terminate, (strlen(terminate)+1));
			printf("Subsriber writes message '%s' to Sub_thread.\n", terminate );
			
			return(1);
		} 
				
		else 
		{
			
		}
		
     
    }


	
///////ATP Server	
	
	atp_pids = fork();
	
	if (atp_pids == -1) /* error */
	{
	   perror("fork");
	   exit(1);
	}
	
	else if (atp_pids == 0)
	{	
		//// connect with publisher
		for (which_pub = 0; which_pub < num_publishers; which_pub++) 
		{	

			printf("This publisher's PID is %d. \n", pub_pids[which_pub]);
			pthread_create(&pub_thread[which_pub], NULL, pub_connection, (void*)(intptr_t) which_pub);
			
			
		}
		
		
		
		for (which_pub = 0; which_pub < num_publishers; which_pub++) 
		{
			pthread_join(pub_thread[which_pub], NULL);
		}
		
				
		
		
		
		
		
		
		
///// connect with subscriber
		
		
			
		for (which_sub=0; which_sub < num_subscribers; which_sub++) 
		{	
			printf("This subscriber's PID is %d. \n", sub_pids[which_sub]);
			pthread_create(&sub_thread[which_sub], NULL, sub_connection, (void*)(intptr_t) which_sub);
			
		}
		
		
		for (which_sub=0; which_sub < num_subscribers; which_sub++) 
		{
			pthread_join(sub_thread[which_sub], NULL);
		}
		
		
		
	}
	
		
	else 
	{
			
	}
	
	while((wait_pid = wait(&status))>0)
	{
		///
	}
	
    return 0;

}
