//Luke Caprio
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_LENGTH 128
#define MAX_BOATS 120

typedef char String[STRING_LENGTH];

typedef union {
    int slipNumber;
    char bayLetter;
    char tag[20];
    int storageNumber;
} StorageInfo;

typedef enum {
    slip,
    land,
    trailor, //I think it should be spelled trailer but it was
    //spelled trailor in the directions so I just used it
    storage,
    error
} StorageType;

typedef struct {
    String name;
    float length;
    StorageType type;
    StorageInfo info;
    float debt;
} Boat;

//Malloc wrapper function
//-----------------------------------------------------------------------------
void* Malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    return ptr;
}

//converts the type to string
//-----------------------------------------------------------------------------
char* convertTypeToString(Boat* boat) {

    switch (boat->type) {
        case slip:
            return("slip");
            break;
        case land:
            return("land");
            break;
        case trailor:
            return("trailor");
            break;
        case storage:
            return("storage");
            break;
        default:
            return("error");
            break;
    }
}

//takes one line and splits it up by commas 
//-----------------------------------------------------------------------------
void assignBoatValues(char *line, Boat *boat) {

    char temp[100];
    char type[10];
    char extra_info[20];
    strcpy(temp,line);

    char *token = strtok(temp, ",");
    strcpy(boat->name,token);

    token = strtok(NULL, ",");
    boat->length = atof(token);

    token = strtok(NULL, ",");
    strcpy(type,token);

    token = strtok(NULL, ",");
    strcpy(extra_info, token);

    token = strtok(NULL, ",");
    boat->debt = atof(token);

    if (strcmp(type, "slip") == 0) {
        boat->type = slip;
        boat->info.slipNumber = atoi(extra_info);
    } else if (strcmp(type, "land") == 0) {
        boat->type = land;
        boat->info.bayLetter = extra_info[0];
    } else if (strcmp(type, "trailor") == 0) {
        boat->type = trailor;
        strcpy(boat->info.tag, extra_info);
    } else if (strcmp(type, "storage") == 0) {
        boat->type = storage;
        boat->info.storageNumber = atoi(extra_info);
    } else {
        boat->type = error;
    }
}

//creates and adds a boat to the boatArray
//-----------------------------------------------------------------------------
void addBoat(Boat** boatArray, int* boatCount, char *csv) {
    if (*boatCount >= MAX_BOATS) {
        printf("Cannot add more boats, marina is full\n");
        return;
    }
    
    Boat *boat = (Boat*)Malloc(sizeof(Boat));
    assignBoatValues(csv, boat);

    boatArray[*boatCount] = boat;
    (*boatCount)++;
}

//Finds the boat, then removes it by shifting array over
//-----------------------------------------------------------------------------

void removeBoat(Boat** boatArray, int* boatCount, char *boatName) {
    int boatIndex = -1;
    int i;
    int j;

    for (i = 0; i < *boatCount; i++) {
        if (strcasecmp(boatArray[i]->name,boatName) == 0) {
            boatIndex = i;
            break;
        }
    }

    if (boatIndex == -1){
        printf("No boat with that name\n");
        return;
    }

    //free memory and shift array over
    free(boatArray[boatIndex]);
    for (j = boatIndex; j < *boatCount - 1; j ++) {
        boatArray[j] = boatArray[j+1]; 
    }

    (*boatCount)--;
}

//Makes a payment for boat in the boatArray, doesn't make payment
//if the payment is more than the boat's depth
//-----------------------------------------------------------------------------
void makePayment(Boat** boatArray, int boatCount, char *boatName) {
    int boatIndex = -1;
    int i;

    for (i = 0; i < boatCount; i++) {
        if (strcasecmp(boatArray[i]->name,boatName) == 0) {
            boatIndex = i;
            break;
        }
    }

    if (boatIndex == -1){
        printf("No boat with that name\n");
        return;
    }

    float payment;
    double currentDebt = boatArray[boatIndex]->debt;
    printf("Please enter the amount to be paid                       : ");
    scanf("%f", &payment);
    
    if (payment > currentDebt) {
        printf("That is more than the amount owed, $%.2f\n",currentDebt);
        return;
    }

    currentDebt = currentDebt - payment;
    boatArray[boatIndex]->debt = currentDebt;
}

//advances month and calculates how much each boat owes and adds it to the debt
//-----------------------------------------------------------------------------
void advanceMonth(Boat** boatArray, int boatCount) {
    int i;
    Boat* currentBoat;

    for (i = 0; i < boatCount; i ++) {
        currentBoat = boatArray[i];
        switch (currentBoat->type) {
        case slip:
            currentBoat->debt = currentBoat->debt + (12.50*currentBoat->length);
            break;
        case land:
            currentBoat->debt = currentBoat->debt + (14.00*currentBoat->length);
            break;
        case trailor:
            currentBoat->debt = currentBoat->debt + (25.00*currentBoat->length);
            break;
        case storage:
            currentBoat->debt = currentBoat->debt + (11.20*currentBoat->length);
            break;
        default:
            return;
            break;
    }
    }
}

//From a file, it creates and adds every boat into the boatArray
//-----------------------------------------------------------------------------

