/*
 * Copyright (c) 2013 The Chromium OS Authors.
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

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <tpm.h>
#include <asm/unaligned.h>
#include <linux/string.h>

/**
 * Print a byte string in hexdecimal format, 16-bytes per line.
 *
 * @param data		byte string to be printed
 * @param count		number of bytes to be printed
 */
static void print_byte_string(uint8_t *data, size_t count)
{
	int i, print_newline = 0;

	for (i = 0; i < count; i++) {
		printf(" %02x", data[i]);
		print_newline = (i % 16 == 15);
		if (print_newline)
			putc('\n');
	}
	/* Avoid duplicated newline at the end */
	if (!print_newline)
		putc('\n');
}

/**
 * Convert a text string of hexdecimal values into a byte string.
 *
 * @param bytes		text string of hexdecimal values with no space
 *			between them
 * @param data		output buffer for byte string.  The caller has to make
 *			sure it is large enough for storing the output.  If
 *			NULL is passed, a large enough buffer will be allocated,
 *			and the caller must free it.
 * @param count_ptr	output variable for the length of byte string
 * @return pointer to output buffer
 */
static void *parse_byte_string(char *bytes, uint8_t *data, size_t *count_ptr)
{
	char byte[3];
	size_t count, length;
	int i;

	length = strlen(bytes);
	count = length / 2;

	if (!data)
		data = malloc(count);
	if (!data)
		return NULL;

	byte[2] = '\0';
	for (i = 0; i < length; i += 2) {
		byte[0] = bytes[i];
		byte[1] = bytes[i + 1];
		data[i / 2] = (uint8_t)simple_strtoul(byte, NULL, 16);
	}

	if (count_ptr)
		*count_ptr = count;

	return data;
}

/**
 * Convert TPM command return code to U-Boot command error codes.
 *
 * @param return_code	TPM command return code
 * @return value of enum command_ret_t
 */
static int convert_return_code(uint32_t return_code)
{
	if (return_code)
		return CMD_RET_FAILURE;
	else
		return CMD_RET_SUCCESS;
}

/**
 * Return number of values defined by a type string.
 *
 * @param type_str	type string
 * @return number of values of type string
 */
static int type_string_get_num_values(const char *type_str)
{
	return strlen(type_str);
}

/**
 * Return total size of values defined by a type string.
 *
 * @param type_str	type string
 * @return total size of values of type string, or 0 if type string
 *  contains illegal type character.
 */
static size_t type_string_get_space_size(const char *type_str)
{
	size_t size;

	for (size = 0; *type_str; type_str++) {
		switch (*type_str) {
		case 'b':
			size += 1;
			break;
		case 'w':
			size += 2;
			break;
		case 'd':
			size += 4;
			break;
		default:
			return 0;
		}
	}

	return size;
}

/**
 * Allocate a buffer large enough to hold values defined by a type
 * string.  The caller has to free the buffer.
 *
 * @param type_str	type string
 * @param count		pointer for storing size of buffer
 * @return pointer to buffer or NULL on error
 */
static void *type_string_alloc(const char *type_str, uint32_t *count)
{
	void *data;
	size_t size;

	size = type_string_get_space_size(type_str);
	if (!size)
		return NULL;
	data = malloc(size);
	if (data)
		*count = size;

	return data;
}

/**
 * Pack values defined by a type string into a buffer.  The buffer must have
 * large enough space.
 *
 * @param type_str	type string
 * @param values	text strings of values to be packed
 * @param data		output buffer of values
 * @return 0 on success, non-0 on error
 */
static int type_string_pack(const char *type_str, char * const values[],
		uint8_t *data)
{
	size_t offset;
	uint32_t value;

	for (offset = 0; *type_str; type_str++, values++) {
		value = simple_strtoul(values[0], NULL, 0);
		switch (*type_str) {
		case 'b':
			data[offset] = value;
			offset += 1;
			break;
		case 'w':
			put_unaligned_be16(value, data + offset);
			offset += 2;
			break;
		case 'd':
			put_unaligned_be32(value, data + offset);
			offset += 4;
			break;
		default:
			return -1;
		}
	}

	return 0;
}

