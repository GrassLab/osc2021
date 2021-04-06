#ifndef _DTP_H_
#define _DTP_H_

struct dtn {
    char *name;
    char *compatible;
};
int do_dtp(int (*_probe_func)(struct dtn *node));

#endif /*_DTP_H_ */