Part 1:
	My application satisfies all requirement. 
	Each publisher run the fork() one time, 
	and connects with ATP by a pipe.
	
	Each subscriber run the fork() one time,
	and connects with ATP by a pipe.
	  
	But ATP only run fork() one time, 
	and connects with both publishers 
	and subscribers by two other pipes.
	
	Each publisher and subscriber generates their 
	own random number by using their own getpid().
	
	Both publishers and subscribers writes messages through 
	pipe within a for loop. And then ATP read their messages,
	send accept or reject back to publishers or subscribers.
	If publishers or subscribers get the accept, then they 
	write the next message. Once the ATP reads the terminate 
	command from either publisher or subscriber, it closes pipes.
	
	Finally, the parent process will wait all child process done.
	
	


Part2:
	In the ATP, for publishers and subscribers, there are two for
	loops to pthread_create and pthread_join. In the pthread_create,
	there are two functions,pub_connection and sub_connection each of them 
	is used as thread for 
	communicating publishers or subscribers with ATP.
	Inside of these two functions, the threads through pipes to 
	read message from publishers or subscribers and	write back information.
	
	
	
Part3:
	I created dequeue, enqueue, and several functions to test a queue.
	There are 2 kinds of structs, one is for queues and other one is for
	topic entry. 2 types queues, one is topic_queue, it has a max size to 
	store entries; other type is wait_queue, it has a size as many as the 
	input publishers. Both them have a same size array.
	My architecture is no mather what, all same topic entries would go
	through the wait_queue firstly, then been enqueued to the topic_queue.
	Each time, before dequeuing, the application check if the topic_queue is 
	full or not, and is there any entry in wait_queue can be enqueued 
	to topci_queue. 
	
	It used mutex lock to synchronize the dequeue and enqueue. 
	
	
	
	
	 