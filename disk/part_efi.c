/*
 * Copyright (C) 2008 RuggedCom, Inc.
 * Richard Retanubun <RichardRetanubun@RuggedCom.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Problems with CONFIG_SYS_64BIT_LBA:
 *
 * struct disk_partition.start in include/part.h is sized as ulong.
 * When CONFIG_SYS_64BIT_LBA is activated, lbaint_t changes from ulong to uint64_t.
 * For now, it is cast back to ulong at assignment.
 *
 * This limits the maximum size of addressable storage to < 2 Terra Bytes
 */
#include <asm/unaligned.h>
#include <common.h>
#include <command.h>
#include <ide.h>
#include <malloc.h>
#include <part_efi.h>
#include <linux/ctype.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef HAVE_BLOCK_DEVICE
/**
 * efi_crc32() - EFI version of crc32 function
 * @buf: buffer to calculate crc32 of
 * @len - length of buf
 *
 * Description: Returns EFI-style CRC32 value for @buf
 */
static inline u32 efi_crc32(const void *buf, u32 len)
{
	return crc32(0, buf, len);
}

/*
 * Private function prototypes
 */

static int pmbr_part_valid(struct partition *part);
static int is_pmbr_valid(legacy_mbr * mbr);
static int is_gpt_valid(block_dev_desc_t * dev_desc, unsigned long long lba,
				gpt_header * pgpt_head, gpt_entry ** pgpt_pte);
static gpt_entry *alloc_read_gpt_entries(block_dev_desc_t * dev_desc,
				gpt_header * pgpt_head);
static int is_pte_valid(gpt_entry * pte);

static char *print_efiname(gpt_entry *pte)
{
	static char name[PARTNAME_SZ + 1];
	int i;
	for (i = 0; i < PARTNAME_SZ; i++) {
		u8 c;
		c = pte->partition_name[i] & 0xff;
		c = (c && !isprint(c)) ? '.' : c;
		name[i] = c;
	}
	name[PARTNAME_SZ] = 0;
	return name;
}

static void uuid_string(unsigned char *uuid, char *str)
{
	static const u8 le[16] = {3, 2, 1, 0, 5, 4, 7, 6, 8, 9, 10, 11,
				  12, 13, 14, 15};
	int i;

	for (i = 0; i < 16; i++) {
		sprintf(str, "%02x", uuid[le[i]]);
		str += 2;
		switch (i) {
		case 3:
		case 5:
		case 7:
		case 9:
			*str++ = '-';
			break;
		}
	}
}

static efi_guid_t system_guid = PARTITION_SYSTEM_GUID;

static inline int is_bootable(gpt_entry *p)
{
	return p->attributes.fields.legacy_bios_bootable ||
		!memcmp(&(p->partition_type_guid), &system_guid,
			sizeof(efi_guid_t));
}

#ifdef CONFIG_EFI_PARTITION
/*
 * Public Functions (include/part.h)
 */

void print_part_efi(block_dev_desc_t * dev_desc)
{
	ALLOC_CACHE_ALIGN_BUFFER(gpt_header, gpt_head, 1);
	gpt_entry *gpt_pte = NULL;
	int i = 0;
	char uuid[37];

	if (!dev_desc) {
		printf("%s: Invalid Argument(s)\n", __func__);
		return;
	}
	/* This function validates AND fills in the GPT header and PTE */
	if (is_gpt_valid(dev_desc, GPT_PRIMARY_PARTITION_TABLE_LBA,
			 gpt_head, &gpt_pte) != 1) {
		printf("%s: *** ERROR: Invalid GPT ***\n", __func__);
		return;
	}

	debug("%s: gpt-entry at %p\n", __func__, gpt_pte);

	printf("Part\tStart LBA\tEnd LBA\t\tName\n");
	printf("\tAttributes\n");
	printf("\tType UUID\n");
	printf("\tPartition UUID\n");

	for (i = 0; i < le32_to_cpu(gpt_head->num_partition_entries); i++) {
		/* Stop at the first non valid PTE */
		if (!is_pte_valid(&gpt_pte[i]))
			break;

		printf("%3d\t0x%08llx\t0x%08llx\t\"%s\"\n", (i + 1),
			le64_to_cpu(gpt_pte[i].starting_lba),
			le64_to_cpu(gpt_pte[i].ending_lba),
			print_efiname(&gpt_pte[i]));
		printf("\tattrs:\t0x%016llx\n", gpt_pte[i].attributes.raw);
		uuid_string(gpt_pte[i].partition_type_guid.b, uuid);
		printf("\ttype:\t%s\n", uuid);
		uuid_string(gpt_pte[i].unique_partition_guid.b, uuid);
		printf("\tuuid:\t%s\n", uuid);
	}

	/* Remember to free pte */
	free(gpt_pte);
	return;
}

