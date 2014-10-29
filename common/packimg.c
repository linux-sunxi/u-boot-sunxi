#include <common.h>
#include <nand.h>
#include <packimg.h>

static uint32_t calc_crc(void *buff, int size)
{
	int i;
	uint32_t ret = 0;
	for (i = 0; i < size; i += 4)
		ret += *(uint32_t *)(buff + i);
	return ret;
}

int packimg_read(nand_info_t *nand, uint32_t nand_off, uint32_t nand_size)
{
	int i, err;
	size_t size;
	char buff[512];
	struct pack_header *ph;
	struct pack_entry *pe;
	uint32_t offs = nand_off, crc;

	if ((nand_off & (nand->erasesize - 1)) || (nand_size & (nand->erasesize - 1))) {
		printf("offset %x and size %x must be block aligned\n", nand_off, nand_size);
		return -1;
	}

	while (offs < nand_off + nand_size) {
		size = 512;
		err = nand_read_skip_bad(nand, offs, &size, NULL,
					       nand->size, (void *)buff);
		if (err) {
			printf("nand read offset %x fail\n", offs);
			return err;
		}
		ph = (void *)buff;
		pe = (void *)buff + sizeof(*ph);

		// check valid header
		if (ph->magic != PACK_MAGIC)
			goto next_block;
		crc = calc_crc(pe, ph->nentry * sizeof(*pe));
		if (ph->crc != crc)
			goto next_block;

		// load all entries
		for (i = 0; i < ph->nentry; i++) {
			size = pe[i].size;
			err = nand_read_skip_bad(nand, offs + pe[i].offset, &size, NULL, nand->size, (void *)pe[i].ldaddr);
			if (err) {
				printf("nand read offset %x size %x to %x fail\n",
					   offs + pe[i].offset, pe[i].size, pe[i].ldaddr);
				return err;
			}
			crc = calc_crc((void *)pe[i].ldaddr, pe[i].size);
			if (pe[i].crc != crc)
				goto next_block;
		}

		printf("load packimg at %x success\n", offs);
		return 0;

	next_block:
		printf("invalid packimg at offset %x\n", offs);
		offs += nand->erasesize;
	}

	printf("load packimg from %x to %x fail\n", nand_off, nand_size);
	return -1;
}

int packimg_write(nand_info_t *nand, uint32_t nand_off, uint32_t nand_size, uint32_t mem_off, 
				  uint32_t mem_size, uint32_t max_copy)
{
	int err;
	int nsize = nand_size, msize, copies = 0;
	uint32_t noffs = nand_off, moffs;

	if ((nand_off & (nand->erasesize - 1)) || (nand_size & (nand->erasesize - 1))) {
		printf("offset %x and size %x must be block aligned\n", nand_off, nand_size);
		return -1;
	}

	while (nsize >= mem_size && copies < max_copy) {
		moffs = mem_off;
		msize = mem_size;

		while (msize > 0) {
			uint32_t size;
			while (nand_block_isbad(nand, noffs)) {
				noffs += nand->erasesize;
				nsize -= nand->erasesize;
				if (nsize < msize)
					goto out;
			}

			err = nand_erase(nand, noffs, nand->erasesize);
			if (err) {
				printf("erase block %x fail\n", noffs);
				goto out;
			}

			size = msize > nand->erasesize ? nand->erasesize : msize;
			err = nand_write(nand, noffs, &size, (void *)moffs);
			if (err) {
				printf("write block %x fail\n", noffs);
				goto out;
			}

			moffs += nand->erasesize;
			msize -= nand->erasesize;
			noffs += nand->erasesize;
			nsize -= nand->erasesize;
		}

		copies++;
	}

out:
	if (copies) {
		printf("success write %d/%d copies of packimg to %x\n", copies, max_copy, nand_off);
		return 0;
	}
	else {
		printf("fail write packimg to %x\n", nand_off);
		return -1;
	}
}
