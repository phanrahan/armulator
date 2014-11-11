
extern unsigned int GET32 ( unsigned int  );
extern void PUT32 ( unsigned int, unsigned int );

#define UART_BASE   0x16000000


void uart_putc( unsigned int c)
{
    PUT32(UART_BASE,c);
}

void uart_init(void)
{
}


unsigned int GETPC ( void );
unsigned int GETSP ( void );
void hexstring ( unsigned int d )
{
    //unsigned int ra;
    unsigned int rb;
    unsigned int rc;

    rb=32;
    while(1)
    {
        rb-=4;
        rc=(d>>rb)&0xF;
        if(rc>9) rc+=0x37; else rc+=0x30;
        uart_putc(rc);
        if(rb==0) break;
    }
    uart_putc(0x0D);
    uart_putc(0x0A);
}


int notmain ( void )
{
    unsigned int ra;

    uart_init();
    hexstring(GETPC());
    hexstring(GETSP());
    for(ra=0;ra<10;ra++)
    {
        uart_putc('A'+ra);
    }
    uart_putc(0x0D);
    uart_putc(0x0A);
    uart_putc(0x55);
    uart_putc(0x30);

    PUT32(0xF0000000,1); //HALT

    return(0);
}