void loadBoats(char* Boatfile, Boat** boatArray, int* boatCount) {
    FILE* file = fopen(Boatfile, "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        addBoat(boatArray, boatCount, line);
    }

    fclose(file);
}

//rewrites the file based off the boats currently in boatArray
//-----------------------------------------------------------------------------
void saveBoats(char* Boatfile, Boat** boatArray, int boatCount) {
    int i;
    FILE* file = fopen(Boatfile, "w");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    
    for (i = 0; i < boatCount; i ++) {
        Boat* currentBoat = boatArray[i];

        switch (currentBoat->type) {
            case slip:
                fprintf(file, "%s,%.2f,slip,%d,%.2f\n", 
                currentBoat->name, currentBoat->length, currentBoat->info.slipNumber, currentBoat->debt);
                break;
            case land:
                fprintf(file, "%s,%.2f,slip,%c,%.2f\n", 
                currentBoat->name, currentBoat->length, currentBoat->info.bayLetter, currentBoat->debt);
                break;
            case trailor:
                fprintf(file, "%s,%.2f,slip,%s,%.2f\n", 
                currentBoat->name, currentBoat->length, currentBoat->info.tag, currentBoat->debt);
                break;
            case storage:
                fprintf(file, "%s,%.2f,slip,%d,%.2f\n", 
                currentBoat->name, currentBoat->length, currentBoat->info.storageNumber, currentBoat->debt);
                break;
            default:
                printf("Error %s has an invalid type\n",currentBoat->name);
                break;
        }
    }

    fclose(file);
}

//Compare function for qsort
//-----------------------------------------------------------------------------
int compareBoats(const void* boat1, const void* boat2) {
    Boat* boatA = *(Boat**)boat1;
    Boat* boatB = *(Boat**)boat2;
    return strcasecmp(boatA->name, boatB->name);
}

//Prints all the boats in boatArray, also sorts the array
//-----------------------------------------------------------------------------
void printInventory(Boat** boatArray, int boatCount) {
    int i;
    Boat* currentBoat;
    qsort(boatArray, boatCount, sizeof(Boat*), compareBoats);
    
    for (i = 0; i < boatCount; i++) {
        currentBoat = boatArray[i];

        printf("%-20s %2.0f'    %-8s ", currentBoat->name, currentBoat->length,
        convertTypeToString(currentBoat));

        switch (currentBoat->type) {
            case slip:
                printf("# %-5d ", currentBoat->info.slipNumber);
                break;
            case land:
                printf("%-2c      ", currentBoat->info.bayLetter);
                break;
            case trailor:
                printf("%-7s ", currentBoat->info.tag);
                break;
            case storage:
                printf("# %-5d ", currentBoat->info.storageNumber);
                break;
            default:
                printf("error ");
                break;
        }

        printf("Owes $%8.2f\n", currentBoat->debt);
    }
}

//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    int boatCount = 0;
    Boat** boatArray = (Boat**)Malloc(MAX_BOATS * sizeof(Boat*));
    char newBoatLine[STRING_LENGTH];
    char boatName[STRING_LENGTH];
    char inputChar;
    int i;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <BoatData.csv>\n", argv[0]);
        return 1;
    }

    printf("\nWelcome to the Boat Management System\n");
    printf("-------------------------------------\n");

    //load up boats from csv
    loadBoats(argv[1], boatArray, &boatCount);

    //menu options
    while (1) {
        printf("(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, e(X)it : ");
        scanf(" %c", &inputChar);
        getchar();
        switch (inputChar) {
            case 'I':
            case 'i':
                //inventory
                printInventory(boatArray, boatCount);
                break;
            case 'A':
            case 'a':
                //add 
                printf("Please enter the boat data in CSV format                 : ");
                if (fgets(newBoatLine, sizeof(newBoatLine), stdin) != NULL) {
                    newBoatLine[strlen(newBoatLine) - 1] = '\0';
                    addBoat(boatArray, &boatCount, newBoatLine);
                }
                break;
            case 'R':
            case 'r':
                //remove
                printf("Please enter the boat name                               : ");
                if (fgets(boatName, sizeof(boatName), stdin) != NULL) {
                    boatName[strlen(boatName) - 1] = '\0';
                    removeBoat(boatArray, &boatCount, boatName);
                }
                break;
            case 'P':
            case 'p':
                //payment
                printf("Please enter the boat name                               : ");
                if (fgets(boatName, sizeof(boatName), stdin) != NULL) {
                    boatName[strlen(boatName) - 1] = '\0';
                    makePayment(boatArray, boatCount, boatName);
                }
                break;
            case 'M':
            case 'm':
                //month
                advanceMonth(boatArray, boatCount);
                break;
            case 'X':
            case 'x':
                //exit
                saveBoats(argv[1],boatArray, boatCount);
                printf("\nExiting the Boat Management System\n");
                //free up memory of the boats then array
                for (i = 0; i < boatCount; i++) {
                    free(boatArray[i]);
                }
                free(boatArray);
                return 0;
            default:
                printf("Invalid option %c\n", inputChar);
                break;
        }
        printf("\n");
    }

}