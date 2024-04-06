#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON.h"

// this struct makes variable length strings possible
struct string {
    char *ptr;
    size_t len;
};

// initializes a string with length 0, and malloc of 1 bit '\0\'
void initialize_string(struct string *input_string) {
    input_string->len = 0;
    input_string->ptr = malloc(input_string->len + 1);
    if (input_string->ptr == NULL) {
        perror("Failed to allocate memory");
        exit(1);
    }
    input_string->ptr[0] = '\0';
}

// allows for dynamic changing string, realloc memory and updates length in struct
size_t write_string(void *ptr, size_t size, size_t new_size, struct string *s) {
    size_t new_len = s->len + size * new_size;
    s->ptr = realloc(s->ptr, new_len + 1);
    if (s->ptr == NULL) {
        perror("Failed to allocate memory");
        exit(1);
    }

    // copy bytes of memory from location pointed to by source to memory block pointed to by destination
    memcpy(s->ptr + s->len, ptr, size * new_size);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size * new_size;
}

int main(void) {
    CURL *curl;
    CURLcode res;
    struct string s;
    char url[256];

    // ************************************************************
    // Make user input currency to be converted, currency targets
    // ************************************************************
    //char *base_currency = "CAD"; // Variable for base currency
    //char currencies[] = "USD,EUR";
    char base_currency[4];
    char target_currency[256];
    double amount_to_convert, converted_amount;

    printf("___________________________________\n");
    printf("|#######====================#######|\n");
    printf("|#(1)*       BCIT CST         *(1)#|\n");
    printf("|#**          /===\\   ********  **#|\n");
    printf("|*# MONEY    | (\") |             #*|\n");
    printf("|#*  ******  | /v\\ |  EXCHANGE   *#|\n");
    printf("|#(1)         \\===/            (1)#|\n");
    printf("|##=========ONE DOLLAR===========##|\n");
    printf("------------------------------------\n");
    printf("Welcome to the Exchange Rate Calculator!\n");
    printf("Please enter the currency codes in uppercase, as provided below.\n");
    printf("Code\tName\n");
    printf("----\t--------------\n");
    printf("EUR\tEuro\n");
    printf("USD\tUS Dollar\n");
    printf("JPY\tJapanese Yen\n");
    printf("BGN\tBulgarian Lev\n");
    printf("CZK\tCzech Republic Koruna\n");
    printf("DKK\tDanish Krone\n");
    printf("GBP\tBritish Pound Sterling\n");
    printf("HUF\tHungarian Forint\n");
    printf("PLN\tPolish Zloty\n");
    printf("RON\tRomanian Leu\n");
    printf("SEK\tSwedish Krona\n");
    printf("CHF\tSwiss Franc\n");
    printf("ISK\tIcelandic Króna\n");
    printf("NOK\tNorwegian Krone\n");
    printf("HRK\tCroatian Kuna\n");
    printf("RUB\tRussian Ruble\n");
    printf("TRY\tTurkish Lira\n");
    printf("AUD\tAustralian Dollar\n");
    printf("BRL\tBrazilian Real\n");
    printf("CAD\tCanadian Dollar\n");
    printf("CNY\tChinese Yuan\n");
    printf("HKD\tHong Kong Dollar\n");
    printf("IDR\tIndonesian Rupiah\n");
    printf("ILS\tIsraeli New Sheqel\n");
    printf("INR\tIndian Rupee\n");
    printf("KRW\tSouth Korean Won\n");
    printf("MXN\tMexican Peso\n");
    printf("MYR\tMalaysian Ringgit\n");
    printf("NZD\tNew Zealand Dollar\n");
    printf("PHP\tPhilippine Peso\n");
    printf("SGD\tSingapore Dollar\n");
    printf("THB\tThai Baht\n");
    printf("ZAR\tSouth African Rand\n");
    printf("\n");
    printf("Enter the base currency (e.g., CAD,USD): ");
    scanf("%3s", base_currency);
    getchar();

    printf("Enter the target currencies, separated by commas (e.g., EUR,JPY): ");
    scanf("%3s", target_currency);
    getchar();

    printf("Enter the amount to convert: ");
    scanf("%lf", &amount_to_convert);

    initialize_string(&s);

    // Construct URL with dynamic variables
    snprintf(url, sizeof(url),
             "https://api.freecurrencyapi.com/v1/latest?apikey=fca_live_Ot56v5jsJHoJOvVbtLCUk1NCZTDdVKBc0TZ89SYa&base_currency=%s&currencies=%s",
             base_currency, target_currency);

    // initializes CURL session to make HTTP request
    curl = curl_easy_init();
    if (curl) {
        // configure curl with url
        curl_easy_setopt(curl, CURLOPT_URL, url);
        // set write_string function to handle response data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_string);
        // store response to s
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        // result of operation stored in res
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            printf("Curl failed\n", curl_easy_strerror(res));
        } else {
            // Parse json to cjson struct
            cJSON *json = cJSON_Parse(s.ptr);
            if (json != NULL) {
                // think dictionary and key "data" but in C
                cJSON *data = cJSON_GetObjectItemCaseSensitive(json, "data");
                if (data != NULL) {
                    // Iterate through the currencies specified and stopping at ',' eg. USD,EUR = 2 iterations
                    // token is pointer, pointing to beginning of currency segment
                    // replace , with null
//                    char *token = strtok(currencies, ",");
//                    // stops when reach last index '\0'
//                    while(token != NULL) {
//                        // use token to get specific exchange rate
//                        cJSON *rate = cJSON_GetObjectItemCaseSensitive(data, token);
//                        if(cJSON_IsNumber(rate)) {
//                            printf("%s to %s Exchange Rate: %f\n", base_currency, token, rate->valuedouble);
//                        }
//                        token = strtok(NULL, ",");
//                    }
                    cJSON *rate = cJSON_GetObjectItemCaseSensitive(data, target_currency);
                    if (rate != NULL && cJSON_IsNumber(rate)) {
                        converted_amount = amount_to_convert * rate->valuedouble;
                        printf("%.2f %s is equivalent to %.2f %s\n", amount_to_convert, base_currency, converted_amount,
                               target_currency);
                    } else {
                        printf("Failed to find rate for %s\n", target_currency);
                    }
                } else {
                    printf("Data not found in response.\n");
                }
                // release memory allocated to json
                cJSON_Delete(json);
            } else {
                printf("Failed to parse JSON.\n");
            }

        }

        // release memory allocation of string in struct
        free(s.ptr);
        // release resources allocated to curl
        curl_easy_cleanup(curl);
    }

    return 0;
}

