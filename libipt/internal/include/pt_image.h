/*
 * Copyright (c) 2013-2016, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of Intel Corporation nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PT_IMAGE_H
#define PT_IMAGE_H

#include "pt_mapped_section.h"

#include "intel-pt.h"

#include <stdint.h>


/* A list of sections. */
struct pt_section_list {
	/* The next list element. */
	struct pt_section_list *next;

	/* The mapped section. */
	struct pt_mapped_section section;

	/* A flag saying whether @section is already mapped. */
	uint32_t mapped:1;
};

/* A traced image consisting of a collection of sections. */
struct pt_image {
	/* The optional image name. */
	char *name;

	/* The list of sections. */
	struct pt_section_list *sections;

	/* An optional read memory callback. */
	struct {
		/* The callback function. */
		read_memory_callback_t *callback;

		/* The callback context. */
		void *context;
	} readmem;

	/* The cache size as number of to-keep-mapped sections. */
	uint16_t cache;

	/* The number of permanently mapped sections. */
	uint16_t mapped;
};

/* Initialize an image with an optional @name. */
extern void pt_image_init(struct pt_image *image, const char *name);

/* Finalize an image.
 *
 * This removes all sections and frees the name.
 */
extern void pt_image_fini(struct pt_image *image);

/* Add a section to an image.
 *
 * Add @section to @image at @vaddr in @asid.  If @section overlaps with
 * existing sections, the existing sections are shrunk, split, or removed to
 * accomodate @section.
 *
 * Returns zero on success.
 * Returns -pte_internal if @image, @section, or @asid is NULL.
 */
extern int pt_image_add(struct pt_image *image, struct pt_section *section,
			const struct pt_asid *asid, uint64_t vaddr);

/* Remove a section from an image.
 *
 * Returns zero on success.
 * Returns -pte_internal if @image, @section, or @asid is NULL.
 * Returns -pte_bad_image if @image does not contain @section at @vaddr.
 */
extern int pt_image_remove(struct pt_image *image, struct pt_section *section,
			   const struct pt_asid *asid, uint64_t vaddr);

/* Read memory from an image.
 *
 * Reads at most @size bytes from @image at @addr in @asid into @buffer.
 *
 * Returns the number of bytes read on success, a negative error code otherwise.
 * Returns -pte_internal if @image, @buffer, or @asid is NULL.
 * Returns -pte_nomap if the section does not contain @addr.
 */
extern int pt_image_read(struct pt_image *image, uint8_t *buffer,
			 uint16_t size, const struct pt_asid *asid,
			 uint64_t addr);

#endif /* PT_IMAGE_H */