/**
 * Read values defined by a type string from a buffer, and write these values
 * to environment variables.
 *
 * @param type_str	type string
 * @param data		input buffer of values
 * @param vars		names of environment variables
 * @return 0 on success, non-0 on error
 */
static int type_string_write_vars(const char *type_str, uint8_t *data,
		char * const vars[])
{
	size_t offset;
	uint32_t value;

	for (offset = 0; *type_str; type_str++, vars++) {
		switch (*type_str) {
		case 'b':
			value = data[offset];
			offset += 1;
			break;
		case 'w':
			value = get_unaligned_be16(data + offset);
			offset += 2;
			break;
		case 'd':
			value = get_unaligned_be32(data + offset);
			offset += 4;
			break;
		default:
			return -1;
		}
		if (setenv_ulong(*vars, value))
			return -1;
	}

	return 0;
}

static int do_tpm_startup(cmd_tbl_t *cmdtp, int flag,
		int argc, char * const argv[])
{
	enum tpm_startup_type mode;

	if (argc != 2)
		return CMD_RET_USAGE;
	if (!strcasecmp("TPM_ST_CLEAR", argv[1])) {
		mode = TPM_ST_CLEAR;
	} else if (!strcasecmp("TPM_ST_STATE", argv[1])) {
		mode = TPM_ST_STATE;
	} else if (!strcasecmp("TPM_ST_DEACTIVATED", argv[1])) {
		mode = TPM_ST_DEACTIVATED;
	} else {
		printf("Couldn't recognize mode string: %s\n", argv[1]);
		return CMD_RET_FAILURE;
	}

	return convert_return_code(tpm_startup(mode));
}

static int do_tpm_nv_define_space(cmd_tbl_t *cmdtp, int flag,
		int argc, char * const argv[])
{
	uint32_t index, perm, size;

	if (argc != 4)
		return CMD_RET_USAGE;
	index = simple_strtoul(argv[1], NULL, 0);
	perm = simple_strtoul(argv[2], NULL, 0);
	size = simple_strtoul(argv[3], NULL, 0);

	return convert_return_code(tpm_nv_define_space(index, perm, size));
}

static int do_tpm_nv_read_value(cmd_tbl_t *cmdtp, int flag,
		int argc, char * const argv[])
{
	uint32_t index, count, rc;
	void *data;

	if (argc != 4)
		return CMD_RET_USAGE;
	index = simple_strtoul(argv[1], NULL, 0);
	data = (void *)simple_strtoul(argv[2], NULL, 0);
	count = simple_strtoul(argv[3], NULL, 0);

	rc = tpm_nv_read_value(index, data, count);
	if (!rc) {
		puts("area content:\n");
		print_byte_string(data, count);
	}

	return convert_return_code(rc);
}

static int do_tpm_nv_write_value(cmd_tbl_t *cmdtp, int flag,
		int argc, char * const argv[])
{
	uint32_t index, rc;
	size_t count;
	void *data;

	if (argc != 3)
		return CMD_RET_USAGE;
	index = simple_strtoul(argv[1], NULL, 0);
	data = parse_byte_string(argv[2], NULL, &count);
	if (!data) {
		printf("Couldn't parse byte string %s\n", argv[2]);
		return CMD_RET_FAILURE;
	}

	rc = tpm_nv_write_value(index, data, count);
	free(data);

	return convert_return_code(rc);
}

static int do_tpm_extend(cmd_tbl_t *cmdtp, int flag,
		int argc, char * const argv[])
{
	uint32_t index, rc;
	uint8_t in_digest[20], out_digest[20];

	if (argc != 3)
		return CMD_RET_USAGE;
	index = simple_strtoul(argv[1], NULL, 0);
	if (!parse_byte_string(argv[2], in_digest, NULL)) {
		printf("Couldn't parse byte string %s\n", argv[2]);
		return CMD_RET_FAILURE;
	}

	rc = tpm_extend(index, in_digest, out_digest);
	if (!rc) {
		puts("PCR value after execution of the command:\n");
		print_byte_string(out_digest, sizeof(out_digest));
	}

	return convert_return_code(rc);
}

