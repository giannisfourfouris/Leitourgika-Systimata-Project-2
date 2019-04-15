#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include"p3150190_p3150102_p3150224_res1.h"
#include <pthread.h>

int Ncust;
int seed;
int account=0;
int counterTransaction=0;
int *seatArray;
//krataei an apetyxe h oxi ena thread
int rc;
int *threadId;
int *standByTime;
int *handlingTime;
int counterSeat=0;
struct timespec startStandBy, stopStandBy;
struct timespec startHandlingTime, stopHandlingTime;
pthread_mutex_t lock;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t condForCash = PTHREAD_COND_INITIALIZER;
 


/**
 * H synartisi pou tha kaleite apo ta threads gia na 
 * vlepei kai na apothikevei theseis, ypologisei arithmo sunalagwn kai
 * prosthetei sto tamio ta leftta
*/
 void *customer(void *x){
 	int id = (int *)x;
	//mutex 1: koitaei gia diarthesimous thlefwnhtes
 	rc = pthread_mutex_lock(&lock);
	if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_lock is %d\n", rc);
			exit(-1);		
		}
	//perimenei mexri na vrei diathesimo thlefwnhti
 	while (Ntel == 0) {
 		printf("Customer %d, didnt find avaliable Phone Operator.Blocked...\n", id);
 		rc = pthread_cond_wait(&cond, &lock);
		if (rc != 0) {
			printf("ERROR: return code from pthread_cond_wait is %d\n", rc);
			exit(-1);		
		}
 	}
 	printf("Costumer %d, is being served.\n",id);
 	//meiwnei to thlefwnhth kata 1
	Ntel--;

	rc = pthread_mutex_unlock(&lock);//telos mutex 1
	if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_unlock is %d\n", rc);
			exit(-1);		
		}
     	//mutex 2: upologismo xronou anamonis
 	rc = pthread_mutex_lock(&lock);
	if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_lock is %d\n", rc);
			exit(-1);		
		}
	//teleiwnei to xrono anamonis	
	if( clock_gettime( CLOCK_REALTIME, &stopStandBy) == -1 ) {
      		perror( "clock gettime" );
      		exit( EXIT_FAILURE );
    	}
	//ypologizwi to xrono anamonis kai ton apothikevei sto pinaka StandBy
        standByTime[id-1] = ( stopStandBy.tv_sec - startStandBy.tv_sec );
       
	rc = pthread_mutex_unlock(&lock);//telos mutex2 
	if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_unlock is %d\n", rc);
			exit(-1);		
		}
	
	//epilegei tuxaio xrono gia sleep
 	int randomWaitTime;
	int rand=rand_r(&seed)%tseathigh;
	if(rand>=5){
		randomWaitTime=rand;
  
         }else{
		randomWaitTime=rand +tseatlow;	
	}
 	

 	sleep(randomWaitTime); //perimenei gia tyxaio xrono apo 5-10 sec

	//mutex 3:elegxos an exei arketes kenes theseis
	rc = pthread_mutex_lock(&lock);
	//epilegei tuxaio arithmo thesewn
 	int randomTicketNumber=rand_r(&seed)%Nseathigh +Nseatlow;	
	sleep(1);
	if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_lock is %d\n", rc);
			exit(-1);		
	}
        int checkIfSeatReserved=0;
 	int lastSeat=(counterSeat+randomTicketNumber);
	for(int i=counterSeat; i<lastSeat;i++){
		if(counterSeat>Nseat){
			//elegxw an to theatro einai gemato 
			printf(" Customer ID: %d Reservation canceled.The theater is full.\n",id);
			break;	
		}else if(Nseat-counterSeat<randomTicketNumber){ 
			//elegxw an exei arketes theseis
			printf("Customer ID: %d Reservation canceled because there are not enough available seats\n.",id);
			break;
		}else{ 
			//desmevw theseis
			seatArray[i]=id;
			//printf("Eimai to seatArray i: %d kai eimai piasmenos apo to pelati %d \n",i,seatArray[i]);
			checkIfSeatReserved=1;	
			
		}
 	}
	rc = pthread_mutex_unlock(&lock);//telos mutex 3
	if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_unlock is %d\n", rc);
			exit(-1);		
		}
	//an desmefthkan theseis tote proxwraw sthn plhrwmh
	//teleiwnw me to searching kai to kleisimo twn thesewn
 	rc = pthread_mutex_lock(&lock); //mutex 4: se periptwsh pou exei theseis enhmwrwsh account, arithmo sunalagis kai gia tipoma
	if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_lock is %d\n", rc);
			exit(-1);		
		}

	//perimenei mexri na vrei diathesimo tamis
 	while (Ncash == 0) {
 		printf("Customer %d, didnt find avaliable cashier .Blocked...\n", id);
 		rc = pthread_cond_wait(&condForCash, &lock);
		if (rc != 0) {
			printf("ERROR: return code from pthread_cond_wait is %d\n", rc);
			exit(-1);		
		}
 	}
 	printf("Costumer %d, is being served from cashier.\n",id);
 	//meiwnei to cashier kata 1
	Ncash--;

	
	//an exei desmeftei h thesh
	if(checkIfSeatReserved==1){
		//dhmiourgei tuxaious arithmous metaksi 1-10.	
		//an aftos o arithmos<=9 tote kanei accept
		//alliws kanei reject
		int randomPercentage=rand_r(&seed)%10+1;
		//an einai megalytero toy Pcardsuccess kanw reject
		if(randomPercentage>Pcardsuccess){
			//elegxw an to h plhrwmh egine apodekth. An den egine apodekth tote ksedesmevw tis theseis.
			printf("Customer ID: %d Transaction rejected because the credit card was declined.\n",id);
			for(int i=lastSeat-1; i>=counterSeat;i--){
				seatArray[i]=0;
			}
			
				
		}else{
			//enimerwsi arithmou sinallagis
			counterTransaction++;
			printf("Customer ID: %d Successfuly Reserved. Your transaction id is %d, your seat numbers are: ",id,counterTransaction);
			for(int i=counterSeat; i<lastSeat;i++){
				printf("%d,  ",i+1);
			}
			printf("and the total amount is %d.\n",Cseat*randomTicketNumber);
			//enimerwsi tou logariasmou
			account= account + (Cseat*randomTicketNumber);
			//enhmerwnw to counterSeat gia na kserw meta apo poia thesei toy pinaka exw kenes theseis
			counterSeat=lastSeat;
			//se periptwsh pou einai gemato to theatro auksanw to counterSeat 
			//wste sthn epomenh epanalhpsh na kserei oti einai gemato
			if(counterSeat==Nseat){
				counterSeat=251;
			}
	
		}

	}
	rc = pthread_mutex_unlock(&lock);//telos mutex 4
	if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_unlock is %d\n", rc);
			exit(-1);		
		}
	
	

	//epilegei tuxaio xrono gia sleep gia cashier
 	int randomWaitTimeCash;
	int randCash=rand_r(&seed)%tcashhigh;
	if(randCash>=tcashlow){
		randomWaitTimeCash=randCash;
  
         }else{
		randomWaitTimeCash=tcashlow +randCash;	
	}
 	

 	sleep(randomWaitTimeCash); //perimenei gia tyxaio xrono apo 2-4sec
	printf("Tha perimenw %d sec apo cashier\n",randomWaitTimeCash);
	
	rc = pthread_mutex_lock(&lock); //mutex 5 gia swsth enhmerwsh Ntel kai Ncash
	if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_lock is %d\n", rc);
			exit(-1);		
		}
	
 	printf("Customer %d successfuly served! \n", id);
 	
	Ncash++;
	rc = pthread_cond_signal(&condForCash); //stelnw to signal gia na parei kapoio allo thread seira gia cashier
	if (rc != 0) {
			printf("ERROR: return code from pthread_cond_signal is %d\n", rc);
			exit(-1);		
		}
	
	Ntel++;
 	rc = pthread_cond_signal(&cond); //stelnw to signal gia na parei kapoio allo thread seira
	if (rc != 0) {
			printf("ERROR: return code from pthread_cond_signal is %d\n", rc);
			exit(-1);		
		}
	rc = pthread_mutex_unlock(&lock);//telos mutex 5
	if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_unlock is %d\n", rc);
			exit(-1);		
		}


	


	rc = pthread_mutex_lock(&lock); //mutex 6 gia handling time
	if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_lock is %d\n", rc);
			exit(-1);		
		}

	//ipologismos handling time
	if( clock_gettime( CLOCK_REALTIME, &stopHandlingTime) == -1 ) {
      		perror( "clock gettime" );
      		exit( EXIT_FAILURE );
	}
	handlingTime[id-1] = ( stopHandlingTime.tv_sec - startHandlingTime.tv_sec );
      
    	

	rc = pthread_mutex_unlock(&lock);//telos mutex 6
	if (rc != 0) {
			printf("ERROR: return code from pthread_mutex_unlock is %d\n", rc);
			exit(-1);		
		}
	/*aparaitito gia na gnwrizei o pateras oti to thread termatise swsta*/
 	pthread_exit(id); //return
 }