int get_partition_info_efi(block_dev_desc_t * dev_desc, int part,
				disk_partition_t * info)
{
	ALLOC_CACHE_ALIGN_BUFFER(gpt_header, gpt_head, 1);
	gpt_entry *gpt_pte = NULL;

	/* "part" argument must be at least 1 */
	if (!dev_desc || !info || part < 1) {
		printf("%s: Invalid Argument(s)\n", __func__);
		return -1;
	}

	/* This function validates AND fills in the GPT header and PTE */
	if (is_gpt_valid(dev_desc, GPT_PRIMARY_PARTITION_TABLE_LBA,
			gpt_head, &gpt_pte) != 1) {
		printf("%s: *** ERROR: Invalid GPT ***\n", __func__);
		return -1;
	}

	if (part > le32_to_cpu(gpt_head->num_partition_entries) ||
	    !is_pte_valid(&gpt_pte[part - 1])) {
		printf("%s: *** ERROR: Invalid partition number %d ***\n",
			__func__, part);
		return -1;
	}

	/* The ulong casting limits the maximum disk size to 2 TB */
	info->start = (u64)le64_to_cpu(gpt_pte[part - 1].starting_lba);
	/* The ending LBA is inclusive, to calculate size, add 1 to it */
	info->size = ((u64)le64_to_cpu(gpt_pte[part - 1].ending_lba) + 1)
		     - info->start;
	info->blksz = GPT_BLOCK_SIZE;

	sprintf((char *)info->name, "%s",
			print_efiname(&gpt_pte[part - 1]));
	sprintf((char *)info->type, "U-Boot");
	info->bootable = is_bootable(&gpt_pte[part - 1]);
#ifdef CONFIG_PARTITION_UUIDS
	uuid_string(gpt_pte[part - 1].unique_partition_guid.b, info->uuid);
#endif

	debug("%s: start 0x%lX, size 0x%lX, name %s", __func__,
		info->start, info->size, info->name);

	/* Remember to free pte */
	free(gpt_pte);
	return 0;
}

int test_part_efi(block_dev_desc_t * dev_desc)
{
	ALLOC_CACHE_ALIGN_BUFFER(legacy_mbr, legacymbr, 1);

	/* Read legacy MBR from block 0 and validate it */
	if ((dev_desc->block_read(dev_desc->dev, 0, 1, (ulong *)legacymbr) != 1)
		|| (is_pmbr_valid(legacymbr) != 1)) {
		return -1;
	}
	return 0;
}

/**
 * set_protective_mbr(): Set the EFI protective MBR
 * @param dev_desc - block device descriptor
 *
 * @return - zero on success, otherwise error
 */
static int set_protective_mbr(block_dev_desc_t *dev_desc)
{
	legacy_mbr *p_mbr;

	/* Setup the Protective MBR */
	p_mbr = calloc(1, sizeof(p_mbr));
	if (p_mbr == NULL) {
		printf("%s: calloc failed!\n", __func__);
		return -1;
	}
	/* Append signature */
	p_mbr->signature = MSDOS_MBR_SIGNATURE;
	p_mbr->partition_record[0].sys_ind = EFI_PMBR_OSTYPE_EFI_GPT;
	p_mbr->partition_record[0].start_sect = 1;
	p_mbr->partition_record[0].nr_sects = (u32) dev_desc->lba;

	/* Write MBR sector to the MMC device */
	if (dev_desc->block_write(dev_desc->dev, 0, 1, p_mbr) != 1) {
		printf("** Can't write to device %d **\n",
			dev_desc->dev);
		free(p_mbr);
		return -1;
	}

	free(p_mbr);
	return 0;
}

