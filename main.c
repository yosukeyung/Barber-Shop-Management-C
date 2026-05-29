#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include<windows.h>

#define TERMINAL_WIDTH 80
#define COLS 7  
#define FILENAME "barber_data.txt"
#define merah "\033[38;5;9m"
#define reset "\033[0m"
#define biruneon "\033[38;5;12m"
#define hijauneon "\033[38;5;10m"
#define lightblue "\033[38;5;12m"
#define white "\033[38;5;15m"
#define gray "\033[38;5;245m"

struct barber {    
    int price, date, month, year, id;
    char c_name[101], service_type[2], service_name[101], b_name[101], rate[10];
    struct barber *left, *right;
} *root = NULL, *input = NULL;

void printNoDataTable();
int key(int d, int m, int y){
    
    return y * 10000 + m * 100 + d;
}
int aa=0;
int sameKey(int d, int m, int y){
	aa++;
	return(y * 10000 + m * 100 + d)+aa;
}

void lower(const char src[], char dest[]) {
    int i = 0;
    for(i = 0; src[i] != '\0'; i++){
        dest[i] = tolower(src[i]); 
    }
    dest[i] = '\0';  
}

void upper(char *s) {
    for (int i = 0; s[i] != '\0'; i++){
        s[i] = toupper(s[i]); 
    }
}

struct barber *insert(struct barber *newnode, struct barber *curr){
    if(curr == NULL){
        newnode->left = newnode->right = NULL;
        return newnode;
    }
    
    if(newnode->id < curr->id) curr->left = insert(newnode, curr->left);            
    else if(newnode->id > curr->id) curr->right = insert(newnode, curr->right);
    
    return curr;
}



void printTableHeader() {
    int widths[COLS] = {12, 32, 12, 22, 12, 8, 15};
    char *headers[COLS] = {"ID", "Customer Name", "Type", "Service Name", "Barber", "Price", "Date"};
    printf("╔");
    for (int i = 0; i < COLS; i++) {
        for (int j = 0; j < widths[i]; j++) printf("═");
        if (i < COLS - 1) printf("╦");
    }
    printf("╗\n");
    printf("║");
    for (int i = 0; i < COLS; i++) {
        printf(" %-*s ", widths[i] - 2, headers[i]);
        printf("║");
    }
    printf("\n");
    printf("╠");
    for (int i = 0; i < COLS; i++) {
        for (int j = 0; j < widths[i]; j++) printf("═");
        if (i < COLS - 1) printf("╬");
    }
    printf("╣\n");
}

void printTableRow(struct barber *node) {
    if (!node) return;
    char datebuf[16];
    snprintf(datebuf, sizeof(datebuf), "%02d/%02d/%04d", node->date, node->month, node->year);
    printf("║ %-10d ║ %-30s ║ %-10s ║ %-20s ║ %-10s ║ %-6d ║ %-13s ║\n",
        node->id, node->c_name, node->service_type, node->service_name,
        node->b_name, node->price, datebuf);
}

void printTableFooter() {
    int widths[COLS] = {12, 32, 12, 22, 12, 8, 15};
    printf("╚");
    for (int i = 0; i < COLS; i++) {
        for (int j = 0; j < widths[i]; j++) printf("═");
        if (i < COLS - 1) printf("╩");
    }
    printf("╝\n");
}

void viewTableAsc(struct barber *curr) {
    if (!curr) return;
    viewTableAsc(curr->left);
    printTableRow(curr);
    viewTableAsc(curr->right);
}

void viewTableDesc(struct barber *curr) {
    if (!curr) return;
    viewTableDesc(curr->right);
    printTableRow(curr);
    viewTableDesc(curr->left);
}

void viewTable(struct barber *root, int order) {
    printTableHeader();
    if (order == 1) viewTableAsc(root);
    else viewTableDesc(root);
    printTableFooter();
}

struct barber *search(int id, struct barber *curr){
    if(curr == NULL) return curr;
    
    if(id < curr->id) return search(id, curr->left);
    else if(id > curr->id) return search(id, curr->right);
    else return curr;
}

struct barber *predecessor(struct barber *curr) {
    struct barber *temp = curr->left;
    while (temp->right != NULL) temp = temp->right;
    return temp;
}

struct barber *deletion(struct barber *curr, int id){
    if(curr == NULL) return NULL;
    
    if(id < curr->id) {
        curr->left = deletion(curr->left, id);
    }
    else if(id > curr->id) {
        curr->right = deletion(curr->right, id);
    }
    else {
        
        if(curr->left == NULL && curr->right == NULL) {
            free(curr);
            return NULL;
        }
        else if(curr->left == NULL) {
            struct barber *temp = curr->right;
            free(curr);
            return temp;
        }
        else if(curr->right == NULL) {
            struct barber *temp = curr->left;
            free(curr);
            return temp;
        }
        else {
            
            struct barber *pred = predecessor(curr);
            
            curr->id = pred->id;
            strcpy(curr->c_name, pred->c_name);
            strcpy(curr->service_type, pred->service_type);
            strcpy(curr->service_name, pred->service_name);
            strcpy(curr->b_name, pred->b_name);
            strcpy(curr->rate, pred->rate);
            curr->price = pred->price;
            curr->date = pred->date;
            curr->month = pred->month;
            curr->year = pred->year;
            
            curr->left = deletion(curr->left, pred->id);
        }
    }
    return curr;
}

