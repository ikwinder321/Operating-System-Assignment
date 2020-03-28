#include<stdlib.h>
#include<sys/types.h>
#include<stdio.h>
#include<math.h>
#include<unistd.h>
#include<pthread.h>
struct station 
{
int pssngr_waiting;//waiting passengers
int pssngr_on_train;//in train passengers
int boarded_passengers;//boarded passengers
pthread_mutex_t lock_var;
pthread_cond_t pssngr_seated_cond;
};
void station_init (struct station *st);
void station_load_train(struct station *st,int count);
void *station_wait_for_train(struct station *st);
void station_on_board(struct station *st);
void *thread_passenger(void *args);//passenger function
void *threads_on_train(void *args);// train loading function
//initializing the waiting passengers,in train passengers,boareded passengers,lock,condition statements;
void station_init(struct station *st)
{
st->pssngr_waiting = 0;
st->boarded_passengers =0;
st->pssngr_on_train =0;
pthread_mutex_init(&(st->lock_var),NULL);
pthread_cond_init(&(st->pssngr_seated_cond),NULL);
}
struct load_train {
	struct station *st;
	int empty_seats;//empty seats in the coming train
};
void *threads_on_train(void *args)
{
	struct load_train *object = (struct load_train*)args;
	station_load_train(object->st, object->empty_seats);
	return NULL;
}
volatile int thread_completed =0;
//int k=0;
//int l=1;
int n;//waiting passenger
int out_val;
int main()
{
struct station st;
station_init(&st);
int i;
printf("\t\t\t\t\t****************************************\n");
printf("\t\t\t\t\t****WELCOME TO RAILWAY SYSTEM****\n");
printf("\t\t\t\t\t*****************************************\n");
printf("ENTER THE HOW MANY PASSENGERS ARE WAITING AT STATION \n");//no of waiting passengers taken from the user
scanf("%d",&n);
if(n<0){
	printf("enter valid passengers");
	exit(0);
}
//
 int ttl_pssngr =n;//total passenger 
//int  passengers_left = total_passengers;
for(i=0;i<n;i++)
{
pthread_t p;
int val =pthread_create(&p,NULL,thread_passenger,&st);//passenger threads
val=pthread_join(p,NULL);
if(val!=0)
{
perror("pthread_create");
exit(1);
}
}
printf("\nTOTAL PASSENGERS WAITING AT THE STATION ARE****%d\n\n",st.pssngr_waiting);//printing the no of waiting passenger
int k;
REPEAT :do
{
int free_seats;
free_seats=10;
printf("Train entering station with %d empty seats\n", free_seats);
struct load_train args = { &st, free_seats};
pthread_t p2;

int val2= pthread_create(&p2, NULL,threads_on_train, &args);

if (val2 != 0) {
perror("pthread_create");
exit(1);
        }
printf("no of passengers trying to enter into the train  %d\n",(st.pssngr_waiting-st.pssngr_on_train));
station_on_board(&st);
printf("boarded_passengers are %d\n",st.boarded_passengers);
printf(" total passengers updated boarded passengers  are %d,%d\n",ttl_pssngr,st.boarded_passengers);//it shows the updated values of the   boarded passengers
}
while(ttl_pssngr> n);
if(st.boarded_passengers == ttl_pssngr)//checking the whether the enter input passengers are equal to boarded or not
{
printf("all passengers are boarded\n");
}
else
{
if(st.boarded_passengers < n )
{
goto REPEAT;
}
}
}
void *thread_passenger(void *args)
{
struct station *st = (struct station*)args;
station_wait_for_train(st);
thread_completed++;
//printf("thread no %d ",thread_completed);
}
void *station_wait_for_train(struct station *st)
{
pthread_mutex_lock(&(st->lock_var));
st->pssngr_waiting++;
/*here in this function it is adding the waiiting passengers at the station 
which the value is taken from the user
*/
pthread_mutex_unlock(&(st->lock_var));
}
void station_load_train(struct station *st,int count)
{
pthread_mutex_lock(&st->lock_var);
printf("no of empty seats are%d\n",count);
while(count >0 && st->pssngr_waiting  >0)
{
st->pssngr_on_train++;//in triin passengers increases
count--;//empty seats decreasing

}
if(count==0){
//	printf("train is full\n");

}
else{
//	printf("ready to board\n");
}


pthread_cond_wait(&(st->pssngr_seated_cond),&(st->lock_var));
//all the passengers in the train are in seated condition
pthread_mutex_unlock(&(st->lock_var));

}
void station_on_board(struct station *st)
{
pthread_mutex_lock((&st->lock_var));
while(st->pssngr_on_train >0 && st->boarded_passengers < n)
{
pthread_cond_signal((&st->pssngr_seated_cond));
//
st->pssngr_on_train--;//internal passengers decreasing
st->boarded_passengers++;//boarded passen=ngers increasing
}
pthread_cond_broadcast(&st->pssngr_seated_cond);
//unblocking all the passengers fron seated condition to boarded 
pthread_mutex_unlock(&st->lock_var);

}
