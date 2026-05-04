    #include <stdio.h>
    #include <stdlib.h>
    #include "s2html_event.h"
    #include "s2html_conv.h"

    /* main function */

    int main (int argc, char *argv[])
    {
        FILE *sfp, *dfp;           // Source file pointer and destination file pointer
        pevent_t *event;           // Pointer to parser event
        char dest_file[100];       // Output file name

        // Check if input file is provided
        if (argc < 2)
        {
            printf("\nError! please enter file name\n");
            printf("Usage: <executable> <file name>\n");
            printf("Example : ./a.out test.c\n\n");
            return 1;   
        }

    #ifdef DEBUG
        // Debug: print file name
        printf("File to be opened : %s\n", argv[1]);
    #endif

        // Open source file in read mode
        sfp = fopen(argv[1], "r");
        if (sfp == NULL)
        {
            printf("Error! File %s could not be opened\n", argv[1]);
            return 2;
        }

        // Generate output file name
        if (argc > 2)
        {
            sprintf(dest_file, "%s.html", argv[2]);
        }
        else
        {
            sprintf(dest_file, "%s.html", argv[1]);
        }

        // Open destination file in write mode
        dfp = fopen(dest_file, "w");
        if (dfp == NULL)
        {
            printf("Error! could not create %s output file\n", dest_file);
            fclose(sfp);   // Close source file before exit
            return 3;
        }

        // Write starting HTML structure
        html_begin(dfp, HTML_OPEN);

        // Read source file, parse and convert to HTML
        do
        {
            // Get next parsed event from source file
            event = get_parser_event(sfp);

            // Safety check before processing
            if (event != NULL)
            {
                source_to_html(dfp, event);
            }

        } while (event->type != PEVENT_EOF);   // Continue until end of file

        // Write ending HTML structure
        html_end(dfp, HTML_CLOSE);

        // Display success message
        printf("\nOutput file %s generated\n", dest_file);

        // Close both files
        fclose(sfp);
        fclose(dfp);

        return 0;
    }