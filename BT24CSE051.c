# include <stdio.h>
# include <string.h>
# include <time.h>
# include <stdlib.h>
# include <strings.h>
# define MAX_SKUS 3000
# define MAX_ORDS 5000
# define MAX_ITEMS 20
# define status_placed 0
# define status_delivered 1
# define status_cancelled 2
static int orderCounter = 1;
struct sku {
    int skuid;
    char name[50];
    char category[30];
    float price;    
    int stock;
    int soldCount;
};

struct Order {
    int orderid;
    long long orderTime;
    int status;
    long long deliveryTime;
    int itemIds[MAX_ITEMS];
    int itemQty[MAX_ITEMS];
    int Count;
    float subtotal;
};

void resetSku(struct sku skulist[], int size);
int findSkuById(struct sku skulist[], int size, int skuid);
int findEmptySkuSlot(struct sku skulist[], int size);
void addSku(struct sku skulist[]);
void searchSkuByName(struct sku skulist[], int size, const char name[]);
void update_Delete(struct sku skulist[],int size,int skuid); 
void placeOrder(struct sku skulist[],int skusize, struct Order orderlist[],int ordersize );
long long getCurrentDateTimeInt();
static int readIntSafe(const char *prompt);
long long addMinutesToDateTimeInt(long long datetime, int minutesToAdd);
void resetOrder(struct Order orderlist[], int size);
int findorder(struct Order orderlist[],int size,int orderid);
int totalsoldCount(struct sku skulist[],int size);
void DeliverOrder(struct sku skulist[],int skusize, struct Order orderlist[],int ordersize);
void CancelOrder(struct sku skulist[],int skusize, struct Order orderlist[],int ordersize);
int importSkusFromCSV(struct sku skulist[], int size);
void exportSkusToCSV(struct sku skulist[], int size);  
void SortSkusbySoldCount(struct sku skulist[],int size,int choice);
void exportOrderdsToCSV(struct Order orderlist[],int size);
void OrderDetails(struct Order orderlist[],int size,int Id);
void SortOrdersbyTime(struct Order orderlist[],int size);
void SkuMenu();
void OrderMenu();
void SortandABCMenu();
void ExportMenu();
void mainmenu();

struct sku skulist[MAX_SKUS];

struct Order orderlist[MAX_ORDS];

//////////////////////////// get current date time as integer /////////////////////////////////////
long long getCurrentDateTimeInt() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    long long result = (t->tm_year + 1900) * 100000000LL +
                       (t->tm_mon + 1) * 1000000LL +
                       t->tm_mday * 10000LL +
                       t->tm_hour * 100LL +
                       t->tm_min;

    return result; 
}

//////////////////////// add mins to current time /////////////////////////////////////
long long addMinutesToDateTimeInt(long long datetime, int minutesToAdd) {
    struct tm t={0};
    t.tm_year = (datetime / 100000000LL) - 1900;
    t.tm_mon = ((datetime / 1000000LL) % 100) - 1;
    t.tm_mday = (datetime / 10000LL) % 100;
    t.tm_hour = (datetime / 100LL) % 100;
    t.tm_min = datetime % 100;
    t.tm_sec = 0;
    t.tm_isdst = -1; 
    time_t base= mktime(&t);
    base += minutesToAdd * 60; 
    struct tm *newTime = localtime(&base);
    long long result = (newTime->tm_year + 1900) * 100000000LL +
                       (newTime->tm_mon + 1) * 1000000LL +
                       newTime->tm_mday * 10000LL +
                       newTime->tm_hour * 100LL +
                       newTime->tm_min;
    return result;
}    

int totalsoldCount(struct sku skulist[],int size){
    int total=0;
    int limit=findEmptySkuSlot(skulist,size);
    if(limit==-1) limit=MAX_SKUS;
    for(int i=0;i<limit;i++){
        total+=skulist[i].soldCount;
    }
    return total;
}