void pressEnterToContinue() {
    printf("%s\n\t\t\t\t\t\t\tPress Enter to Continue...%s", hijauneon, reset);
    while (getchar() != '\n'); 
}

void clearScreen() {
#ifdef _WIN32
    system("cls");    
#else
    system("clear");  
#endif
}

int isLeapYear(int year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

int isValidDate(int d, int m, int y) {
    if (y < 2025) return 0;
    if (m < 1 || m > 12) return 0;
    int maxDay;
    switch (m) {
        case 2:
            maxDay = isLeapYear(y) ? 29 : 28;
            break;
        case 4: case 6: case 9: case 11:
            maxDay = 30;
            break;
        default:
            maxDay = 31;
    }
    if (d < 1 || d > maxDay) return 0;
    return 1;
}

void calculateTotalRevenue(struct barber *curr, int *total) {
    if (curr == NULL) return;
    *total += curr->price;
    calculateTotalRevenue(curr->left, total);
    calculateTotalRevenue(curr->right, total);
}

void countServicesByType(struct barber *curr, int *haircut, int *hairdye, int *perm) {
    if (curr == NULL) return;
    if (strcmp(curr->service_type, "C") == 0) (*haircut)++;
    else if (strcmp(curr->service_type, "D") == 0) (*hairdye)++;
    else if (strcmp(curr->service_type, "P") == 0) (*perm)++;
    countServicesByType(curr->left, haircut, hairdye, perm);
    countServicesByType(curr->right, haircut, hairdye, perm);
}

void countBarber(struct barber *node, char barbers[100][101], int counts[100], int *total) {
    if (node == NULL) return;
    countBarber(node->left, barbers, counts, total);
    int founds = 0;
    for (int i = 0; i < *total; i++) {
        if (strcmp(barbers[i], node->b_name) == 0) {
            counts[i]++;
            founds = 1;
            break;
        }
    }
    if (!founds && *total < 100) {
        strcpy(barbers[*total], node->b_name);
        counts[*total] = 1;
        (*total)++;
    }
    countBarber(node->right, barbers, counts, total);
}

void findMostPopularBarber(struct barber *curr, char popular_barber[101], int *max_count) {
    if (curr == NULL) return;
    char barbers[100][101];
    int counts[100] = {0};
    int total = 0;
    countBarber(curr, barbers, counts, &total);
    for (int i = 0; i < total; i++) {
        if (counts[i] > *max_count) {
            *max_count = counts[i];
            strcpy(popular_barber, barbers[i]);
        }
    }
}

void processData() {
    if (root == NULL) {
        printNoDataTable();
        return;
    }
    
    int total_revenue = 0;
    calculateTotalRevenue(root, &total_revenue);
    int haircut_count = 0, hairdye_count = 0, perm_count = 0;
    countServicesByType(root, &haircut_count, &hairdye_count, &perm_count);
    char popular_barber[101] = "";
    int max_count = 0;
     findMostPopularBarber(root, popular_barber, &max_count);
    
    printf("\n");
    printf("\t\t\t\t\t%s╔══════════════════════════════════════════════════════╗%s\n", biruneon, reset);
    printf("\t\t\t\t\t%s║%s                 BARBER SHOP ANALYSIS                 %s║%s\n", biruneon, reset, biruneon, reset);
    printf("\t\t\t\t\t%s╠══════════════════════════════════════════════════════╣%s\n", biruneon, reset);
    printf("\t\t\t\t\t%s║%s Total Revenue           │ %-24d %s  ║%s\n", biruneon, reset, total_revenue, biruneon, reset);
    printf("\t\t\t\t\t%s╠═════════════════════════╬════════════════════════════╣%s\n", biruneon, reset);
    printf("\t\t\t\t\t%s║%s Haircuts                │ %-24d %s  ║%s\n", biruneon, reset, haircut_count, biruneon, reset);
    printf("\t\t\t\t\t%s║%s Hair Dye                │ %-24d %s  ║%s\n", biruneon, reset, hairdye_count, biruneon, reset);
    printf("\t\t\t\t\t%s║%s Perm                    │ %-24d %s  ║%s\n", biruneon, reset, perm_count, biruneon, reset);
    printf("\t\t\t\t\t%s╠═════════════════════════╬════════════════════════════╣%s\n", biruneon, reset);
    printf("\t\t\t\t\t%s║%s Most Popular Barber     │ %-24s %s  ║%s\n", biruneon, reset, popular_barber, biruneon, reset);
    printf("\t\t\t\t\t%s╚══════════════════════════════════════════════════════╝%s\n", biruneon, reset);
    printf("\n");
}


void saveDataToFile(struct barber *curr, FILE *fp) {
    if (curr == NULL) return;
    
    saveDataToFile(curr->left, fp);
    fprintf(fp, "%d|%s|%s|%s|%s|%d|%d|%d|%d|%s\n",
             curr->id, curr->c_name, curr->service_type, curr->service_name,
            curr->b_name, curr->price, curr->date, curr->month, curr->year, curr->rate);
    saveDataToFile(curr->right, fp);
}

void saveData() {
    FILE *fp = fopen(FILENAME, "w");
    if (fp == NULL) {
        printf("Error opening file for writing!\n");
        return;
    }
    
    saveDataToFile(root, fp);
    fclose(fp);
    printf("Data saved successfully to %s\n", FILENAME);
}

void loadData() {
    FILE *fp = fopen(FILENAME, "r");
    if (fp == NULL) {
        return; 
    }
    
    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        struct barber *newnode = (struct barber *)malloc(sizeof(struct barber));
        
        char *token = strtok(line, "|");
        if (token) newnode->id = atoi(token);
        
        token = strtok(NULL, "|");
        if (token) strcpy(newnode->c_name, token);
        
        token = strtok(NULL, "|");
        if (token) strcpy(newnode->service_type, token);
        
        token = strtok(NULL, "|");
        if (token) strcpy(newnode->service_name, token);
        
        token = strtok(NULL, "|");
        if (token) strcpy(newnode->b_name, token);
        
        token = strtok(NULL, "|");
        if (token) newnode->price = atoi(token);
        
        token = strtok(NULL, "|");
        if (token) newnode->date = atoi(token);
        
        token = strtok(NULL, "|");
        if (token) newnode->month = atoi(token);
        
        token = strtok(NULL, "|");
        if (token) newnode->year = atoi(token);
        
        token = strtok(NULL, "|");
        if (token) {
            token[strcspn(token, "\n")] = 0;
            strcpy(newnode->rate, token);
        }
        
        newnode->left = newnode->right = NULL;
        root = insert(newnode, root);
    }
    
    fclose(fp);
}

