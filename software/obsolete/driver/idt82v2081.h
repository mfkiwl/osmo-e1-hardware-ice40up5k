#ifndef _IDT82_H
#define _IDT82_H

enum idt82_term {
	IDT_TERM_INT_75	= 0
	IDT_TERM_INT_120,
	IDT_TERM_INT_100,
	IDT_TERM_INT_110,
	IDT_TERM_EXT,
};

enum idt82_mode {
	IDT_MODE_E1 = 0,
	IDT_MODE_T1,
	IDT_MODE_J1,
};

struct idt82 {
	enum idt82_mode mode;
	enum idt82_term term;
	void *priv;
	uint8_t cs;
};

int idt82_termination(struct idt82 *idt, enum idt82_term term);
int idt82_mode(struct idt82 *idt, enum idt82_mode mode);
int idt82_get_errcount(struct idt82 *idt);
int idt82_get_line_att(struct idt82 *idt);
int idt82_init(struct idt82 *idt);

/* callbacks into transport */
int idt82_reg_write(struct idt82 *idt, uint8_t reg, uin8_t val);
int idt82_reg_read(struct idt82 *idt, uint8_t reg);

/* board specific initializers */
int idt82_at91_init(struct idt82 *idt, void *spi, unsigned int id,
		    uint8_t cs, uint32_t spi_mr, uint32_t csr);

#endif
