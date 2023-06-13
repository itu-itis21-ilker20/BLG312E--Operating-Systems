/* Fatmagül İlker */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>

pthread_mutex_t lock;

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
		double initial_balance;
		int ordered_num;
		int* ordered_items_id;
		int* ordered_items_qu;
		int* purchased_items_id;
		int* purchased_items_qu;
		int purchased_num;
	} Customer ;
	
typedef struct {
		Customer* customer;
		Product* product;
		int product_qu;
	} Order;

Customer *customers;
Product *products;
Order *orders;
int number_of_product;

void* product_create(void *t)
{
	pthread_mutex_lock(&lock); 
	int index= *(int*) t;
	int id= index +1; 
	int quantity= rand() %10 +1;
	double price= rand() %200 + 1;
	products[index].product_ID=id;
	products[index].product_Price=price;
	products[index].product_Quantity=quantity;	
	pthread_mutex_unlock(&lock);
}

void print_products()
{
	for(int i=0; i<number_of_product; i++)
	{
		printf("Product ID: %d  Quantity: %d Price:%.2f \n", products[i].product_ID, products[i].product_Quantity, products[i].product_Price);
	}
} 
	
void* customer_create(void *t){
	pthread_mutex_lock(&lock);
	Customer *c= (Customer*) t;
	double balance= rand() %200 + 1;
	c->customer_balance=balance;
	c->initial_balance=balance;
	int order_num=c->ordered_num;
	c->purchased_num=0;
	c->ordered_items_id= malloc(order_num * sizeof(int));
	c->ordered_items_qu= malloc(order_num * sizeof(int));
	c->purchased_items_id= malloc(order_num * sizeof(int));
	c->purchased_items_qu= malloc(order_num * sizeof(int));
	int k=1;
	for(int j=0;j<order_num; j++){
		int product_id= rand() % number_of_product +1;
			for(k=0; k<j; k++){
				if(c->ordered_items_id[k]==product_id){ //if a product is already ordered by the customer, the same customer cannot order it again
					break;
				}
			}
		if (k==j){
			int product_qu= rand() %5 +1;
			c->ordered_items_id[j]=product_id;
			c->ordered_items_qu[j]=product_qu;
			k++;
			}
		else {
			j=j-1;
			} 
		}
	
	pthread_mutex_unlock(&lock);

	}
	
void print_customer(Customer *arr, int num_cust, int begin){
	for (int i=0; i<num_cust; i++){
	printf(" \n Customer%d -------------\n", arr[i].customer_ID);
	printf(" Customer İnital Balance: %.2f \n", arr[i].initial_balance);
	if(begin==0){
	printf(" Customer Updated Balance: %.2f \n", arr[i].customer_balance);
}
	printf("Ordered Products \n");
	for(int j=0; j<arr[i].ordered_num; j++){
		printf(" id %d   ", arr[i].ordered_items_id[j]);
		printf(" quantity %d \n", arr[i].ordered_items_qu[j]);
		}
	printf("Purchased Products: \n");
	for(int j=0; j<arr[i].purchased_num; j++){
		printf(" id: %d", arr[i].purchased_items_id[j]);
		printf(" quantity %d \n", arr[i].purchased_items_qu[j]);
		} 
		if(arr[i].purchased_num==0){
		printf(" id: - ");
		printf(" quantity - \n");
		}
	}
} 
	
void* order_product(void *t) 
	{
	pthread_mutex_lock(&lock);
	int index= *(int*) t; 
	int product_id= orders[index].product->product_ID;
	int product_qu= orders[index].product->product_Quantity;
	int price= orders[index].product->product_Price;
	int ordered_qu=orders[index].product_qu;
	int balance= orders[index].customer->customer_balance;
	if(product_qu >= ordered_qu)
	{
		if(balance >= (price * ordered_qu)){
					
			int product_size = sizeof(products) / sizeof(products[0]);
			
			orders[index].product->product_Quantity= product_qu - ordered_qu; //quantity is updated after purchase
			orders[index].customer->purchased_items_id[orders[index].customer->purchased_num]=product_id;
			orders[index].customer->purchased_items_qu[orders[index].customer->purchased_num]=ordered_qu;					
			orders[index].customer->purchased_num++;
			printf("\n-----Customer %d Success: (%d,%d) Paid: %d$ for each ------- \n",orders[index].customer->customer_ID, product_id, ordered_qu, price);  
			printf(" Updated products after purchase: \n");
			print_products();
			printf("previous balance of customer %d: %.2f \n",orders[index].customer->customer_ID, orders[index].customer->customer_balance);
			orders[index].customer->customer_balance= balance - price * ordered_qu; //balance is updated after purchase
			printf("updated balance of customer %d : %.2f \n\n",orders[index].customer->customer_ID, orders[index].customer->customer_balance);
					
			}else 
				printf("Customer %d Fail: (%d,%d)  Insufficient funds.\n",orders[index].customer->customer_ID, product_id, ordered_qu);  			
			}
			else 
			{
				printf("Customer %d Fail: (%d,%d) Only %d left in stock \n", orders[index].customer->customer_ID, product_id, ordered_qu, product_qu); 
			} 	
    pthread_mutex_unlock(&lock);
	} 
	

