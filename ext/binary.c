
/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2014 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  |          ZhuZongXin <dreamsxin@qq.com>                                 |
  +------------------------------------------------------------------------+
*/

#include "binary.h"
#include "di.h"
#include "filterinterface.h"

#include <ext/standard/php_array.h>
#include <ext/spl/spl_array.h>

#include <Zend/zend_closures.h>

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/fcall.h"
#include "kernel/string.h"
#include "kernel/array.h"
#include "kernel/operators.h"
#include "kernel/concat.h"
#include "kernel/object.h"
#include "kernel/exception.h"

#include "interned-strings.h"

/**
 * Phalcon\Binary
 *
 * Provides utilities to work with binary data
 */
zend_class_entry *phalcon_binary_ce;

PHP_METHOD(Phalcon_Binary, pack);
PHP_METHOD(Phalcon_Binary, unpack);
PHP_METHOD(Phalcon_Binary, setbit);
PHP_METHOD(Phalcon_Binary, getbit);
PHP_METHOD(Phalcon_Binary, toBitstring);

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_binary_pack, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, endian, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_binary_unpack, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, endian, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_binary_setbit, 1, 0, 3)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_binary_getbit, 1, 0, 2)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_binary_tobitstring, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry phalcon_binary_method_entry[] = {
	PHP_ME(Phalcon_Binary, pack, arginfo_phalcon_binary_pack, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(Phalcon_Binary, unpack, arginfo_phalcon_binary_unpack, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(Phalcon_Binary, setbit, arginfo_phalcon_binary_setbit, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(Phalcon_Binary, getbit, arginfo_phalcon_binary_getbit, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(Phalcon_Binary, toBitstring, arginfo_phalcon_binary_tobitstring, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_FE_END
};

/**
 * Phalcon\Binary initializer
 */
PHALCON_INIT_CLASS(Phalcon_Binary){

	PHALCON_REGISTER_CLASS(Phalcon, Binary, binary, phalcon_binary_method_entry, ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);

	zend_declare_class_constant_long(phalcon_binary_ce, SL("TYPE_CHAR"), PHALCON_BINARY_TYPE_CHAR);
	zend_declare_class_constant_long(phalcon_binary_ce, SL("TYPE_UNSIGNEDCHAR"), PHALCON_BINARY_TYPE_UNSIGNEDCHAR);
	zend_declare_class_constant_long(phalcon_binary_ce, SL("TYPE_INT16"), PHALCON_BINARY_TYPE_INT16);
	zend_declare_class_constant_long(phalcon_binary_ce, SL("TYPE_UNSIGNEDINT16"), PHALCON_BINARY_TYPE_UNSIGNEDINT16);
	zend_declare_class_constant_long(phalcon_binary_ce, SL("TYPE_INT"), PHALCON_BINARY_TYPE_INT);
	zend_declare_class_constant_long(phalcon_binary_ce, SL("TYPE_UNSIGNEDINT"), PHALCON_BINARY_TYPE_UNSIGNEDINT);
	zend_declare_class_constant_long(phalcon_binary_ce, SL("TYPE_INT32"), PHALCON_BINARY_TYPE_INT32);
	zend_declare_class_constant_long(phalcon_binary_ce, SL("TYPE_UNSIGNEDINT32"), PHALCON_BINARY_TYPE_UNSIGNEDINT32);
	zend_declare_class_constant_long(phalcon_binary_ce, SL("TYPE_FLOAT"), PHALCON_BINARY_TYPE_FLOAT);
	zend_declare_class_constant_long(phalcon_binary_ce, SL("TYPE_DOUBLE"), PHALCON_BINARY_TYPE_DOUBLE);
	//zend_declare_class_constant_long(phalcon_binary_ce, SL("TYPE_STRING"), PHALCON_BINARY_TYPE_STRING);
	//zend_declare_class_constant_long(phalcon_binary_ce, SL("TYPE_HEXSTRING"), PHALCON_BINARY_TYPE_HEXSTRING);

	zend_declare_class_constant_long(phalcon_binary_ce, SL("ENDIAN_MACHINE"), PHALCON_BINARY_ENDIAN_MACHINE);
	zend_declare_class_constant_long(phalcon_binary_ce, SL("ENDIAN_BIG"), PHALCON_BINARY_ENDIAN_BIG);
	zend_declare_class_constant_long(phalcon_binary_ce, SL("ENDIAN_LITTLE"), PHALCON_BINARY_ENDIAN_LITTLE);
	return SUCCESS;
}

/**
 * Pack data into binary string
 *
 * @param int $type
 * @param string $data
 * @param int $endian
 * @return mixed
 **/
PHP_METHOD(Phalcon_Binary, pack)
{
	zval *type, *data, *endian = NULL, format = {};

	phalcon_fetch_params(1, 2, 1, &type, &data, &endian);

	if (!endian || Z_TYPE_P(endian) == IS_NULL) {
		endian = &PHALCON_GLOBAL(z_one);
	}

	switch (Z_LVAL_P(type)) {
		case PHALCON_BINARY_TYPE_CHAR:
			PHALCON_MM_ZVAL_STRING(&format, "c");
			break;
		case PHALCON_BINARY_TYPE_UNSIGNEDCHAR:
			PHALCON_MM_ZVAL_STRING(&format, "C");
			break;
		case PHALCON_BINARY_TYPE_INT16:
			PHALCON_MM_ZVAL_STRING(&format, "s");
			break;
		case PHALCON_BINARY_TYPE_UNSIGNEDINT16:
			if (Z_LVAL_P(endian) == PHALCON_BINARY_ENDIAN_LITTLE) {
				PHALCON_MM_ZVAL_STRING(&format, "v");
			} else if (Z_LVAL_P(endian) == PHALCON_BINARY_ENDIAN_MACHINE) {
				PHALCON_MM_ZVAL_STRING(&format, "S");
			} else { // PHALCON_BINARY_ENDIAN_BIG
				PHALCON_MM_ZVAL_STRING(&format, "n");
			}
			break;
		case PHALCON_BINARY_TYPE_INT:
			PHALCON_MM_ZVAL_STRING(&format, "i");
			break;
		case PHALCON_BINARY_TYPE_UNSIGNEDINT:
			PHALCON_MM_ZVAL_STRING(&format, "I");
			break;
		case PHALCON_BINARY_TYPE_INT32:
			PHALCON_MM_ZVAL_STRING(&format, "l");
			break;
		case PHALCON_BINARY_TYPE_UNSIGNEDINT32:
			if (Z_LVAL_P(endian) == PHALCON_BINARY_ENDIAN_LITTLE) {
				PHALCON_MM_ZVAL_STRING(&format, "V");
			} else if (Z_LVAL_P(endian) == PHALCON_BINARY_ENDIAN_MACHINE) {
				PHALCON_MM_ZVAL_STRING(&format, "L");
			} else { // PHALCON_BINARY_ENDIAN_BIG
				PHALCON_MM_ZVAL_STRING(&format, "N");
			}
			break;
		case PHALCON_BINARY_TYPE_FLOAT:
			PHALCON_MM_ZVAL_STRING(&format, "f");
			break;
		case PHALCON_BINARY_TYPE_DOUBLE:
			PHALCON_MM_ZVAL_STRING(&format, "d");
			break;
		default:
			RETURN_MM_FALSE;
	}

	PHALCON_MM_CALL_FUNCTION(return_value, "pack", &format, data);
	RETURN_MM();
}

/**
 * Unpack data from binary string
 *
 * @param int $type
 * @param string $data
 * @param int $endian
 * @return mixed
 **/
PHP_METHOD(Phalcon_Binary, unpack)
{
	zval *type, *data, *endian = NULL, format = {}, result = {};

	phalcon_fetch_params(1, 2, 1, &type, &data, &endian);

	if (!endian || Z_TYPE_P(endian) == IS_NULL) {
		endian = &PHALCON_GLOBAL(z_one);
	}

	switch (Z_LVAL_P(type)) {
		case PHALCON_BINARY_TYPE_CHAR:
			PHALCON_MM_ZVAL_STRING(&format, "c");
			break;
		case PHALCON_BINARY_TYPE_UNSIGNEDCHAR:
			PHALCON_MM_ZVAL_STRING(&format, "C");
			break;
		case PHALCON_BINARY_TYPE_INT16:
			PHALCON_MM_ZVAL_STRING(&format, "s");
			break;
		case PHALCON_BINARY_TYPE_UNSIGNEDINT16:
			if (Z_LVAL_P(endian) == PHALCON_BINARY_ENDIAN_LITTLE) {
				PHALCON_MM_ZVAL_STRING(&format, "v");
			} else if (Z_LVAL_P(endian) == PHALCON_BINARY_ENDIAN_MACHINE) {
				PHALCON_MM_ZVAL_STRING(&format, "S");
			} else { // PHALCON_BINARY_ENDIAN_BIG
				PHALCON_MM_ZVAL_STRING(&format, "n");
			}
			break;
		case PHALCON_BINARY_TYPE_INT:
			PHALCON_MM_ZVAL_STRING(&format, "i");
			break;
		case PHALCON_BINARY_TYPE_UNSIGNEDINT:
			PHALCON_MM_ZVAL_STRING(&format, "I");
			break;
		case PHALCON_BINARY_TYPE_INT32:
			PHALCON_MM_ZVAL_STRING(&format, "l");
			break;
		case PHALCON_BINARY_TYPE_UNSIGNEDINT32:
			if (Z_LVAL_P(endian) == PHALCON_BINARY_ENDIAN_LITTLE) {
				PHALCON_MM_ZVAL_STRING(&format, "V");
			} else if (Z_LVAL_P(endian) == PHALCON_BINARY_ENDIAN_MACHINE) {
				PHALCON_MM_ZVAL_STRING(&format, "L");
			} else { // PHALCON_BINARY_ENDIAN_BIG
				PHALCON_MM_ZVAL_STRING(&format, "N");
			}
			break;
		case PHALCON_BINARY_TYPE_FLOAT:
			PHALCON_MM_ZVAL_STRING(&format, "f");
			break;
		case PHALCON_BINARY_TYPE_DOUBLE:
			PHALCON_MM_ZVAL_STRING(&format, "d");
			break;
		default:
			RETURN_MM_FALSE;
	}

	PHALCON_MM_CALL_FUNCTION(&result, "unpack", &format, data);
	PHALCON_MM_ADD_ENTRY(&result);

	if (!phalcon_array_isset_fetch_long(return_value, &result, 1, PH_COPY)) {
		RETURN_MM_NULL();
	}
	RETURN_MM();
}

/**
 * Changes a single bit of a string.
 *
 * @param string $data
 * @param int $offset
 * @param int $value
 * @return bool
 **/
PHP_METHOD(Phalcon_Binary, setbit)
{
	zval *data, *offset, *value;
	zend_long byte_offset;

	phalcon_fetch_params(0, 3, 0, &data, &offset, &value);

	if (offset < 0) {
		php_error_docref(NULL, E_NOTICE, "Offset must be greater than or equal to 0");
		RETURN_FALSE;
	}
    byte_offset = Z_LVAL_P(offset) / 8;
	if (byte_offset >= Z_STRLEN_P(data)) {
		zend_string *c;
		c = Z_STR_P(data);
		if (IS_INTERNED(c)) {
			php_error_docref(NULL, E_NOTICE, "Cannot Change interned string");
			RETURN_FALSE;
		}
		zend_string_realloc(c, byte_offset+1, 0);
		ZVAL_STR(data, c);
	}
	if (zend_is_true(value)) {
		PHALCON_BINARY_SETBIT(Z_STRVAL_P(data), Z_LVAL_P(offset));
	} else {
		PHALCON_BINARY_UNSETBIT(Z_STRVAL_P(data), Z_LVAL_P(offset));
	}
	RETURN_TRUE;
}

/**
 * Gets a single bit of a string.
 *
 * @param string $data
 * @param int $offset
 * @return int
 **/
PHP_METHOD(Phalcon_Binary, getbit)
{
	zval *data, *offset;
	zend_long byte_offset;

	phalcon_fetch_params(0, 2, 0, &data, &offset);

	if (offset < 0) {
		php_error_docref(NULL, E_NOTICE, "Offset must be greater than or equal to 0");
		RETURN_FALSE;
	}
    byte_offset = Z_LVAL_P(offset) / 8;
	if (byte_offset >= Z_STRLEN_P(data)) {
		RETURN_FALSE;
	}
	RETURN_LONG(PHALCON_BINARY_CHECKBIT(Z_STRVAL_P(data), Z_LVAL_P(offset)) ? 1 : 0);
}

/**
 * Print binary string.
 *
 * @param string $data
 * @return string
 **/
PHP_METHOD(Phalcon_Binary, toBitstring)
{
	zval *data;
	zend_long byte_length;
	smart_str str = {0};
	int i;

	phalcon_fetch_params(0, 1, 0, &data);

    byte_length = Z_STRLEN_P(data);
	for (i=0; i<byte_length; i++) {
		int j;
		for (j=0; j<8; j++) {
			if (PHALCON_BINARY_CHECKBIT(Z_STRVAL_P(data), (i*8)+j)) {
				smart_str_appendc(&str, '1');
			} else {
				smart_str_appendc(&str, '0');
			}
		}
	}

	smart_str_0(&str);

	if (str.s) {
		RETURN_STR(str.s);
	} else {
		smart_str_free(&str);
		RETURN_FALSE;
	}
}
