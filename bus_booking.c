/*
    BUS TICKET BOOKING SYSTEM
    C Programming Project - 20
    Modules used : Structures, Arrays, File Handling

    Name : Piyush Kumar
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define ROWS 10           // 10 rows in the bus
#define COLS 4            // seats A B | aisle | C D
#define TOTAL_ROUTES 6
#define MAX_BOOK 5        // max seats in one booking
#define FILE_NAME "bus_bookings.txt"

struct route{
    int id;
    char from[20];
    char to[20];
    char time[10];
    char type[15];
    int fare;
};

struct booking{
    int ticketNo;
    int routeId;
    char date[15];
    char seat[12];
    char name[40];
    int age;
    char gender;
    char phone[15];
    int fare;
};

struct route routes[TOTAL_ROUTES] = {
    {1, "Delhi", "Manali", "20:00", "Volvo AC", 1450},
    {2, "Delhi", "Jaipur", "06:30", "AC Seater", 650},
    {3, "Chandigarh", "Shimla", "08:00", "Non AC", 280},
    {4, "Delhi", "Dehradun", "22:30", "AC Seater", 720},
    {5, "Chandigarh", "Manali", "21:00", "Volvo AC", 1100},
    {6, "Delhi", "Agra", "07:00", "Non AC", 390}
};

char colName[COLS] = {'A', 'B', 'C', 'D'};


// reads one booking (one line) from file, returns 1 if read properly
int readBooking(FILE *fp, struct booking *b){
    int x;
    x = fscanf(fp,"%d|%d|%[^|]|%[^|]|%[^|]|%d|%c|%[^|]|%d\n",&b->ticketNo,&b->routeId,b->date,b->seat,b->name,&b->age,&b->gender,b->phone,&b->fare);
    if(x==9)
        return 1;
    return 0;
}

void writeBooking(FILE *fp, struct booking b){
    fprintf(fp,"%d|%d|%s|%s|%s|%d|%c|%s|%d\n",b.ticketNo,b.routeId,b.date,b.seat,b.name,b.age,b.gender,b.phone,b.fare);
}

int findRoute(int id){
    int i;
    for(i=0;i<TOTAL_ROUTES;i++){
        if(routes[i].id==id)
            return i;
    }
    return -1;
}

void showRoutes(){
    int i;
    printf("\n%-4s %-12s %-10s %-7s %-11s %s\n","No","From","To","Time","Bus Type","Fare");
    printf("--------------------------------------------------------\n");
    for(i=0;i<TOTAL_ROUTES;i++){
        printf("%-4d %-12s %-10s %-7s %-11s Rs %d\n",routes[i].id,routes[i].from,routes[i].to,routes[i].time,routes[i].type,routes[i].fare);
    }
    printf("--------------------------------------------------------\n");
}

// date should be like 15/12/2026
int checkDate(char d[]){
    int i, day, month;
    if(strlen(d)!=10 || d[2]!='/' || d[5]!='/')
        return 0;
    for(i=0;i<10;i++){
        if(i==2 || i==5)
            continue;
        if(!isdigit(d[i]))
            return 0;
    }
    day = (d[0]-'0')*10 + (d[1]-'0');
    month = (d[3]-'0')*10 + (d[4]-'0');
    if(day<1 || day>31 || month<1 || month>12)
        return 0;
    return 1;
}

int checkPhone(char p[]){
    int i;
    if(strlen(p)!=10)
        return 0;
    for(i=0;i<10;i++){
        if(!isdigit(p[i]))
            return 0;
    }
    return 1;
}

// changes seat like "5C" to a number from 0 to 39, gives -1 if seat is wrong
int seatIndex(char s[]){
    int len, row, col;
    char c;
    len = strlen(s);
    if(len<2 || len>3)
        return -1;
    c = toupper(s[len-1]);
    if(c<'A' || c>'D' || !isdigit(s[len-2]))
        return -1;
    col = c - 'A';
    row = atoi(s);      // atoi stops when it reaches the letter
    if(row<1 || row>ROWS)
        return -1;
    return (row-1)*COLS + col;
}

// fills seat[][] with 1 where seat is already booked for that route and date
void loadSeats(int routeId, char date[], int seat[ROWS][COLS]){
    FILE *fp;
    struct booking b;
    int i, j, k;

    for(i=0;i<ROWS;i++)
        for(j=0;j<COLS;j++)
            seat[i][j] = 0;

    fp = fopen(FILE_NAME,"r");
    if(fp==NULL)
        return;
    while(readBooking(fp,&b)){
        if(b.routeId==routeId && strcmp(b.date,date)==0){
            k = seatIndex(b.seat);
            if(k!=-1)
                seat[k/COLS][k%COLS] = 1;
        }
    }
    fclose(fp);
}

// prints the bus layout and returns how many seats are free
int showSeatMap(int seat[ROWS][COLS]){
    int i, j, count=0;

    printf("\n     ======= FRONT (Driver) =======\n\n");
    for(i=0;i<ROWS;i++){
        printf("   ");
        for(j=0;j<COLS;j++){
            if(j==2)
                printf("     ");    // aisle
            if(seat[i][j]==1)
                printf("[ XX ]");
            else{
                printf("[%3d%c]",i+1,colName[j]);
                count++;
            }
        }
        printf("\n");
    }
    printf("\n   XX = booked  |  A and D are window seats\n");
    printf("   Seats available: %d/%d\n",count,ROWS*COLS);
    return count;
}

// finds biggest ticket no in file and returns next one
int nextTicketNo(){
    FILE *fp;
    struct booking b;
    int max = 5000;

    fp = fopen(FILE_NAME,"r");
    if(fp==NULL)
        return max+1;
    while(readBooking(fp,&b)){
        if(b.ticketNo>max)
            max = b.ticketNo;
    }
    fclose(fp);
    return max+1;
}

// puts all seats of one ticket into list[] and returns how many seats
int getTicket(int tno, struct booking list[]){
    FILE *fp;
    struct booking b;
    int n = 0;

    fp = fopen(FILE_NAME,"r");
    if(fp==NULL)
        return 0;
    while(readBooking(fp,&b)){
        if(b.ticketNo==tno && n<MAX_BOOK){
            list[n] = b;
            n++;
        }
    }
    fclose(fp);
    return n;
}

// same function prints ticket on screen (stdout) and in ticket file
void printTicket(FILE *out, struct booking list[], int n){
    int i, idx, total=0;
    idx = findRoute(list[0].routeId);

    fprintf(out,"\n==============================================\n");
    fprintf(out,"                 BUS TICKET\n");
    fprintf(out,"==============================================\n");
    fprintf(out," Ticket No : %d\n",list[0].ticketNo);
    fprintf(out," Route     : %s to %s\n",routes[idx].from,routes[idx].to);
    fprintf(out," Date      : %s    Departure : %s\n",list[0].date,routes[idx].time);
    fprintf(out," Bus Type  : %s\n",routes[idx].type);
    fprintf(out," Contact   : %s\n",list[0].phone);
    fprintf(out,"----------------------------------------------\n");
    fprintf(out," %-6s %-22s %-5s %s\n","Seat","Name","Age","Gender");
    for(i=0;i<n;i++){
        fprintf(out," %-6s %-22s %-5d %c\n",list[i].seat,list[i].name,list[i].age,list[i].gender);
        total = total + list[i].fare;
    }
    fprintf(out,"----------------------------------------------\n");
    fprintf(out," Total Fare : Rs %d\n",total);
    fprintf(out,"==============================================\n");
}

void viewSeats(){
    int rid=0, idx;
    int seat[ROWS][COLS];
    char date[15];

    showRoutes();
    printf("\nEnter route no: ");
    scanf("%d",&rid);
    idx = findRoute(rid);
    if(idx==-1){
        printf("Invalid route!\n");
        return;
    }
    printf("Enter date (dd/mm/yyyy): ");
    scanf("%14s",date);
    if(!checkDate(date)){
        printf("Invalid date!\n");
        return;
    }

    printf("\n%s to %s | %s | %s",routes[idx].from,routes[idx].to,date,routes[idx].time);
    loadSeats(rid,date,seat);
    showSeatMap(seat);
}

void bookTicket(){
    struct booking list[MAX_BOOK];
    int seat[ROWS][COLS];
    int rid=0, idx, n=0, i, k, freeSeats, tno, total;
    char date[15], phone[15], s[10], fname[30], ch;
    FILE *fp;

    showRoutes();
    printf("\nEnter route no: ");
    scanf("%d",&rid);
    idx = findRoute(rid);
    if(idx==-1){
        printf("Invalid route!\n");
        return;
    }

    printf("Enter travel date (dd/mm/yyyy): ");
    scanf("%14s",date);
    if(!checkDate(date)){
        printf("Invalid date!\n");
        return;
    }

    loadSeats(rid,date,seat);
    freeSeats = showSeatMap(seat);
    if(freeSeats==0){
        printf("\nSorry, bus is full on this date.\n");
        return;
    }

    printf("\nHow many seats do you want? (max %d): ",MAX_BOOK);
    scanf("%d",&n);
    if(n<1 || n>MAX_BOOK){
        printf("You can book 1 to %d seats only!\n",MAX_BOOK);
        return;
    }
    if(n>freeSeats){
        printf("Only %d seats are free!\n",freeSeats);
        return;
    }

    printf("Enter contact number: ");
    scanf("%14s",phone);
    if(!checkPhone(phone)){
        printf("Phone number should be 10 digits!\n");
        return;
    }

    tno = nextTicketNo();

    for(i=0;i<n;i++){
        printf("\n--- Passenger %d ---\n",i+1);

        // keep asking until a free seat is given
        while(1){
            printf("Choose seat (eg. 4B): ");
            if(scanf("%9s",s)!=1)
                return;
            k = seatIndex(s);
            if(k==-1)
                printf("No such seat, try again.\n");
            else if(seat[k/COLS][k%COLS]==1)
                printf("Seat %s is already booked, choose another.\n",s);
            else
                break;
        }
        seat[k/COLS][k%COLS] = 1;     // so the same seat is not picked twice
        sprintf(list[i].seat,"%d%c",k/COLS+1,colName[k%COLS]);

        printf("Name: ");
        scanf(" %39[^\n]",list[i].name);
        for(k=0;list[i].name[k]!='\0';k++){
            if(list[i].name[k]=='|')       // | is used in file so cant be in name
                list[i].name[k] = ' ';
        }

        list[i].age = 0;
        printf("Age: ");
        scanf("%d",&list[i].age);
        if(list[i].age<1 || list[i].age>110){
            printf("Invalid age! Booking stopped.\n");
            return;
        }

        printf("Gender (M/F/O): ");
        scanf(" %c",&list[i].gender);
        list[i].gender = toupper(list[i].gender);
        if(list[i].gender!='M' && list[i].gender!='F' && list[i].gender!='O'){
            printf("Invalid gender! Booking stopped.\n");
            return;
        }

        list[i].ticketNo = tno;
        list[i].routeId = rid;
        strcpy(list[i].date,date);
        strcpy(list[i].phone,phone);
        list[i].fare = routes[idx].fare;
    }

    total = n * routes[idx].fare;
    printf("\nTotal amount: Rs %d  (%d x %d)\n",total,n,routes[idx].fare);
    printf("Confirm booking? (y/n): ");
    scanf(" %c",&ch);
    if(ch!='y' && ch!='Y'){
        printf("Booking cancelled.\n");
        return;
    }

    // add the new seats at end of bookings file
    fp = fopen(FILE_NAME,"a");
    if(fp==NULL){
        printf("Error in opening file!\n");
        return;
    }
    for(i=0;i<n;i++)
        writeBooking(fp,list[i]);
    fclose(fp);

    // also save the ticket in its own txt file
    sprintf(fname,"ticket_%d.txt",tno);
    fp = fopen(fname,"w");
    if(fp!=NULL){
        printTicket(fp,list,n);
        fclose(fp);
    }

    printf("\nBooking confirmed!!");
    printTicket(stdout,list,n);
    printf("Ticket saved in %s\n",fname);
}

void cancelTicket(){
    struct booking list[MAX_BOOK], b;
    int tno=0, n, i, total=0, refund;
    char ch, fname[30];
    FILE *fp, *temp;

    printf("\nEnter ticket number: ");
    scanf("%d",&tno);
    n = getTicket(tno,list);
    if(n==0){
        printf("No booking found with ticket no %d\n",tno);
        return;
    }

    printTicket(stdout,list,n);
    printf("Cancel all %d seat(s) of this ticket? (y/n): ",n);
    scanf(" %c",&ch);
    if(ch!='y' && ch!='Y'){
        printf("Ok, ticket not cancelled.\n");
        return;
    }

    // copy all other bookings to temp file and then replace the old file
    fp = fopen(FILE_NAME,"r");
    temp = fopen("temp.txt","w");
    if(fp==NULL || temp==NULL){
        printf("Error in opening file!\n");
        return;
    }
    while(readBooking(fp,&b)){
        if(b.ticketNo!=tno)
            writeBooking(temp,b);
    }
    fclose(fp);
    fclose(temp);
    remove(FILE_NAME);
    rename("temp.txt",FILE_NAME);

    sprintf(fname,"ticket_%d.txt",tno);
    remove(fname);         // ticket file not needed now

    for(i=0;i<n;i++)
        total = total + list[i].fare;
    refund = total - (total*10)/100;      // 10% cancellation charge

    printf("\nTicket %d cancelled.\n",tno);
    printf("Refund amount: Rs %d (10%% cancellation charge)\n",refund);
}

void searchBooking(){
    struct booking list[MAX_BOOK], b;
    int opt=0, tno=0, n, found=0, idx;
    char phone[15], rname[45];
    FILE *fp;

    printf("\n1. Search by Ticket No\n");
    printf("2. Search by Phone No\n");
    printf("Enter option: ");
    scanf("%d",&opt);

    if(opt==1){
        printf("Enter ticket number: ");
        scanf("%d",&tno);
        n = getTicket(tno,list);
        if(n==0)
            printf("No booking found!\n");
        else
            printTicket(stdout,list,n);
    }
    else if(opt==2){
        printf("Enter phone number: ");
        scanf("%14s",phone);
        fp = fopen(FILE_NAME,"r");
        if(fp==NULL){
            printf("No bookings yet.\n");
            return;
        }
        printf("\n%-8s %-20s %-12s %-5s %s\n","Ticket","Route","Date","Seat","Name");
        printf("--------------------------------------------------------------\n");
        while(readBooking(fp,&b)){
            if(strcmp(b.phone,phone)==0){
                idx = findRoute(b.routeId);
                sprintf(rname,"%s-%s",routes[idx].from,routes[idx].to);
                printf("%-8d %-20s %-12s %-5s %s\n",b.ticketNo,rname,b.date,b.seat,b.name);
                found = 1;
            }
        }
        fclose(fp);
        if(found==0)
            printf("No booking found for this number.\n");
    }
    else{
        printf("Invalid option!\n");
    }
}

void allRecords(){
    FILE *fp;
    struct booking b;
    int count=0, total=0, idx;
    char rname[45];

    fp = fopen(FILE_NAME,"r");
    if(fp==NULL){
        printf("\nNo bookings yet.\n");
        return;
    }

    printf("\n%-7s %-18s %-11s %-5s %-20s %-4s %-2s %-11s %s\n","Ticket","Route","Date","Seat","Name","Age","G","Phone","Fare");
    printf("------------------------------------------------------------------------------------------\n");
    while(readBooking(fp,&b)){
        idx = findRoute(b.routeId);
        sprintf(rname,"%s-%s",routes[idx].from,routes[idx].to);
        printf("%-7d %-18s %-11s %-5s %-20s %-4d %-2c %-11s %d\n",b.ticketNo,rname,b.date,b.seat,b.name,b.age,b.gender,b.phone,b.fare);
        count++;
        total = total + b.fare;
    }
    fclose(fp);

    if(count==0){
        printf("No bookings yet.\n");
        return;
    }
    printf("------------------------------------------------------------------------------------------\n");
    printf("Total seats booked : %d\n",count);
    printf("Total collection   : Rs %d\n",total);
}

int main(){
    int ch;

    while(1){
        printf("\n\n+----------------------------------------+\n");
        printf("|        BUS TICKET BOOKING SYSTEM       |\n");
        printf("+----------------------------------------+\n");
        printf("  1. Show Routes\n");
        printf("  2. View Seats\n");
        printf("  3. Book Ticket\n");
        printf("  4. Cancel Ticket\n");
        printf("  5. Search Booking\n");
        printf("  6. All Booking Records\n");
        printf("  7. Exit\n");
        printf("Enter your choice: ");

        if(scanf("%d",&ch)!=1){
            while(getchar()!='\n');     // clear wrong input
            ch = 0;
        }

        switch(ch){
            case 1: showRoutes(); break;
            case 2: viewSeats(); break;
            case 3: bookTicket(); break;
            case 4: cancelTicket(); break;
            case 5: searchBooking(); break;
            case 6: allRecords(); break;
            case 7:
                printf("\nThanks for using Bus Ticket Booking System. Happy journey!\n");
                return 0;
            default:
                printf("\nInvalid choice, try again.\n");
        }
    }
    return 0;
}
