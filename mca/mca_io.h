#ifndef MCA_IO_H
#define MCA_IO_H

int read_args(char *filename);
int read_args_find_EP(char *filename);
int write_array(char *filename, double **a, int x, int y);
int write_bool_array(char *filename, bool **a, int x, int y);

#endif
