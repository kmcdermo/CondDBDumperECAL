// merge different dump files with overlapping IOV's
// the only condition for the IOV's in and accross the files is
//
//    if IOV_i+n < IOV_i then IOV_i+n has already been found
//
// If no CMSSW is available, compile with `g++ merge_dump.c'
//
// federico.ferri@cern.ch    2011.08.31

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <set>

std::set<time_t> s;

void usage(char * c)
{
        fprintf(stderr, "Usage: %s <file_1> [file_2] ...\n", c);
        fprintf(stderr, "where if file_1 is `-' %s will read from `stdin'\n", c);
        fprintf(stderr, "Merges different dump files with overlapping IOV's.\n");
        fprintf(stderr, "Condition for the IOV's in and accross the files:\n");
        fprintf(stderr, "  if IOV_i+n < IOV_i then IOV_i+n has already been found\n");
        fprintf(stderr, "The output of the merging process is `stdout'.\n");
}

int already_found(time_t t)
{
        return !s.insert(t).second;
}

void process_file(FILE * fin, FILE * fout)
{
        char * line = NULL;
        size_t len = 0;
        ssize_t read;
        int skip = 0, cnt = 0;
        time_t t1, t3, tp = 0;
        while( (read = getline(&line, &len, fin)) != EOF ) {
                if (line[0] == 'T') {
                        sscanf(line, "T %ld %ld", &t1, &t3);
                        assert(t1 < t3);
                        assert(tp <= t1);
                        assert(cnt == 0 || cnt == 75848);
                        skip = 0;
                        cnt = 0;
                        if (already_found(t1)) {
                                skip = 1;
                                continue;
                        }
                        tp = t3;
                        fprintf(fout, "%s", line);
                } else if (!skip && line[0] == 'P') {
                        fprintf(stdout, "%s", line);
                        ++cnt;
                } else if (!skip) {
                        fprintf(stderr, "First char line error, aborting\n");
                        exit(3);
                }
        }
        if (line) free(line);
}

int main(int argc, char ** argv)
{
        FILE * fd;
        int i;
        if (argc < 2) {
                usage(argv[0]);
                exit(1);
        }
        if (strcmp(argv[1], "-") == 0 && argc != 2) {
                usage(argv[0]);
                exit(2);
        } else if (strcmp(argv[1], "-") == 0) {
                fd = stdin;
                process_file(stdin, stdout);
                fclose(stdout);
                exit(0);
        }
        for (i = 1; i < argc; ++i) {
                fprintf(stderr, "processing file `%s'...", argv[i]);
                fd = fopen(argv[i], "r");
                if (fd != NULL) {
                        process_file(fd, stdout);
                        fclose(fd);
                        fprintf(stderr, " done.\n");
                } else {
                        fprintf(stderr, " could not open file, skipped.\n");
                }
        }
        fclose(stdout);
        return 0;
}
