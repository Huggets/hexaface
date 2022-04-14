#ifdef HXF_WIN32
#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include "app.h"

/**
 * @brief Handle the parameters given to the command line to convert them to HxfAppParam.
 *
 * @param pCmdLine The window command line arguments.
 * @param appParam A pointer to the HxfAppParam that will receive the arguments.
 */
void handleParamaters(HxfAppParam* restrict appParam, int argc, char** argv) {
    char** currentArgument = argv;

    int i = 0;
    while (i != argc) {
        if (strcmp(*currentArgument, "--width") == 0) {
            if (i == argc - 1) {
                fprintf(stderr, "No argument specified for width\n");
                exit(EXIT_FAILURE);
            }
            else {
                currentArgument++;
                i++;
                appParam->windowWidth = strtoul(*currentArgument, NULL, 10);
                if (appParam->windowWidth <= 0) {
                    fprintf(stderr, "Wrong window width given\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
        else if (strcmp(*currentArgument, "--height") == 0) {
            if (i == argc - 1) {
                fprintf(stderr, "No argument specified for height\n");
                exit(EXIT_FAILURE);
            }
            else {
                currentArgument++;
                i++;
                appParam->windowHeight = strtoul(*currentArgument, NULL, 10);
                if (appParam->windowHeight <= 0) {
                    fprintf(stderr, "Wrong window height given\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
        else if (strcmp(*currentArgument, "--appdata") == 0) {
            if (i == argc - 1) {
                fprintf(stderr, "No argument specified for appdata\n");
                exit(EXIT_FAILURE);
            }
            else {
                currentArgument++;
                i++;
                appParam->appDataDirectory = *currentArgument;
            }
        }
        currentArgument++;
        i++;
    }
}

int main(int argc, char** argv) {
    HxfAppParam param = {
        GetModuleHandle(NULL), SW_NORMAL, 800, 600, "appdata"
    };

    handleParamaters(&param, argc, argv);

    hxfAppStart(&param);

    return 0;
}

#endif