/**
 * string_uuid(); Convert UUID stored as string to bytes
 *
 * @param uuid - UUID represented as string
 * @param dst - GUID buffer
 *
 * @return return 0 on successful conversion
 */
static int string_uuid(char *uuid, u8 *dst)
{
	efi_guid_t guid;
	u16 b, c, d;
	u64 e;
	u32 a;
	u8 *p;
	u8 i;

	const u8 uuid_str_len = 36;

	/* The UUID is written in text: */
	/* 1        9    14   19   24 */
	/* xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx */

	debug("%s: uuid: %s\n", __func__, uuid);

	if (strlen(uuid) != uuid_str_len)
		return -1;

	for (i = 0; i < uuid_str_len; i++) {
		if ((i == 8) || (i == 13) || (i == 18) || (i == 23)) {
			if (uuid[i] != '-')
				return -1;
		} else {
			if (!isxdigit(uuid[i]))
				return -1;
		}
	}

	a = (u32)simple_strtoul(uuid, NULL, 16);
	b = (u16)simple_strtoul(uuid + 9, NULL, 16);
	c = (u16)simple_strtoul(uuid + 14, NULL, 16);
	d = (u16)simple_strtoul(uuid + 19, NULL, 16);
	e = (u64)simple_strtoull(uuid + 24, NULL, 16);

	p = (u8 *) &e;
	guid = EFI_GUID(a, b, c, d >> 8, d & 0xFF,
			*(p + 5), *(p + 4), *(p + 3),
			*(p + 2), *(p + 1) , *p);

	memcpy(dst, guid.b, sizeof(efi_guid_t));

	return 0;
}

int write_gpt_table(block_dev_desc_t *dev_desc,
		gpt_header *gpt_h, gpt_entry *gpt_e)
{
	const int pte_blk_num = (gpt_h->num_partition_entries
		* sizeof(gpt_entry)) / dev_desc->blksz;

	u32 calc_crc32;
	u64 val;

	debug("max lba: %x\n", (u32) dev_desc->lba);
	/* Setup the Protective MBR */
	if (set_protective_mbr(dev_desc) < 0)
		goto err;

	/* Generate CRC for the Primary GPT Header */
	calc_crc32 = efi_crc32((const unsigned char *)gpt_e,
			      le32_to_cpu(gpt_h->num_partition_entries) *
			      le32_to_cpu(gpt_h->sizeof_partition_entry));
	gpt_h->partition_entry_array_crc32 = cpu_to_le32(calc_crc32);

	calc_crc32 = efi_crc32((const unsigned char *)gpt_h,
			      le32_to_cpu(gpt_h->header_size));
	gpt_h->header_crc32 = cpu_to_le32(calc_crc32);

	/* Write the First GPT to the block right after the Legacy MBR */
	if (dev_desc->block_write(dev_desc->dev, 1, 1, gpt_h) != 1)
		goto err;

	if (dev_desc->block_write(dev_desc->dev, 2, pte_blk_num, gpt_e)
	    != pte_blk_num)
		goto err;

	/* recalculate the values for the Second GPT Header */
	val = le64_to_cpu(gpt_h->my_lba);
	gpt_h->my_lba = gpt_h->alternate_lba;
	gpt_h->alternate_lba = cpu_to_le64(val);
	gpt_h->header_crc32 = 0;

	calc_crc32 = efi_crc32((const unsigned char *)gpt_h,
			      le32_to_cpu(gpt_h->header_size));
	gpt_h->header_crc32 = cpu_to_le32(calc_crc32);

	if (dev_desc->block_write(dev_desc->dev,
				  le32_to_cpu(gpt_h->last_usable_lba + 1),
				  pte_blk_num, gpt_e) != pte_blk_num)
		goto err;

	if (dev_desc->block_write(dev_desc->dev,
				  le32_to_cpu(gpt_h->my_lba), 1, gpt_h) != 1)
		goto err;

	debug("GPT successfully written to block device!\n");
	return 0;

 err:
	printf("** Can't write to device %d **\n", dev_desc->dev);
	return -1;
}

