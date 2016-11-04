/*
 * @(#)sa_funcs.h	1.1
 * Special functions for 80C296-SA and compatible processors.
 */
#ifndef _sa_funcs_h
#define _sa_funcs_h

void            enable(void);
void            disable(void);

void            enable_pts(void);                           /* dummy function */
void            disable_pts(void);                          /* dummy function */

void            power_down(void);
void            idle(void);

#endif  /* _sa_funcs_h */