static int do_tpm_pcr_read(cmd_tbl_t *cmdtp, int flag,
		int argc, char * const argv[])
{
	uint32_t index, count, rc;
	void *data;

	if (argc != 4)
		return CMD_RET_USAGE;
	index = simple_strtoul(argv[1], NULL, 0);
	data = (void *)simple_strtoul(argv[2], NULL, 0);
	count = simple_strtoul(argv[3], NULL, 0);

	rc = tpm_pcr_read(index, data, count);
	if (!rc) {
		puts("Named PCR content:\n");
		print_byte_string(data, count);
	}

	return convert_return_code(rc);
}

static int do_tpm_tsc_physical_presence(cmd_tbl_t *cmdtp, int flag,
		int argc, char * const argv[])
{
	uint16_t presence;

	if (argc != 2)
		return CMD_RET_USAGE;
	presence = (uint16_t)simple_strtoul(argv[1], NULL, 0);

	return convert_return_code(tpm_tsc_physical_presence(presence));
}

static int do_tpm_read_pubek(cmd_tbl_t *cmdtp, int flag,
		int argc, char * const argv[])
{
	uint32_t count, rc;
	void *data;

	if (argc != 3)
		return CMD_RET_USAGE;
	data = (void *)simple_strtoul(argv[1], NULL, 0);
	count = simple_strtoul(argv[2], NULL, 0);

	rc = tpm_read_pubek(data, count);
	if (!rc) {
		puts("pubek value:\n");
		print_byte_string(data, count);
	}

	return convert_return_code(rc);
}

static int do_tpm_physical_set_deactivated(cmd_tbl_t *cmdtp, int flag,
		int argc, char * const argv[])
{
	uint8_t state;

	if (argc != 2)
		return CMD_RET_USAGE;
	state = (uint8_t)simple_strtoul(argv[1], NULL, 0);

	return convert_return_code(tpm_physical_set_deactivated(state));
}

static int do_tpm_get_capability(cmd_tbl_t *cmdtp, int flag,
		int argc, char * const argv[])
{
	uint32_t cap_area, sub_cap, rc;
	void *cap;
	size_t count;

	if (argc != 5)
		return CMD_RET_USAGE;
	cap_area = simple_strtoul(argv[1], NULL, 0);
	sub_cap = simple_strtoul(argv[2], NULL, 0);
	cap = (void *)simple_strtoul(argv[3], NULL, 0);
	count = simple_strtoul(argv[4], NULL, 0);

	rc = tpm_get_capability(cap_area, sub_cap, cap, count);
	if (!rc) {
		puts("capability information:\n");
		print_byte_string(cap, count);
	}

	return convert_return_code(rc);
}

#define TPM_COMMAND_NO_ARG(cmd)				\
static int do_##cmd(cmd_tbl_t *cmdtp, int flag,		\
		int argc, char * const argv[])		\
{							\
	if (argc != 1)					\
		return CMD_RET_USAGE;			\
	return convert_return_code(cmd());		\
}

TPM_COMMAND_NO_ARG(tpm_init)
TPM_COMMAND_NO_ARG(tpm_self_test_full)
TPM_COMMAND_NO_ARG(tpm_continue_self_test)
TPM_COMMAND_NO_ARG(tpm_force_clear)
TPM_COMMAND_NO_ARG(tpm_physical_enable)
TPM_COMMAND_NO_ARG(tpm_physical_disable)

static int do_tpm_raw_transfer(cmd_tbl_t *cmdtp, int flag,
		int argc, char * const argv[])
{
	void *command;
	uint8_t response[1024];
	size_t count, response_length = sizeof(response);
	uint32_t rc;

	command = parse_byte_string(argv[1], NULL, &count);
	if (!command) {
		printf("Couldn't parse byte string %s\n", argv[1]);
		return CMD_RET_FAILURE;
	}

	rc = tis_sendrecv(command, count, response, &response_length);
	free(command);
	if (!rc) {
		puts("tpm response:\n");
		print_byte_string(response, response_length);
	}

	return convert_return_code(rc);
}

static int do_tpm_nv_define(cmd_tbl_t *cmdtp, int flag,
		int argc, char * const argv[])
{
	uint32_t index, perm, size;

	if (argc != 4)
		return CMD_RET_USAGE;
	size = type_string_get_space_size(argv[1]);
	if (!size) {
		printf("Couldn't parse arguments\n");
		return CMD_RET_USAGE;
	}
	index = simple_strtoul(argv[2], NULL, 0);
	perm = simple_strtoul(argv[3], NULL, 0);

	return convert_return_code(tpm_nv_define_space(index, perm, size));
}