int gpt_fill_pte(gpt_header *gpt_h, gpt_entry *gpt_e,
		disk_partition_t *partitions, int parts)
{
	u32 offset = (u32)le32_to_cpu(gpt_h->first_usable_lba);
	ulong start;
	int i, k;
	size_t name_len;
#ifdef CONFIG_PARTITION_UUIDS
	char *str_uuid;
#endif

	for (i = 0; i < parts; i++) {
		/* partition starting lba */
		start = partitions[i].start;
		if (start && (start < offset)) {
			printf("Partition overlap\n");
			return -1;
		}
		if (start) {
			gpt_e[i].starting_lba = cpu_to_le64(start);
			offset = start + partitions[i].size;
		} else {
			gpt_e[i].starting_lba = cpu_to_le64(offset);
			offset += partitions[i].size;
		}
		if (offset >= gpt_h->last_usable_lba) {
			printf("Partitions layout exceds disk size\n");
			return -1;
		}
		/* partition ending lba */
		if ((i == parts - 1) && (partitions[i].size == 0))
			/* extend the last partition to maximuim */
			gpt_e[i].ending_lba = gpt_h->last_usable_lba;
		else
			gpt_e[i].ending_lba = cpu_to_le64(offset - 1);

		/* partition type GUID */
		memcpy(gpt_e[i].partition_type_guid.b,
			&PARTITION_BASIC_DATA_GUID, 16);

#ifdef CONFIG_PARTITION_UUIDS
		str_uuid = partitions[i].uuid;
		if (string_uuid(str_uuid, gpt_e[i].unique_partition_guid.b)) {
			printf("Partition no. %d: invalid guid: %s\n",
				i, str_uuid);
			return -1;
		}
#endif

		/* partition attributes */
		memset(&gpt_e[i].attributes, 0,
		       sizeof(gpt_entry_attributes));

		/* partition name */
		name_len = sizeof(gpt_e[i].partition_name)
			/ sizeof(efi_char16_t);
		for (k = 0; k < name_len; k++)
			gpt_e[i].partition_name[k] =
				(efi_char16_t)(partitions[i].name[k]);

		debug("%s: name: %s offset[%d]: 0x%x size[%d]: 0x%lx\n",
		      __func__, partitions[i].name, i,
		      offset, i, partitions[i].size);
	}

	return 0;
}

int gpt_fill_header(block_dev_desc_t *dev_desc, gpt_header *gpt_h,
		char *str_guid, int parts_count)
{
	gpt_h->signature = cpu_to_le64(GPT_HEADER_SIGNATURE);
	gpt_h->revision = cpu_to_le32(GPT_HEADER_REVISION_V1);
	gpt_h->header_size = cpu_to_le32(sizeof(gpt_header));
	gpt_h->my_lba = cpu_to_le64(1);
	gpt_h->alternate_lba = cpu_to_le64(dev_desc->lba - 1);
	gpt_h->first_usable_lba = cpu_to_le64(34);
	gpt_h->last_usable_lba = cpu_to_le64(dev_desc->lba - 34);
	gpt_h->partition_entry_lba = cpu_to_le64(2);
	gpt_h->num_partition_entries = cpu_to_le32(GPT_ENTRY_NUMBERS);
	gpt_h->sizeof_partition_entry = cpu_to_le32(sizeof(gpt_entry));
	gpt_h->header_crc32 = 0;
	gpt_h->partition_entry_array_crc32 = 0;

	if (string_uuid(str_guid, gpt_h->disk_guid.b))
		return -1;

	return 0;
}

int gpt_restore(block_dev_desc_t *dev_desc, char *str_disk_guid,
		disk_partition_t *partitions, int parts_count)
{
	int ret;

	gpt_header *gpt_h = calloc(1, sizeof(gpt_header));
	if (gpt_h == NULL) {
		printf("%s: calloc failed!\n", __func__);
		return -1;
	}

	gpt_entry *gpt_e = calloc(GPT_ENTRY_NUMBERS, sizeof(gpt_entry));
	if (gpt_e == NULL) {
		printf("%s: calloc failed!\n", __func__);
		free(gpt_h);
		return -1;
	}

	/* Generate Primary GPT header (LBA1) */
	ret = gpt_fill_header(dev_desc, gpt_h, str_disk_guid, parts_count);
	if (ret)
		goto err;

	/* Generate partition entries */
	ret = gpt_fill_pte(gpt_h, gpt_e, partitions, parts_count);
	if (ret)
		goto err;

	/* Write GPT partition table */
	ret = write_gpt_table(dev_desc, gpt_h, gpt_e);

err:
	free(gpt_e);
	free(gpt_h);
	return ret;
}
#endif

