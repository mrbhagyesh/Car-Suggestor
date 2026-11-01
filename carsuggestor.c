#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define MAX_COLUMNS 21
#define MAX_LINE 2048

typedef struct {
    char car_name[100];
    char brand_name[50];
    long long price;
    char variant_name[200];
    char fuel_type[30];
    char transmission_type[50];
    char body_type[30];
} Car;

/* CSV rows including quoted fields containing commas. */

int parseCSV(char *line, char *fields[], int maxFields)
{
    char *src = line;
    char *dest = line;
    int count = 0;

    while (count < maxFields) {

        fields[count++] = dest;

        if (*src == '"') {
            src++;

            while (*src != '\0') {

                if (*src == '"') {
                    if (*(src + 1) == '"') {
                        *dest++ = '"';
                        src += 2;
                    } else {
                        src++;
                        break;
                    }
                } else {
                    *dest++ = *src++;
                }
            }
        } else {
            while (*src != '\0' &&
                   *src != ',' &&
                   *src != '\n' &&
                   *src != '\r') {

                *dest++ = *src++;
            }
        }

        char delimiter = *src;
        *dest++ = '\0';

        if (delimiter == ',') {
            src++;
        } else {
            break;
        }
    }

    return count;
}

/* Compare user preferences with database values. */

int matchesPreference(char *userChoice, char *carValue)
{
    if (strcasecmp(userChoice, "Any") == 0) {
        return 1;
    }

    return strcasecmp(userChoice, carValue) == 0;
}

int main(void)
{
    FILE *file = fopen("Database.csv", "r");

    if (file == NULL) {
        printf("Error: Could not open Database.csv\n");
        return 1;
    }

    char line[MAX_LINE];
    char *fields[MAX_COLUMNS];

    /* Skip CSV header */

    if (fgets(line, sizeof(line), file) == NULL) {
        printf("Database is empty.\n");
        fclose(file);
        return 1;
    }

    /* User preferences */

    char brandPreference[50];
    char fuelPreference[30];
    char transmissionPreference[50];
    char bodyPreference[30];

    float budget;

    printf("\n========== CARSUGGESTER ==========\n");

    printf("\nEnter your maximum budget (in lakhs): ");
    if (scanf("%f", &budget) != 1 || budget < 0) {
        printf("Invalid budget.\n");
        fclose(file);
        return 1;
    }

    getchar();

    printf("\nEnter preferred brand (Any for all): ");
    fgets(brandPreference, sizeof(brandPreference), stdin);
    brandPreference[strcspn(brandPreference, "\n")] = '\0';

    printf("\nEnter preferred fuel (Any for all): ");
    fgets(fuelPreference, sizeof(fuelPreference), stdin);
    fuelPreference[strcspn(fuelPreference, "\n")] = '\0';

    printf("\nEnter preferred transmission (Any for all): ");
    fgets(transmissionPreference, sizeof(transmissionPreference), stdin);
    transmissionPreference[strcspn(transmissionPreference, "\n")] = '\0';

    printf("\nEnter preferred body type (Any for all): ");
    fgets(bodyPreference, sizeof(bodyPreference), stdin);
    bodyPreference[strcspn(bodyPreference, "\n")] = '\0';

    printf("\n========== YOUR PREFERENCES ==========\n");
    printf("Budget: %.2f lakhs\n", budget);
    printf("Brand: %s\n", brandPreference);
    printf("Fuel: %s\n", fuelPreference);
    printf("Transmission: %s\n", transmissionPreference);
    printf("Body type: %s\n", bodyPreference);

    printf("\n========== RECOMMENDED CARS ==========\n");

    int row_count = 0;
    int matched_count = 0;

    while (fgets(line, sizeof(line), file) != NULL) {

        int totalColumns = parseCSV(line, fields, MAX_COLUMNS);

        /* We need at least columns 0 to 18. */

        if (totalColumns <= 18) {
            continue;
        }

        row_count++;

        /* Read price from CSV (price is in lakhs). */

        char *endptr;
        double priceInLakhs = strtod(fields[18], &endptr);

        if (endptr == fields[18] || priceInLakhs <= 0) {
            continue;
        }

        /* Check all five preferences. */

        if (priceInLakhs <= budget &&
            matchesPreference(brandPreference, fields[4]) &&
            matchesPreference(fuelPreference, fields[15]) &&
            matchesPreference(transmissionPreference, fields[17]) &&
            matchesPreference(bodyPreference, fields[7])) {

            Car car = {0};

            snprintf(car.car_name, sizeof(car.car_name), "%s", fields[5]);
            snprintf(car.brand_name, sizeof(car.brand_name), "%s", fields[4]);
            snprintf(car.variant_name, sizeof(car.variant_name), "%s", fields[12]);
            snprintf(car.fuel_type, sizeof(car.fuel_type), "%s", fields[15]);
            snprintf(car.transmission_type, sizeof(car.transmission_type), "%s", fields[17]);
            snprintf(car.body_type, sizeof(car.body_type), "%s", fields[7]);

            car.price = (long long)(priceInLakhs * 100000.0 + 0.5);

            matched_count++;

            printf("\nCar Details\n");
            printf("------------------------------\n");
            printf("Car Name: %s\n", car.car_name);
            printf("Brand: %s\n", car.brand_name);
            printf("Price: Rs. %.2f lakhs\n", priceInLakhs);
            printf("Variant: %s\n", car.variant_name);
            printf("Fuel: %s\n", car.fuel_type);
            printf("Transmission: %s\n", car.transmission_type);
            printf("Body Type: %s\n", car.body_type);
        }
    }

    fclose(file);

    printf("\n========== SEARCH SUMMARY ==========\n");
    printf("Configurations processed: %d\n", row_count);
    printf("Matching cars found: %d\n", matched_count);

    if (matched_count == 0) {
        printf("\nSorry! No cars match your preferences.\n");
        printf("Try increasing your budget or changing your preferences.\n");
    }

    return 0;
}
