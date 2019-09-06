/* Winbond: This enables the Pin 4 of Winbond as an output pin and
   selects full speed for both fans. */

#include <common.h>
#include <asm/processor.h>

struct ppc_i2c_regs {
	volatile char	iic0_mdbuf;       /* Master data buffer */
	volatile char	iic0_reserved1;   /* Reserved */
	volatile char	iic0_sdbuf;       /* Slave data buffer */
	volatile char	iic0_reserved2;   /* Reserved */
	volatile char	iic0_lmadr;       /* Low master address */
	volatile char	iic0_hmadr;       /* High master address */
	volatile char	iic0_cntl;        /* Control */
	volatile char	iic0_mdcntl;      /* Mode control */
	volatile char	iic0_sts;         /* Status */
	volatile char	iic0_exsts;       /* Extended status */
	volatile char	iic0_lsadr;       /* Low slave address */
	volatile char	iic0_hsadr;       /* High slave address */
	volatile char	iic0_clkdiv;      /* Clock divide */
	volatile char	iic0_intrmsk;     /* Interrupt mask */
	volatile char	iic0_xfrcnt;      /* Transfer count */
	volatile char	iic0_xtcntlss;    /* Extended control / slave status */
	volatile char	iic0_directcntl;  /* Direct control */
};

#define TIMEOUT 2000000

/* IIC addresses for various devices */

#define WINBOND 0x2d


static void i2c_init (struct ppc_i2c_regs *iic)
{
	int i;
	iic->iic0_xtcntlss = 0x01;  /* Soft reset */
	for (i=0;i<1000;i++) ;
	iic->iic0_xtcntlss = 0x00;
/* be on the safe side .. */
	iic->iic0_clkdiv = 0xff;  
//	iic->iic0_clkdiv = 0x02;    /* for OBP of 25MHz */
	iic->iic0_mdcntl = 0xc2; /* Clear FIFO, exit unknown bus state, 
				    ignore all slave requests */
	iic->iic0_lmadr =
	iic->iic0_hmadr =
	iic->iic0_cntl =
	iic->iic0_lsadr =
	iic->iic0_hsadr =
	iic->iic0_intrmsk =
	iic->iic0_xfrcnt =
	iic->iic0_xtcntlss = 0;
}

static int wait_on_bus (struct ppc_i2c_regs *iic)
{
	int timeout;

	timeout = 0;
	while (((iic->iic0_exsts & 0x70)!=0x40) && timeout<TIMEOUT) timeout++;
	printf ("wait on bus Timeout: %d\n", timeout);

	return 0;
}

static int wait_for_completion (struct ppc_i2c_regs *iic, const char *rw, int addr)
{
	int timeout;

	timeout = 0;
	while ((iic->iic0_sts & 1) && timeout<TIMEOUT) timeout++;
	if (iic->iic0_sts & 1) {
		printf ("IIC: Timeout %s address %02x, resetting I2C interface.\n", rw, addr);
		i2c_init (iic);

		return -1;
	} 
	if (iic->iic0_sts & 4) {
		printf ("IIC: Error accessing bus, status: %2.2x extended status: %2.2x count: %2.2x\n", iic->iic0_sts, iic->iic0_exsts, iic->iic0_xfrcnt);
		printf ("IIC: on %s address %02x, resetting I2C interface.\n", rw, addr);
		i2c_init (iic);

		return -1;
	}
	return 0;
}

int tc_i2c_write (struct ppc_i2c_regs *iic, int address, char *buf, size_t count)
{
	int i, ret;

	for (i=0; i<count; i++) { 
		iic->iic0_mdbuf=buf[i];
	}
	iic->iic0_lmadr = address<<1;
	iic->iic0_cntl = 1 | ((count-1)<<4);  /* Start write request. */

	if ((ret=wait_for_completion(iic, "writing to", address))!=0) 
		return ret;

	return 0;
}

int i2c_write_byte (struct ppc_i2c_regs *iic, int address, char b)
{
	return tc_i2c_write (iic, address, &b, 1);
}
	
int tc_i2c_read (struct ppc_i2c_regs *iic, int address, char *buf, size_t count)
{
	int i, ret;

	if ((count > 4) || (count < 1)) return -1;
	i=0;
	iic->iic0_lmadr = address<<1;
	iic->iic0_cntl = 3 | ((count-1)<<4);  /* Start read request. */

	if ((ret=wait_for_completion(iic, "reading from", address))!=0) 
		return ret;

	for (i=0; i<count; i++) { 
		buf[i]=iic->iic0_mdbuf;
	}
	return 0;
}
	
int i2c_read_byte (struct ppc_i2c_regs *iic, int address)
{
	char b; 
	
	tc_i2c_read (iic, address, &b, 1);
	return b;
}


int write_winbond_reg (struct ppc_i2c_regs *iic, int reg, int val)
{
	char buf[4];
	buf[0] = reg;
	buf[1] = val;
	return tc_i2c_write (iic, WINBOND, buf, 2);
}
	
int read_winbond_reg (struct ppc_i2c_regs *iic, int reg, int *val)
{
	i2c_write_byte (iic, WINBOND, reg);
	return i2c_read_byte (iic, WINBOND);
}
	
	
int enable_fans (void) 
{

//ledon();
	struct ppc_i2c_regs *iic=(struct ppc_i2c_regs*) 0xef600500;



	i2c_init (iic);

	printf ("Enabling fans (FULL SPEED)..");
	write_winbond_reg (iic, 0x4d, 0x25);  /* Make Pin 4 an output pin */
	write_winbond_reg (iic, 0x5c, 0x19);  /* Enable PWM for pin 4 */ 
	write_winbond_reg (iic, 0x5a, 0xff);  /* Select full speed */
	printf (" Done.\n");
	return 0;
}