/*
 * Private functions
 */
/*
 * pmbr_part_valid(): Check for EFI partition signature
 *
 * Returns: 1 if EFI GPT partition type is found.
 */
static int pmbr_part_valid(struct partition *part)
{
	if (part->sys_ind == EFI_PMBR_OSTYPE_EFI_GPT &&
		get_unaligned_le32(&part->start_sect) == 1UL) {
		return 1;
	}

	return 0;
}

/*
 * is_pmbr_valid(): test Protective MBR for validity
 *
 * Returns: 1 if PMBR is valid, 0 otherwise.
 * Validity depends on two things:
 *  1) MSDOS signature is in the last two bytes of the MBR
 *  2) One partition of type 0xEE is found, checked by pmbr_part_valid()
 */
static int is_pmbr_valid(legacy_mbr * mbr)
{
	int i = 0;

	if (!mbr || le16_to_cpu(mbr->signature) != MSDOS_MBR_SIGNATURE)
		return 0;

	for (i = 0; i < 4; i++) {
		if (pmbr_part_valid(&mbr->partition_record[i])) {
			return 1;
		}
	}
	return 0;
}

/**
 * is_gpt_valid() - tests one GPT header and PTEs for validity
 *
 * lba is the logical block address of the GPT header to test
 * gpt is a GPT header ptr, filled on return.
 * ptes is a PTEs ptr, filled on return.
 *
 * Description: returns 1 if valid,  0 on error.
 * If valid, returns pointers to PTEs.
 */
static int is_gpt_valid(block_dev_desc_t * dev_desc, unsigned long long lba,
			gpt_header * pgpt_head, gpt_entry ** pgpt_pte)
{
	u32 crc32_backup = 0;
	u32 calc_crc32;
	unsigned long long lastlba;

	if (!dev_desc || !pgpt_head) {
		printf("%s: Invalid Argument(s)\n", __func__);
		return 0;
	}

	/* Read GPT Header from device */
	if (dev_desc->block_read(dev_desc->dev, lba, 1, pgpt_head) != 1) {
		printf("*** ERROR: Can't read GPT header ***\n");
		return 0;
	}

	/* Check the GPT header signature */
	if (le64_to_cpu(pgpt_head->signature) != GPT_HEADER_SIGNATURE) {
		printf("GUID Partition Table Header signature is wrong:"
			"0x%llX != 0x%llX\n",
			le64_to_cpu(pgpt_head->signature),
			GPT_HEADER_SIGNATURE);
		return 0;
	}

	/* Check the GUID Partition Table CRC */
	memcpy(&crc32_backup, &pgpt_head->header_crc32, sizeof(crc32_backup));
	memset(&pgpt_head->header_crc32, 0, sizeof(pgpt_head->header_crc32));

	calc_crc32 = efi_crc32((const unsigned char *)pgpt_head,
		le32_to_cpu(pgpt_head->header_size));

	memcpy(&pgpt_head->header_crc32, &crc32_backup, sizeof(crc32_backup));

	if (calc_crc32 != le32_to_cpu(crc32_backup)) {
		printf("GUID Partition Table Header CRC is wrong:"
			"0x%x != 0x%x\n",
		       le32_to_cpu(crc32_backup), calc_crc32);
		return 0;
	}

	/* Check that the my_lba entry points to the LBA that contains the GPT */
	if (le64_to_cpu(pgpt_head->my_lba) != lba) {
		printf("GPT: my_lba incorrect: %llX != %llX\n",
			le64_to_cpu(pgpt_head->my_lba),
			lba);
		return 0;
	}

	/* Check the first_usable_lba and last_usable_lba are within the disk. */
	lastlba = (unsigned long long)dev_desc->lba;
	if (le64_to_cpu(pgpt_head->first_usable_lba) > lastlba) {
		printf("GPT: first_usable_lba incorrect: %llX > %llX\n",
			le64_to_cpu(pgpt_head->first_usable_lba), lastlba);
		return 0;
	}
	if (le64_to_cpu(pgpt_head->last_usable_lba) > lastlba) {
		printf("GPT: last_usable_lba incorrect: %llX > %llX\n",
			(u64) le64_to_cpu(pgpt_head->last_usable_lba), lastlba);
		return 0;
	}

	debug("GPT: first_usable_lba: %llX last_usable_lba %llX last lba %llX\n",
		le64_to_cpu(pgpt_head->first_usable_lba),
		le64_to_cpu(pgpt_head->last_usable_lba), lastlba);

	/* Read and allocate Partition Table Entries */
	*pgpt_pte = alloc_read_gpt_entries(dev_desc, pgpt_head);
	if (*pgpt_pte == NULL) {
		printf("GPT: Failed to allocate memory for PTE\n");
		return 0;
	}

	/* Check the GUID Partition Table Entry Array CRC */
	calc_crc32 = efi_crc32((const unsigned char *)*pgpt_pte,
		le32_to_cpu(pgpt_head->num_partition_entries) *
		le32_to_cpu(pgpt_head->sizeof_partition_entry));

	if (calc_crc32 != le32_to_cpu(pgpt_head->partition_entry_array_crc32)) {
		printf("GUID Partition Table Entry Array CRC is wrong:"
			"0x%x != 0x%x\n",
			le32_to_cpu(pgpt_head->partition_entry_array_crc32),
			calc_crc32);

		free(*pgpt_pte);
		return 0;
	}

	/* We're done, all's well */
	return 1;
}