void printMenuTable() {
    printf("\t\t\t\t\t\t%s╔════════════════════════════════════════╗%s\n", lightblue, reset);
    printf("\t\t\t\t\t\t%s║%s            BARBER SHOP MENU            %s║%s\n", lightblue, reset, lightblue, reset);
    printf("\t\t\t\t\t\t%s╠════════════════════════════════════════╣%s\n",lightblue, reset);
    printf("\t\t\t\t\t\t%s║%s 1. Create                              %s║%s\n", lightblue, reset, lightblue, reset);
    printf("\t\t\t\t\t\t%s║%s 2. Read                                %s║%s\n", lightblue, reset, lightblue, reset);
    printf("\t\t\t\t\t\t%s║%s 3. Update                              %s║%s\n", lightblue, reset, lightblue, reset);
    printf("\t\t\t\t\t\t%s║%s 4. Delete                              %s║%s\n", lightblue, reset, lightblue, reset);
    printf("\t\t\t\t\t\t%s║%s 5. View Analysis                       %s║%s\n", lightblue, reset, lightblue, reset);
    printf("\t\t\t\t\t\t%s║%s 6. Exit and Save                       %s║%s\n", lightblue, reset, lightblue, reset);
    printf("\t\t\t\t\t\t%s╠════════════════════════════════════════╣%s\n", lightblue, reset);
    printf("\t\t\t\t\t\t%s║ >>                                     ║%s\n", lightblue, reset);
    printf("\t\t\t\t\t\t%s╚════════════════════════════════════════╝%s\n",lightblue, reset);
}

void printSimpleTableRow(struct barber *node) {
    if (!node) return;
    char datebuf[16];
    snprintf(datebuf, sizeof(datebuf), "%02d/%02d/%04d", node->date, node->month, node->year);
    printf("| %-10d | %-28s | %-4s | %-20s | %-14s | %-5d | %-10s |\n",
        node->id, node->c_name, node->service_type, node->service_name,
        node->b_name, node->price, datebuf);
}

void viewSimpleTableAsc(struct barber *curr) {
    if (!curr) return;
    viewSimpleTableAsc(curr->left);
    printSimpleTableRow(curr);
    viewSimpleTableAsc(curr->right);
}

void viewSimpleTableDesc(struct barber *curr) {
    if (!curr) return;
    viewSimpleTableDesc(curr->right);
    printSimpleTableRow(curr);
    viewSimpleTableDesc(curr->left);
}

void printDoubleTableHeader() {
    printf("\t\t\t%s╔════════════╦════════════════════════════════╦══════╦══════════════════════╦══════════════╦═══════╦════════════╗%s\n", lightblue, reset);
    printf("\t\t\t%s║%s ID         %s║%s Customer Name                  %s║%s Type %s║%s Service Name         %s║%s Barber       %s║%s Price %s║%s Date       %s║%s\n",lightblue, reset, lightblue, reset, lightblue, reset, lightblue, reset, lightblue, reset, lightblue, reset, lightblue, reset, lightblue, reset );
    printf("\t\t\t%s╠════════════╬════════════════════════════════╬══════╬══════════════════════╬══════════════╬═══════╬════════════╣%s\n", lightblue, reset);
}

