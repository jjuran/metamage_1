/*	=========
 *	mach_o.cc
 *	=========
 */

#include "recall/mach_o.hh"

#include <string.h>

#ifdef __MACH__
	#include <mach-o/dyld.h>
	#include <mach-o/loader.h>
	#include <mach-o/nlist.h>
	#include <mach-o/stab.h>
#endif


namespace recall
{
	
#ifdef __MACH__
	
	// The following code was taken from MoreAddrToSym.c in Apple's MoreIsBetter sample code.
	
	// FindOwnerOfPC and GetFunctionName countesy of Ed Wynne.
	
	static const struct mach_header *FindOwnerOfPC(unsigned int pc)
	{
		unsigned int			count,index,offset,cmdex;
		struct segment_command	*seg;
		struct load_command		*cmd;
		const struct mach_header		*header;
		
		count = _dyld_image_count();
		for (index = 0;index < count;index += 1)
		{
			header = _dyld_get_image_header(index);
			offset = _dyld_get_image_vmaddr_slide(index);
			cmd = (struct load_command*)((char*)header + sizeof(struct mach_header));
			for (cmdex = 0;cmdex < header->ncmds;cmdex += 1,cmd = (struct load_command*)((char*)cmd + cmd->cmdsize))
			{
				switch(cmd->cmd)
				{
					case LC_SEGMENT:
						seg = (struct segment_command*)cmd;
						if ((pc >= (seg->vmaddr + offset)) && (pc < (seg->vmaddr + offset + seg->vmsize)))
							return header;
						break;
				}
			}
		}
		
		return NULL;
	}

	static const char *GetFunctionName(unsigned int pc,unsigned int *offset, bool *publicSymbol)
	{
		struct segment_command	*seg_linkedit = NULL;
		struct segment_command	*seg_text = NULL;
		struct symtab_command	*symtab = NULL;
		struct load_command		*cmd;
		const struct mach_header*header;
		unsigned int			vm_slide,file_slide;
		struct nlist			*sym,*symbase;
		char					*strings,*name;
		unsigned int			base,index;
		
		header = FindOwnerOfPC(pc);
		if (header != NULL)
		{
			cmd = (struct load_command*)((char*)header + sizeof(struct mach_header));
			for (index = 0;index < header->ncmds;index += 1,cmd = (struct load_command*)((char*)cmd + cmd->cmdsize))
			{
				switch(cmd->cmd)
				{
					case LC_SEGMENT:
						if (!strcmp(((struct segment_command*)cmd)->segname,SEG_TEXT))
							seg_text = (struct segment_command*)cmd;
						else if (!strcmp(((struct segment_command*)cmd)->segname,SEG_LINKEDIT))
							seg_linkedit = (struct segment_command*)cmd;
						break;
					
					case LC_SYMTAB:
						symtab = (struct symtab_command*)cmd;
						break;
				}
			}
			
			if ((seg_text == NULL) || (seg_linkedit == NULL) || (symtab == NULL))
			{
				*offset = 0;
				return NULL;
			}
			
			vm_slide = (unsigned long)header - (unsigned long)seg_text->vmaddr;
			file_slide = ((unsigned long)seg_linkedit->vmaddr - (unsigned long)seg_text->vmaddr) - seg_linkedit->fileoff;
			symbase = (struct nlist*)((unsigned long)header + (symtab->symoff + file_slide));
			strings = (char*)((unsigned long)header + (symtab->stroff + file_slide));
			
			// Look for a global symbol.
			for (index = 0,sym = symbase;index < symtab->nsyms;index += 1,sym += 1)
			{
				if (sym->n_type != N_FUN)
					continue;
				
				name = sym->n_un.n_strx ? (strings + sym->n_un.n_strx) : NULL;
				base = sym->n_value + vm_slide;
				
				for (index += 1,sym += 1;index < symtab->nsyms;index += 1,sym += 1)
					if (sym->n_type == N_FUN)
						break;
				
				if ((pc >= base) && (pc <= (base + sym->n_value)) && (name != NULL) && (strlen(name) > 0))
				{
					*offset = pc - base;
					*publicSymbol = true;
					return name;
				}
			}
			
			// Look for a reasonably close private symbol.
			for (name = NULL,base = 0xFFFFFFFF,index = 0,sym = symbase;index < symtab->nsyms;index += 1,sym += 1)
			{
				if ((sym->n_type & 0x0E) != 0x0E)
					continue;
				
				if ((sym->n_value + vm_slide) > pc)
					continue;
				
				if ((base != 0xFFFFFFFF) && ((pc - (sym->n_value + vm_slide)) >= (pc - base)))
					continue;
				
				name = sym->n_un.n_strx ? (strings + sym->n_un.n_strx) : NULL;
				base = sym->n_value + vm_slide;
			}
			
			*offset = pc - base;
			*publicSymbol = false;
			return name;
		}
		
		*offset = 0;
		return NULL;
	}
	
	const char* find_symbol_name( return_address_mach_o addr )
	{
		unsigned int  offset       = 0;
		bool          publicSymbol = false;
		
		const char* name = GetFunctionName( (unsigned) addr, &offset, &publicSymbol );
		
		return name;
	}
	
#endif
	
}