/**
 * alloc_read_gpt_entries(): reads partition entries from disk
 * @dev_desc
 * @gpt - GPT header
 *
 * Description: Returns ptes on success,  NULL on error.
 * Allocates space for PTEs based on information found in @gpt.
 * Notes: remember to free pte when you're done!
 */
static gpt_entry *alloc_read_gpt_entries(block_dev_desc_t * dev_desc,
					 gpt_header * pgpt_head)
{
	size_t count = 0;
	gpt_entry *pte = NULL;

	if (!dev_desc || !pgpt_head) {
		printf("%s: Invalid Argument(s)\n", __func__);
		return NULL;
	}

	count = le32_to_cpu(pgpt_head->num_partition_entries) *
		le32_to_cpu(pgpt_head->sizeof_partition_entry);

	debug("%s: count = %u * %u = %zu\n", __func__,
	      (u32) le32_to_cpu(pgpt_head->num_partition_entries),
	      (u32) le32_to_cpu(pgpt_head->sizeof_partition_entry), count);

	/* Allocate memory for PTE, remember to FREE */
	if (count != 0) {
		pte = memalign(ARCH_DMA_MINALIGN, count);
	}

	if (count == 0 || pte == NULL) {
		printf("%s: ERROR: Can't allocate 0x%zX "
		       "bytes for GPT Entries\n",
			__func__, count);
		return NULL;
	}

	/* Read GPT Entries from device */
	if (dev_desc->block_read (dev_desc->dev,
		le64_to_cpu(pgpt_head->partition_entry_lba),
		(lbaint_t) (count / GPT_BLOCK_SIZE), pte)
		!= (count / GPT_BLOCK_SIZE)) {

		printf("*** ERROR: Can't read GPT Entries ***\n");
		free(pte);
		return NULL;
	}
	return pte;
}

/**
 * is_pte_valid(): validates a single Partition Table Entry
 * @gpt_entry - Pointer to a single Partition Table Entry
 *
 * Description: returns 1 if valid,  0 on error.
 */
static int is_pte_valid(gpt_entry * pte)
{
	efi_guid_t unused_guid;

	if (!pte) {
		printf("%s: Invalid Argument(s)\n", __func__);
		return 0;
	}

	/* Only one validation for now:
	 * The GUID Partition Type != Unused Entry (ALL-ZERO)
	 */
	memset(unused_guid.b, 0, sizeof(unused_guid.b));

	if (memcmp(pte->partition_type_guid.b, unused_guid.b,
		sizeof(unused_guid.b)) == 0) {

		debug("%s: Found an unused PTE GUID at 0x%08X\n", __func__,
		      (unsigned int)(uintptr_t)pte);

		return 0;
	} else {
		return 1;
	}
}
#endif
