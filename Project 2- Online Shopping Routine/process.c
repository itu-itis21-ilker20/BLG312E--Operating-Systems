/* Fatmagül İlker
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //fork, getpid, getppid
#include <sys/wait.h> //wait
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>    // for shm_open() and shm_unlink()
#include <sys/mman.h> // for mmap() and munmap()
#include <pthread.h>


#define SHMSZ  sizeof(pthread_mutex_t)

typedef struct 
	{
		int product_ID;
		double product_Price; //0-200
		int product_Quantity; //1-10
	} Product;
	
typedef struct
	{
		int customer_ID;
		double customer_balance; //0-200
		int ordered_num;
		int* ordered_items_id;
		int* ordered_items_qu;
		int* purchased_items_id;
		int* purchased_items_qu;
		int purchased_num;
	} Customer;
	

Product *products;
Customer *customers;

int num_orders;
int number_of_customer;

void  product_create( int i){

	int quantity= rand() %10 +1;
	double price= rand() %200 + 1;
	products[i].product_Price=price;
	products[i].product_Quantity=quantity;
	products[i].product_ID= i+1;
	}
	

void print_products(int a)
{		
	for(int i=0; i<a; i++){		
		printf("Product ID: %d  Quantity: %d Price:%.2f \n", products[i].product_ID, products[i].product_Quantity, products[i].product_Price);
	}
	
} 
void print_customer(int i){
	printf(" \n Customer%d -------------\n", customers[i].customer_ID);
	printf("Ordered Products \n");
	for(int j=0; j<customers[i].ordered_num; j++){
		printf(" id %d   ", customers[i].ordered_items_id[j]);
		printf(" quantity %d \n", customers[i].ordered_items_qu[j]);
	}
	}
	

void order_product(int cust_id, int product_id, int ordered_qu, int num_products)
	{
	int product_qu= products[product_id-1].product_Quantity;
	int price= products[product_id-1].product_Price;
	int balance= customers[cust_id -1].customer_balance;
	if(product_qu >= ordered_qu){
		if(balance >= (price * ordered_qu)){ //if the quantity is enough, then balance is checked
			printf("\n-----Customer %d Success: (%d,%d) Paid: %d$ for each ------- \n",customers[cust_id -1].customer_ID, product_id, ordered_qu, price);  
			products[product_id-1].product_Quantity= product_qu - ordered_qu;
			printf("Updated products \n");
			print_products(num_products);
			printf("Previous balance of customer %d: %.2f \n",customers[cust_id -1].customer_ID, customers[cust_id -1].customer_balance);
			customers[cust_id-1].customer_balance= balance - price * ordered_qu;
			printf("Updated balance of customer %d : %.2f \n\n",customers[cust_id -1].customer_ID, customers[cust_id -1].customer_balance);
		} else 
			printf("Customer %d Fail: (%d,%d)  Insufficient funds.\n",customers[cust_id -1].customer_ID, product_id, ordered_qu); 
	}
	else {
		printf("Customer %d Fail: (%d,%d) Only %d left in stock \n",customers[cust_id -1].customer_ID, product_id, ordered_qu, product_qu);
	}  		
}
 
void customer_create( int i, int product_num, pthread_mutex_t *mutex, int scenario){
	pthread_mutex_lock(mutex);
	customers[i].customer_ID= i+1;
	srand(time(NULL) + getpid());  // seed with process ID and current time
    double balance = rand() % 200 + 1;
	customers[i].customer_balance=balance;
	int order_num;
	if(scenario==1){ order_num=1; }
	else {order_num= rand () % product_num +1; } //in scenario 2, order num is randomly set 
	customers[i].ordered_num=order_num;
	customers[i].purchased_num=0;
	customers[i].ordered_items_id= malloc(order_num * sizeof(int));
	customers[i].ordered_items_qu= malloc(order_num * sizeof(int)); 
	int k=1;	
	for(int j=0;j<order_num; j++){
		int product_id= rand() % product_num +1;

			for(k=0; k<j; k++){
				if(customers[i].ordered_items_id[k]==product_id){ //if a product is already ordered by the customer, the same customer cannot order it again
					break;
				}
			}
		if (k==j){
			int product_qu= rand() %5 +1;
			customers[i].ordered_items_id[j]=product_id;
			customers[i].ordered_items_qu[j]=product_qu; 
			k++;
			}
		else {
			j=j-1;
			} 
		}


	for(int j=0; j<order_num; j++){
		int cust_id= i+1;
		int prod_id= customers[i].ordered_items_id[j];
		int qu= customers[i].ordered_items_qu[j];
		
		order_product(cust_id, prod_id, qu, product_num);
		}
	
	pthread_mutex_unlock(mutex);	
} 


int main(int argc, char **argv)
{	
	srand(time(NULL));	
	int num_products;
	printf("Enter the number of customer: ");
	scanf("%d",&number_of_customer);
	printf("Enter the number of product: ");
	scanf("%d",&num_products);
	int scenario;
	printf("Which scenario: ");
	scanf("%d",&scenario);
	int shmid;
    key_t key = 5678;
    pthread_mutex_t *mutex;
    shmid = shmget(key, SHMSZ, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }
	size_t size =  num_products *sizeof(Product);
	size_t sizec =  number_of_customer *sizeof(Customer);
	products = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); //products pointer is copied to the shared memory space
	customers = mmap(NULL, sizec, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); //customers pointer is copied to the shared memory space
	mutex = shmat(shmid, NULL, 0); //mutex is copied to the shared memory space
    if (mutex == (pthread_mutex_t *) -1) {
        perror("shmat");
        exit(1);
    }
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(mutex, &attr);
	
	for(int k=0; k<num_products; k++){
		product_create(k);		
	} //all products are created
	printf("All Products Initially \n");
	print_products(num_products);
	
	int pid;
	for (int i = 0; i < number_of_customer; i++) {
	
		pid = fork();   
		if (pid == 0) {
        // child process
			customer_create(i, num_products, mutex, scenario); 	//all processes call the customer_create function 	
			exit(0);
        
		}
		else {
    // parent process
		}	
    }
  
    int status;
    for (int i = 0; i < number_of_customer; i++) {
        pid = wait(&status);
        if (pid < 0) {
            perror("wait");
            exit(1);
        }
    }
	
	pthread_mutex_destroy(mutex);

	return 0;
}
