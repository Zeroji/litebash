#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "interface.h"

#define EOL '\0'
#define SPACE ' '
#define STREAM_IN '<'
#define STREAM_OUT '>'
#define STREAM_ERR ']'
#define PIPE_OR '|'
#define ASYNC_AND '&'
#define SEPARATOR ';'
#define QUOTE '"'


typedef enum { ARGV, IN, OUT, ERR, BOTH } STATE;

// The art of doing twice as much as you should half as well as you could

void free_(int err __attribute__((unused)), void* ptr) { free(ptr); }
void fcls_(int err __attribute__((unused)), void* ptr) { fclose(ptr); }

int cd(char *dir);

int parse(char *line, FILE *std_in, FILE *std_out, FILE *std_err) {
    // parsing
    char c;
    int i=0;
    // argc, argv
    int fargc=0;
    char *fargv[1024];                                          // TO BE FREE
    // char buffer for words
    size_t bi = 0;
    char buffer[1024];
    // parser state + file mode
    STATE state = ARGV;
    char mode = 'w';
    // files
    FILE *in = std_in, *out = std_out, *err = std_err;

    do
    {
        c=line[i++];
        // word delimiters (e.g. `ls|head>output` works)
        char SPECIALS[] = {STREAM_IN, STREAM_OUT, STREAM_ERR, PIPE_OR, ASYNC_AND, SPACE, SEPARATOR, QUOTE, EOL};
        size_t iter;
        for(iter = 0; bi && iter < sizeof(SPECIALS); iter ++) // going to 7 for \0
            if(c==SPECIALS[iter]) {
                buffer[bi++] = EOL;
                if(state==ARGV) { // allocating size for new arg & copying
                    char *temp = malloc(bi * sizeof(char));
                    if(!temp) goto error;
                    on_exit(free_, temp);
                    strcpy(temp, buffer);
                    fargv[fargc++] = temp;
                } else {
                    // redirection can't happen before command name
                    if(!fargc) goto error;

                    // fopen mode must be char*
                    char mode_[2];
                    mode_[0] = mode;
                    mode_[1] = '\0';
                    // printf("Opening %s, mode %c for state %d\n", buffer, mode, state);

                    // existence & permission checks
                    if(mode == 'r' && access(buffer, F_OK)) {
                            printf("lb: %s: no such file or directory\n", buffer);
                            goto error;
                    } else if((mode == 'r' && access(buffer, R_OK)) ||\
                       (mode != 'r' && !access(buffer, F_OK) && access(buffer, W_OK))) {
                        printf("lb: %s: permission denied\n", buffer);
                        goto error;
                    }

                    FILE *temp =  fopen(buffer, mode_);

                    if(!temp) {
                        fprintf(stderr, "lb: %s: access error\n", buffer);
                        goto error;
                    }

                    on_exit(fcls_, temp);

                    switch(state) {
                        case IN:
                            in = temp; break;
                        case BOTH:
                            err = temp;
                        case OUT:
                            out = temp; break;
                        case ERR:
                            err = temp; break;
                        default: /* cannot happen */ break;
                    }
                }

                bi=0;
                state = ARGV;
                mode = 'w';
            }
        switch(c) {
            case EOL:
            case SPACE:
            // `|`, `&` and `;` are handled later
            case PIPE_OR:
            case ASYNC_AND:
            case SEPARATOR:
                break;
            case QUOTE:
                while(line[i] != '"' && line[i] != '\0') {
                    c = line[i++];
                    if(c == '\\' && line[i] == '"')
                        c = line[i++];
                    buffer[bi++] = c;
                }
                break;
            case STREAM_IN:
                switch(state) {
                    case ARGV:
                        mode = 'r';
                        state = IN;
                        break;
                    default: goto syn_error;
                }
                break;
            case STREAM_OUT:
                switch(state) {
                    case ARGV: state = OUT; break;
                    case OUT:
                        if(mode!='w') goto syn_error;
                        mode='a'; break;
                    case IN: goto syn_error;
                    case ERR: state = BOTH; break;
                    case BOTH:
                        if(mode!='w') goto syn_error;
                        mode='a'; break;
                    default:
                        goto error;
                }
                break;
            case STREAM_ERR:
                switch(state) {
                    case ARGV: state = ERR; break;
                    case ERR:
                        if(mode!='w') goto syn_error;
                        mode='a'; break;
                    case IN: goto syn_error;
                    case OUT: state = BOTH; break;
                    case BOTH:
                        if(mode!='w') goto syn_error;
                        mode='a'; break;
                    default:
                        goto syn_error;
                }
                break;
            case '\\':
                c=line[i++];
                if(c=='\0') break;
            default:
                buffer[bi++] = c;
        }
        if(c == PIPE_OR || c == ASYNC_AND || c == SEPARATOR || c == EOL) {
            if(!fargc) goto error;
            int state = 0; // separator
            char d = line[i];

            if(c == PIPE_OR) state = 1; // PIPE
            if(c == ASYNC_AND) state = 2; // ASYNC
            if(c == PIPE_OR && d == PIPE_OR) state = 3; // OR
            if(c == ASYNC_AND && d == ASYNC_AND) state = 4; // AND
            if(c == EOL) state = 5; // Nothing else
            if(state == 1) out = tmpfile();
            fargv[fargc] = NULL;

            // printf("Calling %s with async %d, state %d, args:", fargv[0], state==2, state);
            // int it; for(it=1;it<fargc;it++) printf(" %s", fargv[it]);
            // printf("\n");

            int res;
            if(!strcmp("exit", fargv[0])) {
                exit(0);
            } else if(!strcmp("cd", fargv[0])) {
                if(fargc > 1) res = cd(fargv[1]);
            } else {
                res = call(getfunc(fargv[0]), fargc, fargv, in, out, err, state==2);
            }

            if(state == 1) rewind(std_in = out);
            if(state == 2) printf("[-] %d\n", res);
            if(state == 3 && !res) return 1;
            if(state == 4 && res) return 1;
            if(state == 5) return res;
            return parse(line + i + state/3, std_in, std_out, std_err);
        }
    } while(1);

    syn_error:
    printf("lb: syntax error near unexpected token `%c'\n", c);
    error:
    return -1;
}

int cd(char *name) {
    if(chdir(name)) {
        if(errno == EACCES)
            fprintf(stderr, "lb: cd: %s: Permission denied\n", name);
        if(errno == ENOENT)
            fprintf(stderr, "lb: cd: %s: No such file or directory\n", name);
        return -1;
    }
    return 0;
}
