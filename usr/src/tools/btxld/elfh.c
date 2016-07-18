/*
 * Copyright (c) 1998 Robert Nordier
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#include <sys/types.h>
#include <sys/byteorder.h>

#include <stddef.h>
#include "elfh.h"

#define SET_ME	0xeeeeeeee    /* filled in by btxld */

/*
 * ELF header template.
 */
const struct elfh elfhdr = {
    {
	{
	    ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3,     /* e_ident */
	    ELFCLASS32, ELFDATA2LSB, EV_CURRENT, 0,
	    'F', 'r', 'e', 'e', 'B', 'S', 'D', 0
	},
	LE_16(ET_EXEC),			    /* e_type */
	LE_16(EM_386),			    /* e_machine */
	LE_32(EV_CURRENT),			    /* e_version */
	LE_32(SET_ME),			    /* e_entry */
	LE_32(offsetof(struct elfh, p)),	    /* e_phoff */
	LE_32(offsetof(struct elfh, sh)),	    /* e_shoff */
	0,					    /* e_flags */
	LE_16(sizeof(elfhdr.e)),		    /* e_ehsize */
	LE_16(sizeof(elfhdr.p[0])),		    /* e_phentsize */
	LE_16(sizeof(elfhdr.p) / sizeof(elfhdr.p[0])), /* e_phnum */
	LE_16(sizeof(elfhdr.sh[0])),		    /* e_shentsize */
	LE_16(sizeof(elfhdr.sh) / sizeof(elfhdr.sh[0])), /* e_shnum */
	LE_16(1)				    /* e_shstrndx */
    },
    {
	{
	    LE_32(PT_LOAD),			    /* p_type */
	    LE_32(sizeof(elfhdr)),		    /* p_offset */
	    LE_32(SET_ME),			    /* p_vaddr */
	    LE_32(SET_ME),			    /* p_paddr */
	    LE_32(SET_ME),			    /* p_filesz */
	    LE_32(SET_ME),			    /* p_memsz */
	    LE_32(PF_R | PF_X),		    /* p_flags */
	    LE_32(0x1000)			    /* p_align */
	},
	{
	    LE_32(PT_LOAD),			    /* p_type */
	    LE_32(SET_ME),			    /* p_offset */
	    LE_32(SET_ME),			    /* p_vaddr */
	    LE_32(SET_ME),			    /* p_paddr */
	    LE_32(SET_ME),			    /* p_filesz */
	    LE_32(SET_ME),			    /* p_memsz */
	    LE_32(PF_R | PF_W),		    /* p_flags */
	    LE_32(0x1000)			    /* p_align */
	}
    },
    {
	{
	    0, LE_32(SHT_NULL), 0, 0, 0, 0, LE_32(SHN_UNDEF), 0, 0, 0
	},
	{
	    LE_32(1),				    /* sh_name */
	    LE_32(SHT_STRTAB), 		    /* sh_type */
	    0,					    /* sh_flags */
	    0,					    /* sh_addr */
	    LE_32(offsetof(struct elfh, shstrtab)), /* sh_offset */
	    LE_32(sizeof(elfhdr.shstrtab)),	    /* sh_size */
	    LE_32(SHN_UNDEF),			    /* sh_link */
	    0,					    /* sh_info */
	    LE_32(1),				    /* sh_addralign */
	    0					    /* sh_entsize */
	},
	{
	    LE_32(0xb),			    /* sh_name */
	    LE_32(SHT_PROGBITS),		    /* sh_type */
	    LE_32(SHF_EXECINSTR | SHF_ALLOC),	    /* sh_flags */
	    LE_32(SET_ME),			    /* sh_addr */
	    LE_32(SET_ME),			    /* sh_offset */
	    LE_32(SET_ME),			    /* sh_size */
	    LE_32(SHN_UNDEF),			    /* sh_link */
	    0,					    /* sh_info */
	    LE_32(4),				    /* sh_addralign */
	    0					    /* sh_entsize */
	},
	{
	    LE_32(0x11),			    /* sh_name */
	    LE_32(SHT_PROGBITS),		    /* sh_type */
	    LE_32(SHF_ALLOC | SHF_WRITE),	    /* sh_flags */
	    LE_32(SET_ME),			    /* sh_addr */
	    LE_32(SET_ME),			    /* sh_offset */
	    LE_32(SET_ME),			    /* sh_size */
	    LE_32(SHN_UNDEF),			    /* sh_link */
	    0,					    /* sh_info */
	    LE_32(4),				    /* sh_addralign */
	    0					    /* sh_entsize */
	}
    },
    "\0.shstrtab\0.text\0.data" 		    /* shstrtab */
};
