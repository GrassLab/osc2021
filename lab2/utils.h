extern void delay ( unsigned long);
extern void put32 ( unsigned long, unsigned int );
extern unsigned int get32 ( unsigned long );
extern void branch_to_address(char *dt, char *addr);
extern int lead_zero(int num);
extern unsigned int get_el(void);
extern unsigned int get_sctlr_el1(void);
extern unsigned long get_hcr_el2(void);
extern unsigned int get_scr_el3(void);
extern unsigned int get_spsr_el2(void);
extern unsigned int get_cntfrq_el0(void);
extern unsigned long run_user_program(char *start, char *sp);