int main(int argc, char** argv) {
	//dhmioyrgia pinaka pthread      
	pthread_t *threads;


    //treia arguments (to prwto orisma einai to onoma tou programmatos to deftero einai
    // to Ncust kai to trito einai to seed).
	if (argc != 3) {
		printf("ERROR: Provide two arguments.\n");
		return -1;
	}
      
     Ncust=atoi(argv[1]);
     seed=atoi(argv[2]); 
     printf("Num of costumers: %d Seed : %d\n", Ncust,seed);

	//xrisi malloc gia dimiourgia tou pinaka me megethos 250 gia tis theseis.
	//desmeuoume sizeof(int) * 250 sti mnimi.
	seatArray = (int *) malloc(sizeof(int) * 250);

	//elegxos an apetyxe i malloc
	if (seatArray == NULL) {
		printf("ERROR: Malloc failed not enough memory!\n");
		return -1;
	}

   	//arxikopoihsh pinaka 250 thesewn
	for (int i = 0; i < 250; i++) {
		seatArray[i] =0;
	}
       
	//desmeysh mnhmhs gia ta threads
        threads = malloc( Ncust* sizeof(pthread_t));
	if (threads == NULL) {
		printf("NOT ENOUGH MEMORY!\n");
		return -1;
	}

      
   	
        //xrisi malloc gia dimiourgia tou pinaka me megethos Ncust gia to id.
	//desmeuoume sizeof(int) * Ncust sti mnimi.
	threadId = (int *) malloc(sizeof(int) * Ncust);

	//elegxos an apetyxe i malloc
	if (threadId == NULL) {
		printf("ERROR: Malloc failed not enough memory!\n");
		return -1;
	}

	//xrisi malloc gia dimiourgia tou pinaka me megethos Ncust gia xrono anamonis.
	//desmeuoume sizeof(int) * Ncust sti mnimi.
	standByTime = (int *) malloc(sizeof(int) * Ncust);

	//elegxos an apetyxe i malloc
	if (standByTime == NULL) {
		printf("ERROR: Malloc failed not enough memory!\n");
		return -1;
	}

	//xrisi malloc gia dimiourgia tou pinaka me megethos Ncust gia xrono eksuphrethshs.
	//desmeuoume sizeof(int) * Ncust sti mnimi.
	handlingTime = (int *) malloc(sizeof(int) * Ncust);

	//elegxos an apetyxe i malloc
	if (handlingTime == NULL) {
		printf("ERROR: Malloc failed not enough memory!\n");
		return -1;
	}

        //arxikopoihsh tou mutex lock
        pthread_mutex_init(&lock, NULL);

	//Arxikopoihsh threads
   	for(int threadCount = 0; threadCount < Ncust; threadCount++) {
    		
		//arxikopoihsh threadId
		threadId[threadCount] = threadCount + 1;
		//status gia create threads
		rc=pthread_create(&threads[threadCount],NULL,customer,threadId[threadCount]); 
		/*elegxos oti to thread dimiourgithike swsta.*/
    		if (rc != 0) {
    			printf("ERROR: return code from pthread_create() is %d\n", rc);
       			exit(-1);
       		}
		//arxikopoihsh xronou anamonis ana pelati kai xronoy kai xronou eksipiretisis ana pelati
		handlingTime[threadCount]=0;
		standByTime[threadCount]=0;
		
		//ksekinaw kai metraw thn wra anamonis
       		if( clock_gettime( CLOCK_REALTIME, &startStandBy) == -1 ) {
    		  perror( "clock gettime" );
     		 exit( EXIT_FAILURE );
   		 }

		//ksekinaw kai metraw thn wra eksyphrethshs
       		if( clock_gettime( CLOCK_REALTIME, &startHandlingTime) == -1 ) {
    		  perror( "clock gettime" );
     		 exit( EXIT_FAILURE );
   		 }
		
    	}

       	//ksekinima olwn twn threads me xrhsh join
	for (int threadCount = 0; threadCount < Ncust; threadCount++) {
		//status gia join
		rc = pthread_join(threads[threadCount], NULL);
		
		if (rc != 0) {
			printf("ERROR: return code from pthread_join() is %d\n", rc);
			exit(-1);		
		}

	}
        printf("Main: All threads successfuly return!\n");
	//emfanisi thesis kai pelati
	//an to counter einai 251 dhladg olos o pinakas gematos
	//tote to epanaferw 250 gia na einai entaksei to tupwma
	if(counterSeat==251){
		counterSeat=Nseat;
	}
	for(int i=0;i<counterSeat;i++){
		
		printf("Seat no%d - Customer id: %d \n",i+1,seatArray[i]);
	}
	//arxikopoihsh duo metavlhtwn gia ypologismo mesoy xronou anamonis kai eksyphrethshs
	double sumStandBy=0;
	double sumHandling=0;
	//athroizw olous toys xronoys
	for(int i=0;i<Ncust;i++){
		sumStandBy=sumStandBy+ standByTime[i];
		sumHandling= sumHandling + handlingTime[i];
	}
	
	printf("Total Profit : %d$\n",account);
	printf("Average Stand By Time Per Customer: %lf sec\n",sumStandBy/Ncust);
	printf("Average Handling Time Per Customer: %lf sec\n",sumHandling/Ncust);




    //panta eletherwnoume ti mnimi pou exoume desmeusei.
    free(seatArray);
    free(threads);
    free(threadId);
    free(handlingTime);
    free(standByTime); 
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
    pthread_cond_destroy(&condForCash);

return 0;

//todo:  percentages,
}