int main(int argc, char **argv)
{
	pthread_mutex_init(&lock, NULL); //initiliaze mutex
	srand(time(NULL)); //random number each time independently

	int number_of_customer;
	printf("Enter the number of customer: ");
	scanf("%d",&number_of_customer);
	printf("Enter the number of product: ");
	scanf("%d",&number_of_product);
	int scenario;
	printf("Which scenario: ");
	scanf("%d",&scenario);
    
	products = malloc(number_of_product * sizeof (*products));
	pthread_t product_threads[number_of_product]; //initiliaze thread array for products
    for (int i = 0; i < number_of_product; i++){
        int* temp_i = (int*) malloc(sizeof(int));
        *temp_i = i;
        if (pthread_create(&product_threads[i], NULL, &product_create, temp_i) != 0){
            perror ("Failed to create a thread.");
        }
    }
    for (int i = 0; i < number_of_product; i++){
        if (pthread_join(product_threads[i], NULL) != 0){
            perror ("Failed to join a thread.");
        }
    } 
	printf("All Products Initially \n");
	print_products();
	customers = malloc(number_of_customer * sizeof (*customers)); 
	pthread_t customer_threads[number_of_customer]; //initiliaze thread array  for customers 
    for (int i = 0; i < number_of_customer; i++){
        int* temp_i = (int*) malloc(sizeof(int));
        *temp_i = i;
        customers[i].customer_ID=i+1; 
        if(scenario==1) customers[i].ordered_num=1;
        else if (scenario==2) customers[i].ordered_num= rand() % number_of_product +1;
        if (pthread_create(&customer_threads[i], NULL, &customer_create, &customers[*temp_i]) != 0){
            perror ("Failed to create a thread.");
        }
    }
    for (int i = 0; i < number_of_customer; i++){
        if (pthread_join(customer_threads[i], NULL) != 0){
            perror ("Failed to join a thread.");
        }
    } 
		printf("\nAll Customers Initially - before purchasing ");
      print_customer(customers, number_of_customer,1);
   
    //Order Products
	int number_of_orders=0;
	if(scenario==1) number_of_orders=number_of_customer; //if scenario 1, order num equal to customer num
	else if(scenario==2){
		    for(int a=0; a<number_of_customer; a++){
		number_of_orders=number_of_orders + customers[a].ordered_num;
		}	
		}

	orders = malloc(number_of_orders * sizeof (*orders));
    int order_index=0;
    for(int a=0;a<number_of_customer;a++){ 
		for(int b=0; b<customers[a].ordered_num; b++){
				orders[order_index].customer=&customers[a];
				orders[order_index].product=&products[customers[a].ordered_items_id[b]-1];
				orders[order_index].product_qu=customers[a].ordered_items_qu[b];
				order_index++;
			}		//order_list is finished
		}
		
	
	printf("\n------Shopping Starts------ \n");
	pthread_t order_threads[number_of_orders];
    for (int i = number_of_orders-1; i >= 0 ; i--){
        int* temp_i = (int*) malloc(sizeof(int));
        *temp_i = i; 
        if (pthread_create(&order_threads[i], NULL, &order_product, temp_i) != 0){ //each order thread goes to order_product function 
            perror ("Failed to create a thread.");
        }
            	
    }

    for (int i = 0; i < number_of_orders; i++){
        if (pthread_join(order_threads[i], NULL) != 0){
            perror ("Failed to join a thread.");
        }
    } 
 
	printf("Updated Customers \n");
    print_customer(customers, number_of_customer,0);

	printf("-----------End of Shopping-------------\n");


	pthread_mutex_destroy(&lock);
	free(orders);
	free(customers);
	free(products); 
	return 0;
}