static int do_tpm_nv_read(cmd_tbl_t *cmdtp, int flag,
		int argc, char * const argv[])
{
	uint32_t index, count, err;
	void *data;

	if (argc < 3)
		return CMD_RET_USAGE;
	if (argc != 3 + type_string_get_num_values(argv[1]))
		return CMD_RET_USAGE;
	index = simple_strtoul(argv[2], NULL, 0);
	data = type_string_alloc(argv[1], &count);
	if (!data) {
		printf("Couldn't parse arguments\n");
		return CMD_RET_USAGE;
	}

	err = tpm_nv_read_value(index, data, count);
	if (!err) {
		if (type_string_write_vars(argv[1], data, argv + 3)) {
			printf("Couldn't write to variables\n");
			err = ~0;
		}
	}
	free(data);

	return convert_return_code(err);
}

static int do_tpm_nv_write(cmd_tbl_t *cmdtp, int flag,
		int argc, char * const argv[])
{
	uint32_t index, count, err;
	void *data;

	if (argc < 3)
		return CMD_RET_USAGE;
	if (argc != 3 + type_string_get_num_values(argv[1]))
		return CMD_RET_USAGE;
	index = simple_strtoul(argv[2], NULL, 0);
	data = type_string_alloc(argv[1], &count);
	if (!data) {
		printf("Couldn't parse arguments\n");
		return CMD_RET_USAGE;
	}
	if (type_string_pack(argv[1], argv + 3, data)) {
		printf("Couldn't parse arguments\n");
		free(data);
		return CMD_RET_USAGE;
	}

	err = tpm_nv_write_value(index, data, count);
	free(data);

	return convert_return_code(err);
}