void printDoubleTableRow(struct barber *node) {
    if (!node) return;
    
    char datebuf[16];
    snprintf(datebuf, sizeof(datebuf), "%02d/%02d/%04d", 
             node->date, node->month, node->year);
    
    printf("\t\t\t%s║%s %-10d %s║%s %-30s %s║%s %-4s %s║%s %-20s %s║%s %-12s %s║%s %-5d %s║%s %-10s %s║%s\n",
           lightblue, reset, node->id,
           lightblue, reset, node->c_name,
           lightblue, reset, node->service_type,
           lightblue, reset, node->service_name,
           lightblue, reset, node->b_name,
           lightblue, reset, node->price,
           lightblue, reset, datebuf,
           lightblue, reset);
}

void printDoubleTableFooter() {
    printf("\t\t\t%s╚════════════╩════════════════════════════════╩══════╩══════════════════════╩══════════════╩═══════╩════════════╝%s\n", lightblue, reset);
}

void viewDoubleTableAsc(struct barber *curr) {
    if (!curr) return;
    viewDoubleTableAsc(curr->left);
    printDoubleTableRow(curr);
    viewDoubleTableAsc(curr->right);
}

void viewDoubleTableDesc(struct barber *curr) {
    if (!curr) return;
    viewDoubleTableDesc(curr->right);
    printDoubleTableRow(curr);
    viewDoubleTableDesc(curr->left);
}

void viewDoubleTable(struct barber *root, int order) {
    printDoubleTableHeader();
    if (order == 1) viewDoubleTableAsc(root);
    else viewDoubleTableDesc(root);
    printDoubleTableFooter();
}

void printNoDataTable() {
    printf("%s\n", biruneon);
    printf("\t\t\t\t\t\t\t╔═════════════════════════════════════╗\n");
    printf("\t\t\t\t\t\t\t║%s          NO DATA AVAILABLE          %s║%s\n", reset, biruneon, reset);
    printf("\t\t\t\t\t\t\t%s╠═════════════════════════════════════╣%s\n", biruneon, reset);
    printf("\t\t\t\t\t\t\t%s║%s  %sPlease add data to see the table! %s %s║%s\n", biruneon, reset, hijauneon, reset, biruneon, reset);
    printf("\t\t\t\t\t\t\t%s╚═════════════════════════════════════╝%s\n",biruneon, reset);
}

void printEmptyDoubleTable() {
    printDoubleTableHeader();
    printf("%s║%s%70s%s║%s\n", lightblue, reset, "NO DATA", lightblue, reset);
    printDoubleTableFooter();
}

int isValidRating(const char *rating) {
    if (strlen(rating) > 5) return 0;
    for (int i = 0; rating[i] != '\0'; i++) {
        if (rating[i] != '*') return 0;
    }
    return 1;
}

int isValidName(const char *name) {
    if (strlen(name) == 0 || strspn(name, " ") == strlen(name)) return 0;
    
    for (int i = 0; name[i] != '\0'; i++) {
        if (isdigit(name[i])) return 0;
    }
    return 1;
}

void stopsound() {
    system("kill $(cat sound.pid) 2>/dev/null; rm -f sound.pid");
}

void handle_sigint(int sig) {
    stopsound();
    exit(0);
}

void printReadOptionsTable() {
    printf("\t\t\t\t\t\t\t%s╔══════════════════════════════════════════════╗%s\n", lightblue, reset);
    printf("\t\t\t\t\t\t\t%s║%s              READ DATA OPTIONS               %s║%s\n", lightblue, reset, lightblue, reset);
    printf("\t\t\t\t\t\t\t%s╠══════════════════════════════════════════════╣%s\n", lightblue, reset);
    printf("\t\t\t\t\t\t\t%s║%s 1. Ascending by date                         %s║%s\n", lightblue, reset, lightblue, reset);
    printf("\t\t\t\t\t\t\t%s║%s 2. Descending by date                        %s║%s\n", lightblue, reset, lightblue, reset);
    printf("\t\t\t\t\t\t\t%s╠══════════════════════════════════════════════╣%s\n", lightblue, reset);
    printf("\t\t\t\t\t\t\t%s║ >>                                           ║%s\n", lightblue, reset);
    printf("\t\t\t\t\t\t\t%s╚══════════════════════════════════════════════╝%s\n", lightblue, reset);
    printf("\033[2A"); 
    printf("\033[61C"); 
}

