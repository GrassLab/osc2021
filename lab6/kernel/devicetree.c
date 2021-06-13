#include "devicetree.h"
#include "uart.h"
#include "util.h"

unsigned long int align4(unsigned long int size) 
{
	unsigned long int residual = size % 4;
	
	if(residual > 0)
		return size + (4 - residual);
	else
		return size;
}

unsigned int convert_bigendian(void *ptr) 
{
	// ex. 1234 -> 4321
	
    unsigned char *bytes = (unsigned char *)ptr;
	
    unsigned int value = bytes[3];	
    value |= bytes[2] << 8;
    value |= bytes[1] << 16;
    value |= bytes[0] << 24;

    return value;
}

void dtb_ls() 
{
	unsigned long int dtb = *((unsigned long int*)DTB_ADDR);
	struct fdt_header *header = (struct fdt_header *)dtb;

	if (convert_bigendian(&header->magic) != (unsigned long int)DTB_MAGIC)
	{
		uart_putstr("Format error !");
		return;
	}

	return parse_node(header, dtb);
}

void print_indent(int level)
{
	while(level > 0)
	{
		uart_putstr("-");
		level--;
	}
}

void parse_node(struct fdt_header *header, unsigned long int dtb) 
{
	unsigned long int ptr = dtb + convert_bigendian(&header->off_dt_struct);
	unsigned long int endptr = ptr + convert_bigendian(&header->totalsize);
	int level = 0;

	while (ptr < endptr) 
	{
		unsigned int tag = convert_bigendian((char *)ptr);
		ptr += sizeof(unsigned int);

		switch (tag) 
		{
			case FDT_BEGIN_NODE: // Start node
			
				// struct fdt_node_header 
				// {
				// 		fdt32_t tag; => 4 byte
				//		char name[0]; => length + '\0'
				// };
				print_indent(level);
				uart_putstr((char *)ptr);
				uart_putstr("\n");
				level++;
				
				ptr += align4(strlen((char *)ptr) + 1); // length + '\0'   
				break;

			case FDT_END_NODE: // End node
				level--;
				break;

			case FDT_NOP: // Nop
				break;

			case FDT_PROP: // Property
			{
				// struct fdt_property 
				// {
				//		unsigned int tag; => 4 byte
				//		unsigned int len; => 4 byte
				//		unsigned int nameoff; => 4 byte
				//		char data[0]; => len
				// };
				
				unsigned int len = convert_bigendian((char *)ptr); // data length
				ptr += sizeof(unsigned int); // nameoff
				ptr += sizeof(unsigned int); // data
				ptr += align4(len);	// next node
				break;
			}				

			case FDT_END: // Device tree end
				break;
		}
	}
}

int dtb_cat(char* name, dtb_callback cb)
{
	unsigned long int dtb = *((unsigned long int*)DTB_ADDR);
	struct fdt_header *header = (struct fdt_header *)dtb;

	if (convert_bigendian(&header->magic) != (unsigned long int)DTB_MAGIC)
	{
		uart_putstr("Format error !");
		return -1;
	}

	return parse_and_callback(header, dtb, name, cb);
}

int parse_and_callback(struct fdt_header *header, unsigned long int dtb, char* name, dtb_callback cb) 
{
	unsigned long int ptr = dtb + convert_bigendian(&header->off_dt_struct);
	unsigned long int endptr = ptr + convert_bigendian(&header->totalsize);
	unsigned long int strptr = dtb + convert_bigendian(&header->off_dt_strings);

	while (ptr < endptr) 
	{
		unsigned int tag = convert_bigendian((char *)ptr);
		ptr += sizeof(unsigned int);

		switch (tag) 
		{
			case FDT_BEGIN_NODE: // Start node
			
				// struct fdt_node_header 
				// {
				// 		fdt32_t tag; => 4 byte
				//		char name[0]; => length + '\0'
				// };			
				if (strcmp(name, (char *)ptr))
				{
					// if node name is search name , excute callback
					cb(ptr, strptr);
					return 0;
				}
				
				ptr += align4(strlen((char *)ptr) + 1); // length + '\0'
				break;

			case FDT_END_NODE: // End node
				break;

			case FDT_NOP: // Nop
				break;

			case FDT_PROP: // Property
			{
				// struct fdt_property 
				// {
				//		unsigned int tag; => 4 byte
				//		unsigned int len; => 4 byte
				//		unsigned int nameoff; => 4 byte
				//		char data[0]; => len
				// };
				
				unsigned int len = convert_bigendian((char *)ptr); // data length
				ptr += sizeof(unsigned int); // nameoff
				ptr += sizeof(unsigned int); // data
				ptr += align4(len); // next node
				break;
			}				

			case FDT_END: // Device tree end
				break;
		}
	}
	
	return -1;
}