////////////////////////// QuickSort helper functions ///////////////
int compareSku(int a, int b, struct sku skulist[]) {
    if (skulist[a].soldCount > skulist[b].soldCount) return 1;
    if (skulist[a].soldCount < skulist[b].soldCount) return 0;
    return strcmp(skulist[a].name, skulist[b].name) < 0;
}

void swapInt(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

int partition(int index[], int low, int high, struct sku skulist[]) {
    int pivot = index[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (compareSku(index[j], pivot, skulist)) {
            i++;
            swapInt(&index[i], &index[j]);
        }
    }
    swapInt(&index[i + 1], &index[high]);
    return i + 1;
}

void quickSort(int index[], int low, int high, struct sku skulist[]) {
    if (low < high) {
        int pi = partition(index, low, high, skulist);
        quickSort(index, low, pi - 1, skulist);
        quickSort(index, pi + 1, high, skulist);
    }
}

///////////////////////// for readind int only////////////////////////////
static int readIntSafe(const char *prompt) {
    int x, ok = 0;
    int c;
    while (!ok) {
        if (prompt) printf("%s", prompt);
        if (scanf("%d", &x) != 1) {
            printf("Invalid input! Enter a number.\n");
            // clear the bad input from buffer
            do { c = getchar(); } while (c != '\n' && c != EOF);
        } else {
            ok = 1;
        }
    }
    return x;
}
//////////////////////////////// reset sku list /////////////////////////////////////
void resetSku(struct sku skulist[], int size) {
    for (int i = 0; i < size; i++) {
        skulist[i].skuid = -1;
        skulist[i].name[0] = '\0';
        skulist[i].category[0] = '\0';
        skulist[i].price = -1.0;
        skulist[i].stock = -1;
        skulist[i].soldCount = 0;
    }
}

//////////////////////// reset order list /////////////////////////////////////
void resetOrder(struct Order orderlist[], int size) {
    for (int i = 0; i < size; i++) {
        orderlist[i].orderid = -1;
        orderlist[i].orderTime = -1;
        orderlist[i].status = -1;
        orderlist[i].deliveryTime = 0;
        orderlist[i].Count = 0;
        orderlist[i].subtotal = -1.0;
        for (int j = 0; j < MAX_ITEMS; j++) {
            orderlist[i].itemIds[j] = -1;
            orderlist[i].itemQty[j] = -1;
        }
    }
}    
//////////////////////////////////////// find sku by id /////////////////////////////////////
int findSkuById(struct sku skulist[], int size, int skuid) {
    for (int i = 0; i < size; i++) {
        if (skulist[i].skuid == skuid) {
            return i;
        }
    }
    return -1;
}
///////////////////////////////// find order by id/////////////////////////////
int findorder(struct Order orderlist[],int size,int orderid){
    for(int i=0;i<size;i++){
        if(orderlist[i].orderid==orderid){
            return i;
        }
    }
    return -1;
}
///////////////////////////// find empty sku slot /////////////////////////////////////
int findEmptySkuSlot(struct sku skulist[], int size) {
    for (int i = 0; i < size; i++) {
        if (skulist[i].skuid == -1) {
            return i;
        }
    }
    return -1;
}

/////////////////////////////////////////////////// addition of new SKU ///////////////////////////////////////////////
void addSku(struct sku skulist[]) {
    int running = 1;

    while (running) {
        int empty = findEmptySkuSlot(skulist, MAX_SKUS);
        if (empty == -1) {
            printf("No empty slot available to add new SKU.\n");
            return;
        }

        int skuid, valid = 0;
        while (!valid) {
            printf("Enter SKU ID: ");
            if (scanf("%d", &skuid) != 1) {
                printf("Invalid input! Numbers only.\n");
                while (getchar() != '\n');
            } else if (findSkuById(skulist, MAX_SKUS, skuid) != -1) {
                printf("SKU with ID %d already exists. Try another.\n", skuid);
            } else {
                valid = 1;
            }
        }

    char name[50], category[30];

    while (getchar() != '\n');  

    printf("Enter SKU Name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';   

    printf("Enter SKU Category: ");
    fgets(category, sizeof(category), stdin);
    category[strcspn(category, "\n")] = '\0';

        float price;
        valid = 0;
        while (!valid) {
            printf("Enter SKU Price: ");
            if (scanf("%f", &price) != 1 || price <=0) {
                printf("Invalid price! Try again.\n");
                while (getchar() != '\n');
            } else {
                valid = 1;
            }
        }

        int stock = -1;
        while (stock < 0) {
            stock = readIntSafe("Enter SKU Stock: ");
            if (stock < 0) {
                printf("Stock cannot be negative. Try again.\n");
            }
        }

        skulist[empty].skuid = skuid;
        strcpy(skulist[empty].name, name);
        strcpy(skulist[empty].category, category);
        skulist[empty].price = price;
        skulist[empty].stock = stock;
        skulist[empty].soldCount = 0;

        printf("SKU %d added successfully.\n", skuid);

        int again = -1;
        while (again != 0 && again != 1) {
            printf("Add another? (1 = Yes, 0 = No): ");
            scanf("%d", &again);
        }

        if (again == 0) {
            running = 0;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////// search sku by name /////////////////////////////////////
void searchSkuByName(struct sku skulist[], int size, const char name[]) {
    int found = 0;
    for (int i = 0; i < size; i++) {
        if (strcasecmp(skulist[i].name, name) == 0) {
            printf("SKU Found: ID=%d\n", skulist[i].skuid);
            found = 1;
        }
    }
    if (!found) {
        printf("No SKU found with name: %s\n", name);
    }
}




//////////////////// Update or delete Sku /////////////////////////////////////////////////

///// check delete 

void update_Delete(struct sku skulist[],int size,int skuid){

    int find=findSkuById(skulist,MAX_SKUS,skuid);
    if(find ==-1){
        printf("Enter a valid skuId: \n");

    }
    else{
        printf("Enter 1 for Update\n");
        printf("Enter 2 for Delete\n");
        int choice;
        scanf("%d",&choice);
        if(choice==1){
            char Newname[50];
            char Newcategory[30];
            float Newprice;
            int Newstock;

            printf("Enter New name , '/' for skip ");
            scanf("%s",Newname);
            printf("Enter New category , '/' for skip ");
            scanf("%s",Newcategory);
            printf("Enter New price, -1 for skip ");
            scanf("%f",&Newprice);
            printf("Enter new total stock,-1 for skip ");
            scanf("%d",&Newstock);

            if(strcmp("/",Newname)!=0) strcpy(skulist[find].name,Newname);
            if(strcmp("/",Newcategory)!=0) strcpy(skulist[find].category,Newcategory);
            if(Newprice!=-1) skulist[find].price=Newprice;
            if(Newstock!=-1) skulist[find].stock=Newstock; 
        }
        else if(choice==2){
            skulist[find].skuid = -1;
            skulist[find].name[0] = '\0';
            skulist[find].category[0] = '\0';
            skulist[find].price = -1.0;
            skulist[find].stock = -1;
            skulist[find].soldCount = 0;

            // shift elements to fill the gap
            for (int i = find; i < size - 1; i++) {
                skulist[i] = skulist[i + 1]; 
            }
            // clear the last element
            skulist[size - 1].skuid = -1;
            skulist[size - 1].name[0] = '\0';
            skulist[size - 1].category[0] = '\0';
            skulist[size - 1].price = -1.0;
            skulist[size - 1].stock = -1;
            skulist[size - 1].soldCount = 0;    
        }
        else{
            printf("Invalid choice");
        }    
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////// Place Order /////////////////////////////////////////////////
void placeOrder(struct sku skulist[],int skusize, struct Order orderlist[],int ordersize ) {
    int t_itemids;
    int t_quantity;
    float totalprice=0;
    int totaltypes;
    int temp;
    totaltypes = readIntSafe("Enter number of different items in the order: ");
    if (totaltypes > MAX_ITEMS || totaltypes <= 0) {
        printf("Cannot order more than %d different items or 0 items\n", MAX_ITEMS);
        return;
    }
    for (int i = 0; i < totaltypes; i++) {
        printf("Enter SKU ID for item %d: ", i + 1);
        scanf("%d", &t_itemids);
        temp=findSkuById(skulist,MAX_SKUS,t_itemids);
        if(temp==-1){
            printf("Invalid SKU ID. Please try again.\n");
            i--;
            continue;
        }
        printf("Enter quantity for item %d: ", i + 1);
        scanf("%d", &t_quantity);
        if (t_quantity > skulist[temp].stock) {
            printf("Insufficient stock for SKU ID %d. Available stock: %d\n", t_itemids, skulist[temp].stock);
            i--;
            continue;
        }
        orderlist[orderCounter-1].itemIds[i] = t_itemids;
        orderlist[orderCounter-1].itemQty[i] = t_quantity;
        orderlist[orderCounter-1].Count++;
        totalprice += skulist[temp].price * t_quantity;

        /// no change in stock or sold count until order is confirmed
    }
    orderlist[orderCounter-1].orderid = orderCounter;
    orderlist[orderCounter-1].subtotal = totalprice;
    orderlist[orderCounter-1].status = status_placed; 
    /// take care of time and delivery time later
    long long order_time=getCurrentDateTimeInt();
    orderlist[orderCounter-1].orderTime=order_time;
    printf("Order placed successfully with Order ID: %d\n", orderCounter);
    orderCounter++;
}  
//////////////////// Deliver Order ///////////////////////////////////////////////// 
void DeliverOrder(struct sku skulist[],int skusize, struct Order orderlist[],int ordersize){
    int t_orderId;
    printf("Enter order Id: ");
    scanf("%d",&t_orderId);
    int t_find=findorder(orderlist,MAX_ORDS,t_orderId);
    if(t_find==-1){
        printf("Enter Valid order id\n");
        return;
    }
    else{
        if(orderlist[t_find].status!=status_placed){
            printf("Order cannot be delivered as its not in placed status\n");
            return;
        }
        orderlist[t_find].status=status_delivered;
        // update stock and sold count
        for(int i=0;i<orderlist[t_find].Count;i++){
            int sku_index=findSkuById(skulist,MAX_SKUS,orderlist[t_find].itemIds[i]);
            if(skulist[sku_index].stock>=orderlist[t_find].itemQty[i]){
                skulist[sku_index].stock-=orderlist[t_find].itemQty[i];
                skulist[sku_index].soldCount+=orderlist[t_find].itemQty[i];
            }
            else{
                printf("Insufficient stock for SKU ID %d during delivery", skulist[sku_index].skuid);
            }
        }
        long long delivery_time=addMinutesToDateTimeInt(orderlist[t_find].orderTime,(rand()%11)+10); 
        orderlist[t_find].deliveryTime=delivery_time;
        printf("Order Delivered Successfully\n");
    }
}
//////////////////// Cancel Order ///////////////////////////////////////////////// 
void CancelOrder(struct sku skulist[],int skusize, struct Order orderlist[],int ordersize){
    int t_orderId;
    printf("Enter order Id: ");
    scanf("%d",&t_orderId);
    int t_find=findorder(orderlist,MAX_ORDS,t_orderId);
    if(t_find==-1){
        printf("Enter Valid order id\n");
        return;
    }
    else{
        if(orderlist[t_find].status==status_delivered){
            printf("Order cannot be cancelled as already delivered");
            return ;
        }
        else if(orderlist[t_find].status==status_cancelled){
            printf("Order is already cancelled");
            return ;
        }
        orderlist[t_find].status=status_cancelled;
        printf("Order Cancelled Successfully\n");
    }
}

//////////////////////////////import from csv//////////////////
int importSkusFromCSV(struct sku skulist[], int size){
    int index = findEmptySkuSlot(skulist, size);
    FILE *filepointer;
    filepointer = fopen("Import_Store.csv", "r");
    if (filepointer == NULL) {
        printf("File not found\n");
        return 1;
    }

    int read = 0;
    int done = 0;
    char line[1024];
    fgets(line,1024,filepointer);
    do {
        int skuid, stock, sold;
        float price;
        char name[50], category[30];
        int valid = 1;
        read = fscanf(filepointer, "%d,%50[^,],%30[^,],%f,%d,%d",
                      &skuid, name, category, &price, &stock, &sold);

        if (read == 6) {
            if (findSkuById(skulist, size, skuid) != -1) {
                printf("SKU with ID %d already exists. Skipping.\n", skuid);
                valid = 0;
            }

            if (valid == 1 && index != -1) {
                skulist[index].skuid = skuid;
                strcpy(skulist[index].name, name);
                strcpy(skulist[index].category, category);
                skulist[index].price = price;
                skulist[index].stock = stock;
                skulist[index].soldCount = sold;
                index++;
            }

            if (index == size) {
                done = 1;
            }
        }

        if (read != 6 && feof(filepointer)) {
            done = 1;
        }

        if (read != 6 && !feof(filepointer)) {
            printf("File format incorrect\n");
            fclose(filepointer);
            return 1;
        }

    } while (!done);

    fclose(filepointer);
    return 0;
}

/////////////////////////////// copying sku structure to csv ///////////////////////////
void exportSkusToCSV(struct sku skulist[], int size){
        FILE *filepointer;
        filepointer=fopen("Import_Store.csv","w");
        if(filepointer==NULL){
            printf("Error no file found\n");
        }
        int limit=findEmptySkuSlot(skulist,size);
        if(limit==-1) limit=MAX_SKUS;
        fprintf(filepointer,"SID,NAME,CATEGORY,PRICE,STOCK,SOLDCOUNT\n");
        for(int i=0;i<limit;i++){
            fprintf(filepointer,"%d,%s,%s,%f,%d,%d\n",skulist[i].skuid,skulist[i].name,skulist[i].category,skulist[i].price,skulist[i].stock,skulist[i].soldCount);
        }
        if(ferror(filepointer)){
            printf("Error writing to file\n");
        }
        fclose(filepointer);
}

//////////////////////////////// copying order structure to csv ///////////////////////////
void exportOrderdsToCSV(struct Order orderlist[],int size){
        FILE *filepointer;
        filepointer=fopen("Orders.csv","w");
        if(filepointer==NULL) printf("No file found");
        int i=0;
        fprintf(filepointer,"ID,ORDERTIME,STATUS,DELIVERYTIME,COUNT,SUBTOTAL\n");
        while(orderlist[i].orderid!=-1 && i < MAX_ORDS){
            fprintf(filepointer,"%d,%lld,%d,%lld,%d,%f\n",orderlist[i].orderid,orderlist[i].orderTime,orderlist[i].status,orderlist[i].deliveryTime,orderlist[i].Count,orderlist[i].subtotal);
            i++;
        }
        if(ferror(filepointer)){
            printf("Error writing to file\n");
        }
        fclose(filepointer);
}
//////////////////////////////// order details to csv ///////////////////////////
void OrderDetails(struct Order orderlist[],int size,int Id){
    int i=findorder(orderlist,MAX_ORDS,Id);
    if(i==-1){
        printf("Enter Valid id");
    }
    else{
        FILE *filepointer;
        filepointer=fopen("OrderDetails.csv","a");
        if(filepointer==NULL){
            printf("No file found");
        }
        int j=0;
        fprintf(filepointer,"\n########## OrderId:%d #################\n",Id);
        fprintf(filepointer,"ItemId,Qty\n");
        while(j<orderlist[i].Count){
            fprintf(filepointer,"%d,%d\n",orderlist[i].itemIds[j],orderlist[i].itemQty[j]);
            j++; 
        }
        if(ferror(filepointer)) printf("Error writing to file");
        fclose(filepointer);
    }
}


/////////////////////////////// sort order by sold count ///////////////////////////
void SortSkusbySoldCount(struct sku skulist[], int size, int choice) {
    int index[MAX_SKUS];
    int limit = findEmptySkuSlot(skulist, size);
    if (limit == -1) limit = size;

    for (int i = 0; i < limit; i++) {
        index[i] = i;
    }

    quickSort(index, 0, limit - 1, skulist);
    if(choice==1){
        int k;
        printf("Top K sold SKUs. Enter K:");
        scanf("%d",&k);
        if(k>limit) k=limit;
        for(int i=0;i<k;i++){
            int idx=index[i];
            printf("SKU ID: %d, Name: %s, Sold Count: %d\n",skulist[idx].skuid,skulist[idx].name,skulist[idx].soldCount);
        }
    }
    else {
        int total = totalsoldCount(skulist, size);
        if (total == 0) {
            printf("No sales data available.\n");
            return;
        }

        int cumSum = 0;
        int Count_A = 0, Count_B = 0, Count_C = 0;

        for (int i = 0; i < limit; i++) {
            int idx = index[i];
            cumSum += skulist[idx].soldCount;
            float perc = (cumSum * 100.0f) / total;

            if (perc <= 80.0)
                Count_A++;
            else if (perc <= 95.0)
                Count_B++;
            else
                Count_C++;
        }

        printf("\nABC Analysis:\n");
        printf("Category A (≈80%%): %d SKUs\n", Count_A);
        printf("Category B (≈15%%): %d SKUs\n", Count_B);
        printf("Category C (≈5%%): %d SKUs\n", Count_C);
    }

}

//////////////////// sort by time /////////////////////////////////////////////////
void SortOrdersbyTime(struct Order orderlist[],int size){
    int index[MAX_ORDS];
    int limit=orderCounter-1;
    for(int i=0;i<limit;i++){
        index[i]=i;
    }
    for(int i=0;i<limit-1;i++){
        for(int j=i+1;j<limit;j++){
            int idx1=index[i];
            int idx2=index[j];
            if(orderlist[idx1].orderTime>orderlist[idx2].orderTime|| (orderlist[idx1].orderTime==orderlist[idx2].orderTime && orderlist[idx1].orderid>orderlist[idx2].orderid)){
                int temp=index[i];
                index[i]=index[j];
                index[j]=temp;
            }
        }
    }
    printf("Orders sorted by time:\n");
    for(int i=0;i<limit;i++){
        int idx=index[i];
        printf("Order ID: %d, Order Time: %lld\n",orderlist[idx].orderid,orderlist[idx].orderTime);
    }   
}
///////////////////////////////////////// sku menu ////////////////////////////////////////

void SkuMenu(){
    int choice;
    int running = 1;
    while(running) {
        printf("======================== SKU Management Menu ========================\n");
        printf("1.Add SKU\n");
        printf("2.Update/Delete SKU\n");
        printf("3.Search SKU by Name\n");
        printf("4.Reset Skus\n");
        printf("5.Return to Main Menu\n");
        choice = readIntSafe("Enter your choice: ");

        switch(choice){
            case 1:{
                addSku(skulist);
                break;
            }
            case 2:{
                printf("Enter SKU ID to update/delete: ");
                int del_skuid;
                scanf("%d", &del_skuid);
                update_Delete(skulist, MAX_SKUS, del_skuid);
                break;
            }
            case 3:{
                printf("Enter SKU Name to search: ");
                char search_name[50];
                scanf("%s", search_name);
                searchSkuByName(skulist, MAX_SKUS, search_name);
                break;
            }
            case 4:{
                resetSku(skulist, MAX_SKUS);
                printf("SKU list has been reset.\n");
                break;
            }
            case 5:
                running = 0;
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}
///////////////////////////////////////// Order menu  ////////////////////////////////////////
void OrderMenu(){
    int choice;
    int running = 1;
    while(running) {
        printf("======================== Order Management Menu ========================\n");
        printf("1.Place New Order\n");
        printf("2.Cancel Order\n");
        printf("3.Deliver Order\n");
        printf("4.Reset Order list\n");
        printf("5.Return to Main Menu\n");
        choice = readIntSafe("Enter your choice: ");

        switch(choice){
            case 1:
                placeOrder(skulist,MAX_SKUS,orderlist,MAX_ORDS);
                break;
            case 2:
                CancelOrder(skulist,MAX_SKUS,orderlist,MAX_ORDS);
                break;
            case 3:
                DeliverOrder(skulist,MAX_SKUS,orderlist,MAX_ORDS);
                break;
            case 4:{
                resetOrder(orderlist, MAX_ORDS);
                printf("Order list has been reset.\n");
                break;
            }
            case 5:
                running = 0;
                break;
            default:
                printf("Invalid choice. Please try again.\n");  
        }
    }
}

//////////////////// sorting and abc analysis menu /////////////////////////////////////////////////
void SortandABCMenu(){
    int choice;
    int running = 1;
    while(running) {
        printf("======================== Sort and ABC Management Menu ========================\n");
        printf("1.Sort Orders by time\n");
        printf("2.Top K Bestsellers\n");
        printf("3.ABC Analysis\n");
        printf("4.Return to Main Menu\n");
        choice = readIntSafe("Enter your choice: ");

        switch(choice){
            case 1:
                SortOrdersbyTime(orderlist,MAX_ORDS);
                break;
            case 2:
                SortSkusbySoldCount(skulist,MAX_SKUS,1);
                break;
            case 3:
                SortSkusbySoldCount(skulist,MAX_SKUS,2);
                break;
            case 4:
                running = 0;
                break;
            default:
                printf("Invalid choice. Please try again.\n"); 
        }
    }
}    

///////////////////////////////////////// Import-Export menu ////////////////////////////////////////

void ExportMenu(){
    int choice;
    int running = 1;
    while(running) {
        printf("======================== File Export and Import Menu ========================\n");
        printf("1.Export SKUs to CSV\n");
        printf("2.Export Orders to CSV\n");
        printf("3.Export Order Details to CSV\n");
        printf("4.Import SKUs from CSV\n");
        printf("5.Return to main menu\n");
        choice = readIntSafe("Enter your choice: ");

        switch(choice){
            case 1:
                exportSkusToCSV(skulist,MAX_SKUS);
                break;
            case 2:
                exportOrderdsToCSV(orderlist,MAX_ORDS);
                break;
            case 3:
                int orderId;
                printf("Enter Order ID to export details: ");
                scanf("%d", &orderId);
                OrderDetails(orderlist, MAX_ORDS, orderId);
                break;
            case 4:
                if(importSkusFromCSV(skulist,MAX_SKUS)==0){
                    printf("SKUs imported successfully from CSV.\n");
                }
                break;
            case 5:
                running = 0;
                break;
            default:
                printf("Invalid choice. Please try again.\n"); 
        }
    }
}
////////////////////////////// main menu///////////////////////////////////
void mainmenu(){
    int choice;
    int running=1;
    while(running){
        printf("======================== Menu ========================\n");
        printf("1.Sku Management\n");
        printf("2.Order Management\n");
        printf("3.Sort and ABC analysis\n");
        printf("4.File Export and Import\n");
        printf("5.Exit\n");
        choice = readIntSafe("Enter your choice: ");
        
        switch(choice){
            case 1:
                SkuMenu();
                break;
            case 2:
                OrderMenu();
                break;
            case 3:
                SortandABCMenu();
                break;
            case 4:
                ExportMenu();
                break;
            case 5:
                running=0;
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}


/// //////////////////////////// main function////////////////////////////////////////////
int main(){
    srand(time(NULL));
    resetSku(skulist, MAX_SKUS);
    resetOrder(orderlist, MAX_ORDS); 
    importSkusFromCSV(skulist,MAX_SKUS);
    mainmenu();
    exportSkusToCSV(skulist,MAX_SKUS);
    return 0;
}