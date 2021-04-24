#include <syscall.h>
#include <string.h>
#include <stdarg.h>
#include "printf.h"
int
printf (const char *format, ...)
{
  char buf[STRBUF_SIZE];
  va_list args;
  double val_double;
  unsigned long val_ulong;
  unsigned long val_hex;
  unsigned long dividend, divisor;
  va_start (args, format);
  while (*format != '\0')
    {
      if (*format == '%')
	{
	  ++format;
	  switch (*format)
	    {
	    case 'd':
	      val_ulong = va_arg (args, unsigned long);
	      ltoa (val_ulong, buf);
	      putstr (buf);
	      break;
	    case 'q':
	      // kernel space float point
	      dividend = va_arg (args, unsigned long);
	      divisor = va_arg (args, unsigned long);
	      fdivtoa (dividend, divisor, buf);
	      putstr (buf);
	      break;
	    case 'f':
	      // user space float point
	      val_double = va_arg (args, double);
	      ftoa (val_double, buf);
	      putstr (buf);
	      break;
	    case 's':
	      putstr (va_arg (args, char *));
	      break;
	    case '%':
	      uart_write ("%", 1);
	      break;
	    case 'x':
	      val_hex = va_arg (args, unsigned long);
	      puthex  (val_hex);
	      break;
	    case 'p':
	      val_hex = va_arg (args, unsigned long);
	      puthex  (val_hex >> 32);
	      puthex  (val_hex);
	      break;
	    default:
	      buf[0] = '\0';
	      break;
	    }
	}
      else
	{
    if(*format == '\n')
      uart_write("\r", 1);
	  uart_write ((char *)format, 1);
	}
      ++format;
    }
  va_end (args);
  return 0;
}

void
ltoa (unsigned long val, char *buf)
{
  unsigned long scan = 10000000000000000000u;
  int met_num = 0;
  while (scan != 0)
    {
      *buf = (val / scan) % 10;
      if (*buf != 0)
	met_num = 1;
      if (met_num)
	*buf++ += '0';
      scan /= 10;
    }
  if (!met_num)
    *buf++ = '0';
  *buf = '\0';
}

void
ftoa (double val, char *buf)
{
  double scan = 1000000;
  int cnt = 0;
  int met_num = 0;

  while (cnt < 6)
    {
      *buf = (long) (val / scan) % 10;
      if (*buf != 0)
	met_num = 1;
      *buf += '0';
      if (met_num)
	buf++;
      if (scan < 1)
	cnt++;
      if (scan == 1)
	{
	  if (!met_num)
	    {
	      *buf++ = '0';
	      met_num = 1;
	    }
	  *buf++ = '.';
	}
      scan /= 10;
    }
  *buf = 0;
}

void
fdivtoa (unsigned long dividend, unsigned long divisor, char *buf)
{
  unsigned long quot = dividend / divisor;
  unsigned long remainder = dividend % divisor;
  int cnt = 0;

  ltoa (quot, buf);
  while (*buf != '\0')
    ++buf;
  *buf++ = '.';
  // postfix 6 points
  remainder *= 1000000;
  buf += 6;
  *buf-- = '\0';
  quot = remainder / divisor;
  for (cnt = 0; cnt < 6; cnt++)
    {
      *buf-- = quot % 10 + '0';
      quot /= 10;
    }
}

void
putstr (char *s)
{
  while (*s != '\0')
    uart_write (s++, 1);
}

void
puthex (unsigned int d)
{
  char buf[1];
  unsigned int n;
  int c;
  for (c = 28; c >= 0; c -= 4)
    {
      // get highest tetrad
      n = (d >> c) & 0xF;
      // 0-9 => '0'-'9', 10-15 => 'A'-'F'
      n += n > 9 ? 0x37 : 0x30;
      buf[0] = n;
      uart_write (buf, 1);
    }
}