#define MAKE_TPM_CMD_ENTRY(cmd) \
	U_BOOT_CMD_MKENT(cmd, 0, 1, do_tpm_ ## cmd, "", "")

static cmd_tbl_t tpm_commands[] = {
	U_BOOT_CMD_MKENT(init, 0, 1,
			do_tpm_init, "", ""),
	U_BOOT_CMD_MKENT(startup, 0, 1,
			do_tpm_startup, "", ""),
	U_BOOT_CMD_MKENT(self_test_full, 0, 1,
			do_tpm_self_test_full, "", ""),
	U_BOOT_CMD_MKENT(continue_self_test, 0, 1,
			do_tpm_continue_self_test, "", ""),
	U_BOOT_CMD_MKENT(force_clear, 0, 1,
			do_tpm_force_clear, "", ""),
	U_BOOT_CMD_MKENT(physical_enable, 0, 1,
			do_tpm_physical_enable, "", ""),
	U_BOOT_CMD_MKENT(physical_disable, 0, 1,
			do_tpm_physical_disable, "", ""),
	U_BOOT_CMD_MKENT(nv_define_space, 0, 1,
			do_tpm_nv_define_space, "", ""),
	U_BOOT_CMD_MKENT(nv_read_value, 0, 1,
			do_tpm_nv_read_value, "", ""),
	U_BOOT_CMD_MKENT(nv_write_value, 0, 1,
			do_tpm_nv_write_value, "", ""),
	U_BOOT_CMD_MKENT(extend, 0, 1,
			do_tpm_extend, "", ""),
	U_BOOT_CMD_MKENT(pcr_read, 0, 1,
			do_tpm_pcr_read, "", ""),
	U_BOOT_CMD_MKENT(tsc_physical_presence, 0, 1,
			do_tpm_tsc_physical_presence, "", ""),
	U_BOOT_CMD_MKENT(read_pubek, 0, 1,
			do_tpm_read_pubek, "", ""),
	U_BOOT_CMD_MKENT(physical_set_deactivated, 0, 1,
			do_tpm_physical_set_deactivated, "", ""),
	U_BOOT_CMD_MKENT(get_capability, 0, 1,
			do_tpm_get_capability, "", ""),
	U_BOOT_CMD_MKENT(raw_transfer, 0, 1,
			do_tpm_raw_transfer, "", ""),
	U_BOOT_CMD_MKENT(nv_define, 0, 1,
			do_tpm_nv_define, "", ""),
	U_BOOT_CMD_MKENT(nv_read, 0, 1,
			do_tpm_nv_read, "", ""),
	U_BOOT_CMD_MKENT(nv_write, 0, 1,
			do_tpm_nv_write, "", ""),
};

static int do_tpm(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	cmd_tbl_t *tpm_cmd;

	if (argc < 2)
		return CMD_RET_USAGE;
	tpm_cmd = find_cmd_tbl(argv[1], tpm_commands, ARRAY_SIZE(tpm_commands));
	if (!tpm_cmd)
		return CMD_RET_USAGE;

	return tpm_cmd->cmd(cmdtp, flag, argc - 1, argv + 1);
}

U_BOOT_CMD(tpm, CONFIG_SYS_MAXARGS, 1, do_tpm,
"Issue a TPM command",
"cmd args...\n"
"    - Issue TPM command <cmd> with arguments <args...>.\n"
"Admin Startup and State Commands:\n"
"  init\n"
"    - Put TPM into a state where it waits for 'startup' command.\n"
"  startup mode\n"
"    - Issue TPM_Starup command.  <mode> is one of TPM_ST_CLEAR,\n"
"      TPM_ST_STATE, and TPM_ST_DEACTIVATED.\n"
"Admin Testing Commands:\n"
"  self_test_full\n"
"    - Test all of the TPM capabilities.\n"
"  continue_self_test\n"
"    - Inform TPM that it should complete the self-test.\n"
"Admin Opt-in Commands:\n"
"  physical_enable\n"
"    - Set the PERMANENT disable flag to FALSE using physical presence as\n"
"      authorization.\n"
"  physical_disable\n"
"    - Set the PERMANENT disable flag to TRUE using physical presence as\n"
"      authorization.\n"
"  physical_set_deactivated 0|1\n"
"    - Set deactivated flag.\n"
"Admin Ownership Commands:\n"
"  force_clear\n"
"    - Issue TPM_ForceClear command.\n"
"  tsc_physical_presence flags\n"
"    - Set TPM device's Physical Presence flags to <flags>.\n"
"The Capability Commands:\n"
"  get_capability cap_area sub_cap addr count\n"
"    - Read <count> bytes of TPM capability indexed by <cap_area> and\n"
"      <sub_cap> to memory address <addr>.\n"
"Endorsement Key Handling Commands:\n"
"  read_pubek addr count\n"
"    - Read <count> bytes of the public endorsement key to memory\n"
"      address <addr>\n"
"Integrity Collection and Reporting Commands:\n"
"  extend index digest_hex_string\n"
"    - Add a new measurement to a PCR.  Update PCR <index> with the 20-bytes\n"
"      <digest_hex_string>\n"
"  pcr_read index addr count\n"
"    - Read <count> bytes from PCR <index> to memory address <addr>.\n"
"Non-volatile Storage Commands:\n"
"  nv_define_space index permission size\n"
"    - Establish a space at index <index> with <permission> of <size> bytes.\n"
"  nv_read_value index addr count\n"
"    - Read <count> bytes from space <index> to memory address <addr>.\n"
"  nv_write_value index addr count\n"
"    - Write <count> bytes from memory address <addr> to space <index>.\n"
"Miscellaneous helper functions:\n"
"  raw_transfer byte_string\n"
"    - Send a byte string <byte_string> to TPM and print the response.\n"
" Non-volatile storage helper functions:\n"
"    These helper functions treat a non-volatile space as a non-padded\n"
"    sequence of integer values.  These integer values are defined by a type\n"
"    string, which is a text string of 'bwd' characters: 'b' means a 8-bit\n"
"    value, 'w' 16-bit value, 'd' 32-bit value.  All helper functions take\n"
"    a type string as their first argument.\n"
"  nv_define type_string index perm\n"
"    - Define a space <index> with permission <perm>.\n"
"  nv_read types_string index vars...\n"
"    - Read from space <index> to environment variables <vars...>.\n"
"  nv_write types_string index values...\n"
"    - Write to space <index> from values <values...>.\n"
);