int main(){
	loadData();
	// ini buat munculin lagu untuk os mac
    // signal(SIGINT, handle_sigint);
    // struct stat buffer;
    // if (stat("sound.wav", &buffer) != 0) {
    //     printf("%sFile sound.wav tidak ditemukan!%s\n", merah, reset);
    // } else {
    //     int ret = system("afplay sound.wav & echo $! > sound.pid");
    //     if (ret != 0) printf("%sGagal memutar sound! Cek nama file dan format sound.\n%s", merah, reset);
    // }
    
    //ini buat munculin lagu untuk os windows
    PlaySound("sound.wav", NULL, SND_FILENAME | SND_ASYNC);
    printf("\t\t\t\t\t%s__          __  _______   __        _______   _______    __         __    ______\n", biruneon);
	printf("\t\t\t\t\t\\ \\        / / |  ____|  | |       | _____|  |  ___  |  |   \\     /   |  |  ____|\n");
	printf("\t\t\t\t\t \\ \\      / /  | |____   | |       | |       | |   | |  | |\\ \\   / /| |  | |____\n");
	printf("\t\t\t\t\t  \\ \\ /\\ / /   |  ____|  | |       | |       | |   | |  | | \\ \\ / / | |  |  ____|\n");
	printf("\t\t\t\t\t   \\ V  V /    | |____   | |_____  | |____   | |___| |  | |  \\ V /  | |  | |____\n");
	printf("\t\t\t\t\t    \\_/\\_/     |______|  |_______| |______|  |_______|  |_|   \\_/   |_|  |______|%s\n", reset);
	printf("\n");
	printf("%s\t\t\t\t\t\t\t   .--.%s       %s_______   _______%s           %s.--.\n", biruneon, reset, hijauneon, reset, biruneon, reset);
    printf("%s\t\t\t\t\t\t\t  | o_o|%s     %s|__   __| |  ___  |%s         %s|o_o | \n", biruneon, reset, hijauneon, reset, biruneon, reset);
    printf("%s\t\t\t\t\t\t\t  | \\_:|%s        %s| |    | |   | |%s         %s|:_/ | \n", biruneon, reset, hijauneon, reset, biruneon, reset);
    printf("%s\t\t\t\t\t\t\t //    \\\\%s       %s| |    | |   | |%s        %s//   \\ \\ \n", biruneon, reset, hijauneon, reset, biruneon, reset);
    printf("%s\t\t\t\t\t\t\t( |     |)%s      %s| |    | |   | |%s       %s(|     | )\n",biruneon, reset, hijauneon, reset, biruneon, reset);
    printf("%s\t\t\t\t\t\t\t/`\\_   _/`\\%s     %s| |    | |___| |%s      %s/'\\_   _/`\\\n",biruneon, reset, hijauneon, reset, biruneon, reset);
    printf("%s\t\t\t\t\t\t\t\\___)=(___/%s     %s|_|    |_______|%s      %s\\___)=(___/\n", biruneon, reset, hijauneon, reset, biruneon, reset);
	printf("\n");
	printf("\t\t\t%s________    _______    ________   ________   ________   ________       _________    _     _    _______    _______                 ", biruneon);
	printf("\t\t\t|  ____ \\  |  ____ |  |  ____  \\  |  ____ \\  | ______|  |  ____  \\     |  ______|  | |   | |  |  ___  |  |  ___  |       \n");
	printf("\t\t\t| |   | |  | |   | |  | |____| |  | |   | |  | |        | |____| |     |  ______   | |___| |  | |   | |  |  |__| |  \n");
	printf("\t\t\t| |___/ /  | |___| |  | |  ___/   | |___/ /  | |_____   | |  ___/      |______  |  |  ___  |  | |   | |  |  _____|       \n");
	printf("\t\t\t| |   \\ \\  |  ___  |  | |-\\ \\     | |   \\ \\  | ______|  | |-\\ \\               | |  | |   | |  | |   | |  | | \n");
	printf("\t\t\t| |___| |  | |   | |  | |  \\ \\    | |___| |  | |______  | |  \\ \\        ______| |  | |   | |  | |___| |  | |    \n");
	printf("\t\t\t|_______/  |_|   |_|  |_|   \\_\\   |_______/  |_______|  |_|   \\_\\      |________|  |_|   |_|  |_______|  |_|    \n%s", reset);
    printf("\n");
    
    printf("%s\t\t\t\t\t\t\t\t   Press any key to continue . . .%s", hijauneon, reset);getchar();
    clearScreen();
    
    int menu, option;
    char newb;
    char c[] = "Haircut", d[] = "Hairdye", p[] = "Perm";
    char a[] = "Basic", s[] = "Super", r[] = "Supreme";
    char cb[] = "Curly Basic", cs[] = "Curly Super", db[] = "Down Basic", ds[] = "Down Super";
    char barb[10][101] = {"Yosuke", "Apis", "Fred", "Laura"};
    loadData();
    
    do{
        int founds = 0, index = 0;
        clearScreen();
        printMenuTable();
        printf("\033[2A"); 
        printf("\033[53C"); 
        scanf("%d", &menu); getchar();
        
        switch(menu){
            case 1:
                clearScreen();
                input = (struct barber *)malloc(sizeof(struct barber));
                do {
                    printf("Input date [DD/MM/YYYY][the year must be 2025 and above]: ");
                    scanf("%d/%d/%d", &input->date, &input->month, &input->year); getchar();
                    if (!isValidDate(input->date, input->month, input->year)) {
                        printf("%sInvalid date! Please input a valid date.\n%s", merah, reset);
                    }
                } while (!isValidDate(input->date, input->month, input->year));
                int id = key(input->date, input->month, input->year);
                
                while(search(id,root)!=NULL) id+=1;
                input->id=id;
                
             
                do {
                    printf("Input customer name: ");
                    scanf("%[^\n]", input->c_name); getchar();
                    if (!isValidName(input->c_name)) {
                        printf("%sInvalid input! Input must be character\n%s", merah, reset);
                    }
                } while(!isValidName(input->c_name));
                do {
                    printf("Input service type [C for %s, D for %s, P for %s]: ", c, d, p);
                    scanf("%s", input->service_type); getchar();
                    upper(input->service_type);
                    if (strcmp(input->service_type, "C") != 0 && strcmp(input->service_type, "D") != 0 && strcmp(input->service_type, "P") != 0) {
                        printf("%sInvalid input!\n%s", merah, reset);
                    }
                } while(strcmp(input->service_type, "C") != 0 && strcmp(input->service_type, "D") != 0 && strcmp(input->service_type, "P") != 0);
                if(strcmp(input->service_type, "C") == 0){
                    char input_lower[101], a_lower[101], s_lower[101], r_lower[101];
                    do {
                        printf("Input %s name [%s, %s, %s]: ", c, a, s, r);
                        scanf("%s", input->service_name); getchar();
                        lower(input->service_name, input_lower);
                        lower(a, a_lower); lower(s, s_lower); lower(r, r_lower);
                        if(strcmp(input_lower, a_lower) != 0 && strcmp(input_lower, s_lower) != 0 && strcmp(input_lower, r_lower) != 0) {
                            printf("%sInvalid Input!\n%s", merah, reset);
                        }
                    } while(strcmp(input_lower, a_lower) != 0 && strcmp(input_lower, s_lower) != 0 && strcmp(input_lower, r_lower) != 0);
                    if (strcmp(input_lower, a_lower) == 0) input->price = 25;
                    else if (strcmp(input_lower, s_lower) == 0) input->price = 50;
                    else if (strcmp(input_lower, r_lower) == 0) input->price = 100;
                }
                else if (strcmp(input->service_type, "D") == 0){
                    char input_lower[101], a_lower[101], s_lower[101];
                    do {
                        printf("Input %s name [%s, %s]: ", d, a, s);
                        scanf("%s", input->service_name); getchar();
                        lower(input->service_name, input_lower);
                        lower(a, a_lower); lower(s, s_lower);
                        if(strcmp(input_lower, a_lower) != 0 && strcmp(input_lower, s_lower) != 0) {
                            printf("%sInvalid Input!\n%s", merah, reset);
                        }
                    } while(strcmp(input_lower, a_lower) != 0 && strcmp(input_lower, s_lower) != 0);
                    if (strcmp(input_lower, a_lower) == 0) input->price = 40;
                    else if (strcmp(input_lower, s_lower) == 0) input->price = 65;
                }
                else if (strcmp(input->service_type, "P") == 0){
                    char input_lower[101], cb_lower[101], cs_lower[101], db_lower[101], ds_lower[101];
                    do {
                        printf("Input %s name [%s, %s, %s, %s]: ", p, cb, cs, db, ds);
                        scanf("%[^\n]", input->service_name); getchar();
                        lower(input->service_name, input_lower);
                        lower(cb, cb_lower); lower(cs, cs_lower); lower(db, db_lower); lower(ds, ds_lower);
                        if(strcmp(input_lower, cb_lower) != 0 && strcmp(input_lower, cs_lower) != 0 && 
                           strcmp(input_lower, db_lower) != 0 && strcmp(input_lower, ds_lower) != 0) {
                            printf("%sInvalid Input!\n%s", merah, reset);
                        }
                    } while(strcmp(input_lower, cb_lower) != 0 && strcmp(input_lower, cs_lower) != 0 && 
                           strcmp(input_lower, db_lower) != 0 && strcmp(input_lower, ds_lower) != 0);
                    if (strcmp(input_lower, cb_lower) == 0) input->price = 250;
                    else if (strcmp(input_lower, cs_lower) == 0) input->price = 275;
                    else if (strcmp(input_lower, db_lower) == 0) input->price = 500;
                    else if (strcmp(input_lower, ds_lower) == 0) input->price = 550;
                }
                founds = 0;
                index = 0;
                do {
                    do {
                        printf("Input barber name: ");
                        scanf("%[^\n]", input->b_name); getchar();
                        if (!isValidName(input->b_name)) {
                            printf("%sInvalid input! please input a valid name\n%s", merah, reset);
                        }
                    } while(!isValidName(input->b_name));
                    founds = 0;
                    char input_b_lower[101], barb_lower[101];
                    lower(input->b_name, input_b_lower);
                    for(int i = 0; strlen(barb[i]) > 0; i++){
                        lower(barb[i], barb_lower);
                        if(strcmp(input_b_lower, barb_lower) == 0){
                            founds = 1;    
                            break;
                        }
                        index = i;
                    }
                    if(founds == 1) break;
                    else {
                        printf("If %s is a new barber [y/n]: ", input->b_name);
                        scanf("%c", &newb); getchar();
                        if(newb == 'y' || newb == 'Y'){
                            strcpy(barb[index+1], input->b_name);
                            break;
                        }
                    }
                } while(1);
                do {
                    printf("Input rate [max 5 characters, only * allowed]: "); 
                    scanf("%s", input->rate); 
                    getchar();
                    if (!isValidRating(input->rate)) {
                        printf("%sInvalid rating! Please use only asterisks (*) and maximum 5 characters.\n%s", merah, reset);
                    }
                } while (!isValidRating(input->rate));
                root = insert(input, root);
                printf("%s\nData added successfully!\n%s", hijauneon, reset);
                pressEnterToContinue();
                break;
            case 2:
            	clearScreen();
                if (root == NULL) {
                    printNoDataTable();
                    pressEnterToContinue();
                    break;
                }
                else{
                printReadOptionsTable();
                scanf("%d", &option);getchar();
                clearScreen();
                viewDoubleTable(root, option);
            }
            pressEnterToContinue();
                break;

            case 3:
            	clearScreen();
                if (root == NULL) {
                    printNoDataTable();
                    pressEnterToContinue();
                    break;
                }
                else{
                viewDoubleTable(root, 1);
                int update_id;
                char confirm_update;
                struct barber *found = NULL;
                do {
                    printf("Input ID of data to update: ");
                    scanf("%d", &update_id); getchar();
                    found = search(update_id, root);
                    if (found == NULL) {
                        printf("%s\nData not found.%s\n", merah, reset);
                        pressEnterToContinue();
                        clearScreen();
                        viewDoubleTable(root, 1);
                        continue;
                    }
                    clearScreen();
                    printf("\nData to update:\n");
                    printDoubleTableHeader();
                    printDoubleTableRow(found);
                    printDoubleTableFooter();
                    printf("Are you sure you want to update this data? (y/n): ");
                    scanf("%c", &confirm_update); getchar();
                    if (confirm_update == 'n' || confirm_update == 'N') {
                        break;
                    }
                } while (confirm_update != 'y' && confirm_update != 'Y');
                if (confirm_update == 'n' || confirm_update == 'N') break;
                printf("\nUpdating data for %s on %02d/%02d/%04d\n", found->c_name, found->date, found->month, found->year);
                
                do {
                    printf("Input new customer name: ");
                    scanf("%[^\n]", found->c_name); getchar();
                    if (!isValidName(found->c_name)) {
                        printf("%sInvalid input! Input must be character\n%s", merah, reset);
                    }
                } while(!isValidName(found->c_name));
                do {
                    printf("Input new service type [C for Haircut, D for Hairdye, P for Perm]: ");
                    scanf("%s", found->service_type); getchar();
                    upper(found->service_type);
                    if (strcmp(found->service_type, "C") != 0 && strcmp(found->service_type, "D") != 0 && strcmp(found->service_type, "P") != 0) {
                        printf("%sInvalid input!\n%s", merah, reset);
                    }
                } while(strcmp(found->service_type, "C") != 0 && strcmp(found->service_type, "D") != 0 && strcmp(found->service_type, "P") != 0);
                if(strcmp(found->service_type, "C") == 0){
                    char input_lower[101], a_lower[101], s_lower[101], r_lower[101];
                    char a[] = "Basic", s[] = "Super", r[] = "Supreme";
                    do {
                        printf("Input Haircut name [Basic, Super, Supreme]: ");
                        scanf("%s", found->service_name); getchar();
                        lower(found->service_name, input_lower);
                        lower(a, a_lower); lower(s, s_lower); lower(r, r_lower);
                        if(strcmp(input_lower, a_lower) != 0 && strcmp(input_lower, s_lower) != 0 && strcmp(input_lower, r_lower) != 0) {
                            printf("%sInvalid Input!\n%s", merah, reset);
                        }
                    } while(strcmp(input_lower, a_lower) != 0 && strcmp(input_lower, s_lower) != 0 && strcmp(input_lower, r_lower) != 0);
                    if (strcmp(input_lower, a_lower) == 0) found->price = 25;
                    else if (strcmp(input_lower, s_lower) == 0) found->price = 50;
                    else if (strcmp(input_lower, r_lower) == 0) found->price = 100;
                }
                else if (strcmp(found->service_type, "D") == 0){
                    char input_lower[101], a_lower[101], s_lower[101];
                    char a[] = "Basic", s[] = "Super";
                    do {
                        printf("Input Hairdye name [Basic, Super]: ");
                        scanf("%s", found->service_name); getchar();
                        lower(found->service_name, input_lower);
                        lower(a, a_lower); lower(s, s_lower);
                        if(strcmp(input_lower, a_lower) != 0 && strcmp(input_lower, s_lower) != 0) {
                            printf("%sInvalid Input!\n%s", merah, reset);
                        }
                    } while(strcmp(input_lower, a_lower) != 0 && strcmp(input_lower, s_lower) != 0);
                    if (strcmp(input_lower, a_lower) == 0) found->price = 40;
                    else if (strcmp(input_lower, s_lower) == 0) found->price = 65;
                }
                else if (strcmp(found->service_type, "P") == 0){
                    char input_lower[101], cb_lower[101], cs_lower[101], db_lower[101], ds_lower[101];
                    char cb[] = "Curly Basic", cs[] = "Curly Super", db[] = "Down Basic", ds[] = "Down Super";
                    do {
                        printf("Input Perm name [Curly Basic, Curly Super, Down Basic, Down Super]: ");
                        scanf("%[^\n]", found->service_name); getchar();
                        lower(found->service_name, input_lower);
                        lower(cb, cb_lower); lower(cs, cs_lower); lower(db, db_lower); lower(ds, ds_lower);
                        if(strcmp(input_lower, cb_lower) != 0 && strcmp(input_lower, cs_lower) != 0 && 
                           strcmp(input_lower, db_lower) != 0 && strcmp(input_lower, ds_lower) != 0) {
                            printf("%sInvalid Input!\n%s", merah, reset);
                        }
                    } while(strcmp(input_lower, cb_lower) != 0 && strcmp(input_lower, cs_lower) != 0 && 
                           strcmp(input_lower, db_lower) != 0 && strcmp(input_lower, ds_lower) != 0);
                    if (strcmp(input_lower, cb_lower) == 0) found->price = 250;
                    else if (strcmp(input_lower, cs_lower) == 0) found->price = 275;
                    else if (strcmp(input_lower, db_lower) == 0) found->price = 500;
                    else if (strcmp(input_lower, ds_lower) == 0) found->price = 550;
                }
                int found_barber = 0, index_barber = 0;
                char update_newb;
              
                do {
                    do {
                        printf("Input barber name: ");
                        scanf("%[^\n]", found->b_name); getchar();
                        if (!isValidName(found->b_name)) {
                            printf("%sInvalid input! please input a valid name\n%s", merah, reset);
                        }
                    } while(!isValidName(found->b_name));
                    found_barber = 0;
                    char found_b_lower[101], barb_lower[101];
                    lower(found->b_name, found_b_lower);
                    for (int i = 0; strlen(barb[i]) > 0; i++) {
                        lower(barb[i], barb_lower);
                        if (strcmp(found_b_lower, barb_lower) == 0) {
                            found_barber = 1;
                            break;
                        }
                        index_barber = i;
                    }
                    if (found_barber) break;
                    else {
                        printf("If %s is a new barber [y/n]: ", found->b_name);
                        scanf("%c", &update_newb); getchar();
                        if (update_newb == 'y' || update_newb == 'Y') {
                            strcpy(barb[index_barber + 1], found->b_name);
                            break;
                        }
                    }
                } while (1);
                do {
                    printf("Input rate [max 5 characters, only * allowed]: ");
                    scanf("%s", found->rate); getchar();
                    if (!isValidRating(found->rate)) {
                        printf("%sInvalid rating! Please use only asterisks (*) and maximum 5 characters.\n%s", merah, reset);
                    }
                } while (!isValidRating(found->rate));
                printf("%s\nData successfully updated!\n%s", hijauneon, reset);
            }
                pressEnterToContinue();
                break;
        
            case 4:
            	clearScreen();
                if(root == NULL) {
                    printNoDataTable();
                    pressEnterToContinue();
                    break;
                }
                else{
                clearScreen();
                viewDoubleTable(root, 1); 
                int del_id; 
                char confirm_delete;
                struct barber *found = NULL;
                do {
                    printf("Input ID of data to delete: ");
                    scanf("%d", &del_id); getchar(); 
                    found = search(del_id, root);
                    if(found == NULL) {
                        printf("\n%sData not found.\n%s", merah,reset);
                        pressEnterToContinue();
                        clearScreen();
                        viewDoubleTable(root, 1);
                        continue;
                    }
                    clearScreen();
                    printf("\nData to delete:\n");
                    printDoubleTableHeader();
                    printDoubleTableRow(found);
                    printDoubleTableFooter();
                    printf("Are you sure you want to delete this data? (y/n): ");
                    scanf("%c", &confirm_delete); getchar();
                    if (confirm_delete == 'n' || confirm_delete == 'N') {
                        break;
                    }
                } while (confirm_delete != 'y' && confirm_delete != 'Y');
                if (confirm_delete == 'n' || confirm_delete == 'N') break;
                root = deletion(root, del_id);
                printf("%s\nData deleted successfully.\n%s", hijauneon, reset);
            }
                pressEnterToContinue();
                break;

			case 5:
				clearScreen();
                if (root == NULL) {
                    printNoDataTable();
                    pressEnterToContinue();
                    break;
                }
                else{
                processData();
            }
            pressEnterToContinue();
                break;
        
            case 6:
                stopsound();
                saveData();
                printf("Exiting program...\n");                
        }
       
    } while(menu != 6);
    return 0;
}
