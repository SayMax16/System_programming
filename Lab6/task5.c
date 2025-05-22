#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define MAX_LINE 1024
#define MAX_FIELDS 10
#define MAX_FIELD_LENGTH 100
#define MAX_CATEGORIES 10

// Structure to hold parsed data
typedef struct {
    char name[MAX_FIELD_LENGTH];
    int id;
    double value;
    char category[MAX_FIELD_LENGTH];
    char timestamp[MAX_FIELD_LENGTH];
} ParsedData;

// Structure to hold validation rules
typedef struct {
    int min_id;
    int max_id;
    double min_value;
    double max_value;
    char valid_categories[MAX_CATEGORIES][MAX_FIELD_LENGTH];
    int num_categories;
} ValidationRules;

// Structure to hold field information
typedef struct {
    char name[MAX_FIELD_LENGTH];
    char type[MAX_FIELD_LENGTH];
    int num_categories;
    char valid_categories[MAX_CATEGORIES][MAX_FIELD_LENGTH];
} Field;

// Function to initialize validation rules
void init_validation_rules(ValidationRules *rules) {
    rules->min_id = 1;
    rules->max_id = 999999;
    rules->min_value = 0.0;
    rules->max_value = 1000000.0;
    
    const char *categories[] = {
        "A", "B", "C", "D", "E"
    };
    rules->num_categories = sizeof(categories) / sizeof(categories[0]);
    
    for (int i = 0; i < rules->num_categories; i++) {
        strncpy(rules->valid_categories[i], categories[i], MAX_FIELD_LENGTH - 1);
        rules->valid_categories[i][MAX_FIELD_LENGTH - 1] = '\0';
    }
}

// Function to validate parsed data
int validate_data(const ParsedData *data, const ValidationRules *rules) {
    // Validate ID
    if (data->id < rules->min_id || data->id > rules->max_id) {
        fprintf(stderr, "Error: Invalid ID %d (must be between %d and %d)\n",
                data->id, rules->min_id, rules->max_id);
        return 0;
    }
    
    // Validate value
    if (data->value < rules->min_value || data->value > rules->max_value) {
        fprintf(stderr, "Error: Invalid value %.2f (must be between %.2f and %.2f)\n",
                data->value, rules->min_value, rules->max_value);
        return 0;
    }
    
    // Validate category
    int valid_category = 0;
    for (int i = 0; i < rules->num_categories; i++) {
        if (strcmp(data->category, rules->valid_categories[i]) == 0) {
            valid_category = 1;
            break;
        }
    }
    if (!valid_category) {
        fprintf(stderr, "Error: Invalid category '%s'\n", data->category);
        return 0;
    }
    
    // Validate timestamp format (YYYY-MM-DD HH:MM:SS)
    if (strlen(data->timestamp) != 19) {
        fprintf(stderr, "Error: Invalid timestamp format\n");
        return 0;
    }
    
    // Check timestamp characters
    for (int i = 0; i < 19; i++) {
        if (i == 4 || i == 7) {
            if (data->timestamp[i] != '-') {
                fprintf(stderr, "Error: Invalid timestamp format (expected '-')\n");
                return 0;
            }
        } else if (i == 10) {
            if (data->timestamp[i] != ' ') {
                fprintf(stderr, "Error: Invalid timestamp format (expected space)\n");
                return 0;
            }
        } else if (i == 13 || i == 16) {
            if (data->timestamp[i] != ':') {
                fprintf(stderr, "Error: Invalid timestamp format (expected ':')\n");
                return 0;
            }
        } else {
            if (!isdigit(data->timestamp[i])) {
                fprintf(stderr, "Error: Invalid timestamp format (expected digit)\n");
                return 0;
            }
        }
    }
    
    return 1;
}

// Function to parse line into ParsedData structure
int parse_line(const char *line, ParsedData *data) {
    char name[MAX_FIELD_LENGTH];
    int id;
    double value;
    char category[MAX_FIELD_LENGTH];
    char timestamp[MAX_FIELD_LENGTH];
    
    // Parse line using sscanf
    int result = sscanf(line, "%[^,],%d,%lf,%[^,],%s",
                       name, &id, &value, category, timestamp);
    
    if (result != 5) {
        fprintf(stderr, "Error: Invalid line format\n");
        return 0;
    }
    
    // Copy parsed data to structure
    strncpy(data->name, name, MAX_FIELD_LENGTH - 1);
    data->name[MAX_FIELD_LENGTH - 1] = '\0';
    data->id = id;
    data->value = value;
    strncpy(data->category, category, MAX_FIELD_LENGTH - 1);
    data->category[MAX_FIELD_LENGTH - 1] = '\0';
    strncpy(data->timestamp, timestamp, MAX_FIELD_LENGTH - 1);
    data->timestamp[MAX_FIELD_LENGTH - 1] = '\0';
    
    return 1;
}

// Function to print parsed data
void print_parsed_data(const ParsedData *data) {
    printf("\nParsed Data:\n");
    printf("Name: %s\n", data->name);
    printf("ID: %d\n", data->id);
    printf("Value: %.2f\n", data->value);
    printf("Category: %s\n", data->category);
    printf("Timestamp: %s\n", data->timestamp);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    
    // Open input file
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening input file");
        return 1;
    }
    
    // Initialize validation rules
    ValidationRules rules;
    init_validation_rules(&rules);
    
    // Process file line by line
    char line[MAX_LINE];
    int line_number = 0;
    int valid_lines = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line_number++;
        
        // Remove newline character
        line[strcspn(line, "\n")] = '\0';
        
        // Skip empty lines
        if (strlen(line) == 0) continue;
        
        // Parse line
        ParsedData data;
        if (!parse_line(line, &data)) {
            fprintf(stderr, "Error parsing line %d\n", line_number);
            continue;
        }
        
        // Validate data
        if (!validate_data(&data, &rules)) {
            fprintf(stderr, "Error validating line %d\n", line_number);
            continue;
        }
        
        // Print valid data
        print_parsed_data(&data);
        valid_lines++;
    }
    
    fclose(file);
    
    printf("\nProcessing complete:\n");
    printf("Total lines processed: %d\n", line_number);
    printf("Valid lines: %d\n", valid_lines);
    printf("Invalid lines: %d\n", line_number - valid_lines);
    
    return 0;
} 