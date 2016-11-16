/*
  Quite-Alright JSON for C - https://github.com/USESystemEngineeringBV/qajson4c

  Licensed under the MIT License <http://opensource.org/licenses/MIT>.

  Copyright (c) 2016 Pascal Proksch - USE System Engineering BV

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef QAJ4C_H_
#define QAJ4C_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

struct QAJ4C_Document;
typedef struct QAJ4C_Document QAJ4C_Document;

struct QAJ4C_Value;
typedef struct QAJ4C_Value QAJ4C_Value;

struct QAJ4C_Member;
typedef struct QAJ4C_Member QAJ4C_Member;

struct QAJ4C_Builder {
    uint8_t* buffer;
    size_t buffer_size;

    size_t cur_str_pos;
    size_t cur_obj_pos;
};
typedef struct QAJ4C_Builder QAJ4C_Builder;

typedef enum QAJ4C_ERROR_CODES {
    QAJ4C_ERROR_DEPTH_OVERFLOW = 1,
    QAJ4C_ERROR_UNEXPECTED_CHAR = 2,
    QAJ4C_ERROR_BUFFER_TRUNCATED = 3,
    QAJ4C_ERROR_INVALID_STRING_START = 4,
    QAJ4C_ERROR_INVALID_NUMBER_FORMAT = 5,
    QAJ4C_ERROR_UNEXPECTED_JSON_APPENDIX = 6,
    QAJ4C_ERROR_ARRAY_MISSING_COMMA = 7,
    QAJ4C_ERROR_OBJECT_MISSING_COLON = 8,
    QAJ4C_ERROR_FATAL_PARSER_ERROR = 9,
} QAJ4C_ERROR_CODES;

void QAJ4C_print_stats();

/**
 * This method will walk through the json message and analyue what buffer size would be required
 * to store the complete DOM.
 */
unsigned QAJ4C_calculate_max_buffer_size( const char* json );

/**
 * This method will walk through the json message and analyze the maximum required buffer size that
 * would be required to store the DOM (in case the strings do not have to be copied into the buffer)
 */
unsigned QAJ4C_calculate_max_buffer_size_insitu( const char* json );

/**
 * This method will parse the json message and will use the handed over buffer to store the DOM
 * and the strings.
 *
 * In case the parse fails the document's root value will contain an error value. Usually, a
 * object is expected so you should in each case check the document's root value with QAJ4C_is_object.
 */
const QAJ4C_Document* QAJ4C_parse( const char* json, void* buffer, size_t buffer_size );

/**
 * This method will parse the json message and will use the handed over buffer to store the DOM.
 * The strings will not be copied within the buffer and will be referenced to the json message.
 * Just like strtok, the json message will be adjusted in place.
 *
 * In case the parse fails the document's root value will contain an error value. Usually, a
 * object is expected so you should in each case check the document's root value with QAJ4C_is_object.
 */
const QAJ4C_Document* QAJ4C_parse_insitu( char* json, void* buffer, size_t buffer_size );

/**
 * This method prints the DOM as JSON in the handed over buffer.
 */
size_t QAJ4C_sprint( const QAJ4C_Document* document, char* buffer, size_t buffer_size );

/**
 * This method will retrieve the root value of the document. This method will only return NULL
 * in case the document itself is NULL.
 */
const QAJ4C_Value* QAJ4C_get_root_value( const QAJ4C_Document* document );


/**
 * This method will return true, in case the value can be read out as string.
 */
bool QAJ4C_is_string( const QAJ4C_Value* value );

/**
 * This method will return the starting pointer of the c-string.
 *
 * @Note: This method should only be invoked on fields that are known to
 * be string. It is usually a good idea to call the QAJ4C_is_string method first!
 * @Note: With UTF-8 the \0 character may also be placed within the string,
 * thus in case you expect UTF-8 it might be a good idea to also read the
 * QAJ4C_get_string_length method.
 */

const char* QAJ4C_get_string( const QAJ4C_Value* value );

/**
 * This method will return string length of the string.
 *
 * @Note: With UTF-8 the \0 character may also be placed within the string.
 * For this reason all string operations are also available with a defined
 * string size.
 */
unsigned QAJ4C_get_string_length( const QAJ4C_Value* value );

/**
 * This method will compare the value's string value with the handed over string
 * with the given length.
 *
 * Has the same behavior like strcmp.
 */
int QAJ4C_string_cmp2( const QAJ4C_Value* value, const char* str, size_t len );

/**
 * This method will compare the value's string value with the handed over string
 * using strlen to determine the strings size.
 *
 * Has the same return behavior like strcmp.
 */
static inline bool QAJ4C_string_cmp( const QAJ4C_Value* value, const char* str ) {
    return QAJ4C_string_cmp2(value, str, strlen(str));
}

/**
 * This method will return true, in case the handed over string with the given size
 * is equal to the value's string.
 */
static inline bool QAJ4C_string_equals2( const QAJ4C_Value* value, const char* str, size_t len ) {
    return QAJ4C_string_cmp2(value, str, len) == 0;
}

/**
 * This method will return true, in case the handed over string is equal to the
 * value's string. The string's length is determined using strlen.
 */
static inline bool QAJ4C_string_equals( const QAJ4C_Value* value, const char* str ) {
    return QAJ4C_string_equals2(value, str, strlen(str));
}

/**
 * This method will return true, in case the value can be read out as object.
 */
bool QAJ4C_is_object( const QAJ4C_Value* value );

/**
 * This method will return true, in case the value can be read out as array.
 */
bool QAJ4C_is_array( const QAJ4C_Value* value );

/**
 * This method will return true, in case the value can be read out as int32_t.
 */
bool QAJ4C_is_int( const QAJ4C_Value* value );

/**
 * This method will return the value's int32_t value.
 *
 * @Note: This method should only be invoked on fields that are known to
 * be int32_t. It is usually a good idea to call the QAJ4C_is_int method first!
 */
int32_t QAJ4C_get_int( const QAJ4C_Value* value );

/**
 * This method will return true, in case the value can be read out as int64_t.
 */
bool QAJ4C_is_int64( const QAJ4C_Value* value );

/**
 * This method will return the value's int64_t value.
 *
 * @Note: This method should only be invoked on fields that are known to
 * be int64_t. It is usually a good idea to call the QAJ4C_is_int64 method first!
 */
int64_t QAJ4C_get_int64( const QAJ4C_Value* value );

/**
 * This method will return true, in case the value can be read out as uint32_t.
 */
bool QAJ4C_is_uint( const QAJ4C_Value* value );

/**
 * This method will return the value's uint32_t value.
 *
 * @Note: This method should only be invoked on fields that are known to
 * be uint32_t. It is usually a good idea to call the QAJ4C_is_uint method first!
 */
uint32_t QAJ4C_get_uint( const QAJ4C_Value* value );

/**
 * This method will return true, in case the value can be read out as uint64_t.
 */
bool QAJ4C_is_uint64( const QAJ4C_Value* value );

/**
 * This method will return the value's uint64_t value.
 *
 * @Note: This method should only be invoked on fields that are known to
 * be int64_t. It is usually a good idea to call the QAJ4C_is_uint64 method first!
 */
uint64_t QAJ4C_get_uint64( const QAJ4C_Value* value );

/**
 * This method will return true, in case the value can be read out as double.
 *
 * @Note: this will return true for all parsed numeric values.
 */
bool QAJ4C_is_double( const QAJ4C_Value* value );

/**
 * This method will return the value's double value.
 *
 * @Note: This method should only be invoked on fields that are known to
 * be double. It is usually a good idea to call the QAJ4C_is_double method first!
 */
double QAJ4C_get_double( const QAJ4C_Value* value );

/**
 * This method will return true, in case the type can be read out as boolean.
 */
bool QAJ4C_is_bool( const QAJ4C_Value* value );

/**
 * This method will read the bool value of the 'value'.
 *
 * @Note: This method should only be invoked on fields that are known to
 * be boolean. It is usually a good idea to call the QAJ4C_is_bool method first!
 */
bool QAJ4C_get_bool( const QAJ4C_Value* value );

/**
 * This method will return true, in case the value pointer is null
 * but also in case the value's type is null.
 */
bool QAJ4C_is_null( const QAJ4C_Value* value );

/**
 * Checks if the value is an error (not a valid json type)
 *
 * @Note: In case the parse method will fail and the buffer size is sufficient.
 * The root value will contain an error value, that can be 'queried' for
 * detailed error information.
 */
bool QAJ4C_is_error( const QAJ4C_Value* value );

/**
 * This method will return the json string that caused the error.
 *
 * @Note: This method will fail in case the value is not an error!
 */
const char* QAJ4C_error_get_json( const QAJ4C_Value* value );

/**
 * This method will return an error code specifying the reason why
 * the parse method failed.
 *
 * @Note: This method will fail in case the value is not an error!
 */
QAJ4C_ERROR_CODES QAJ4C_error_get_errno( const QAJ4C_Value* value );

/**
 * This method will return position within the json message where the
 * parse error was triggered.
 *
 * @Note: This method will fail in case the value is not an error!
 */
unsigned QAJ4C_error_get_json_pos( const QAJ4C_Value* value );


/**
 * In case the value is an object, this method will retrieve the object member count.
 */
unsigned QAJ4C_object_size( const QAJ4C_Value* value );

/**
 * In case the value is an object, this method will get the member (containing of key and value).
 * with the given index.
 *
 * @Note: the object is internally organized as c-array. This way random access on index is
 * cheap.
 */
const QAJ4C_Member* QAJ4C_object_get_member( const QAJ4C_Value* value, unsigned index );

/**
 * This method will return the key's value of the member.
 */
const QAJ4C_Value* QAJ4C_member_get_key( const QAJ4C_Member* member );

/**
 * This method will return the value's value of the member.
 */
const QAJ4C_Value* QAJ4C_member_get_value( const QAJ4C_Member* member );

/**
 * In case the value is an object this method will retrieve a member by name with the
 * given size and return the value of the member.
 */
const QAJ4C_Value* QAJ4C_object_get2( const QAJ4C_Value* value, const char* str, size_t len );

/**
 * In case the value is an object this method will retrieve a member by name (using strlen
 * to determine the size) and return the value of the member.
 */
static inline const QAJ4C_Value* QAJ4C_object_get( const QAJ4C_Value* value, const char* str ) {
    return QAJ4C_object_get2(value, str, strlen(str));
}

/**
 * In case the value is an array, this method will return the array size.
 */
unsigned QAJ4C_array_size( const QAJ4C_Value* value );

/**
 * In case the value is an array, this method will return the value at the given
 * index of this array.
 *
 * @Note: a QAJ4C_array is organized as a c-array internally, thus random access
 * is possible with low cost.
 */
const QAJ4C_Value* QAJ4C_array_get( const QAJ4C_Value* value, unsigned index );

/**
 * Initializes the builder with the given buffer.
 */
void QAJ4C_builder_init( QAJ4C_Builder* me, void* buff, size_t buff_size );

/**
 * This method will retrieve the document from the builder.
 * Returns NULL in case the buffer has insufficient size.
 */
QAJ4C_Document* QAJ4C_builder_get_document( QAJ4C_Builder* builder );

/**
 * This method will return the root value non-const document. This method will
 * only return NULL in case the document is NULL itself.
 */
QAJ4C_Value* QAJ4C_get_root_value_rw( QAJ4C_Document* document );

/**
 * This method will set the value to a boolean with the handed over value.
 */
void QAJ4C_set_bool( QAJ4C_Value* value_ptr, bool value );

/**
 * This method will set the value to a int32_t with the handed over value.
 */
void QAJ4C_set_int( QAJ4C_Value* value_ptr, int32_t value );

/**
 * This method will set the value to a int64_t with the handed over value.
 */
void QAJ4C_set_int64( QAJ4C_Value* value_ptr, int64_t value );

/**
 * This method will set the value to a uint32_t with the handed over value.
 */
void QAJ4C_set_uint( QAJ4C_Value* value_ptr, uint32_t value );

/**
 * This method will set the value to a uint64_t with the handed over value.
 */
void QAJ4C_set_uint64( QAJ4C_Value* value_ptr, uint64_t value );

/**
 * This method will set the value to a double with the handed over value.
 */
void QAJ4C_set_double( QAJ4C_Value* value_ptr, double value );

/**
 * This method will set the value to string with a pointer to the handed over
 * string with the given size.
 *
 * @Note: As the string is a reference, the lifetime of the string must at least
 * be as long as the lifetime of the DOM object.
 */
void QAJ4C_set_string_ref2( QAJ4C_Value* value_ptr, const char* str, size_t len );

/**
 * This method will set the value to string with a pointer to the handed over
 * string using strlen() to determine the string size.
 *
 * @Note: As the string is a reference, the lifetime of the string must at least
 * be as long as the lifetime of the DOM object.
 */
static inline void QAJ4C_set_string_ref( QAJ4C_Value* value_ptr, const char* str ) {
    QAJ4C_set_string_ref2(value_ptr, str, strlen(str));
}

/**
 * This method will copy the handed over string with the given string size
 * using the builder.
 */
void QAJ4C_set_string_copy2( QAJ4C_Value* value_ptr, QAJ4C_Builder* builder, const char* str, size_t len );

/**
 * This method will copy the handed over string using the builder. The string
 * size is determined by using strlen.
 */
static inline void QAJ4C_set_string_copy( QAJ4C_Value* value_ptr, QAJ4C_Builder* builder, const char* str ) {
    QAJ4C_set_string_copy2(value_ptr, builder, str, strlen(str));
}

/**
 * This method will set the values type to array with the given value count. The
 * memory allocation will be performed on the builder.
 *
 * @Note: objects and arrays cannot be resized. Thus in case invoking this function twice
 * on the same value will cause the memory allocated for the old "members" to be wasted!
 */
void QAJ4C_set_array( QAJ4C_Value* value_ptr, unsigned count, QAJ4C_Builder* builder );

/**
 * Will retrieve the entry of the array at the given index.
 *
 * @Note: a QAJ4C_array is organized as a c-array internally, thus random access
 * is possible with low cost.
 */
QAJ4C_Value* QAJ4C_array_get_rw( QAJ4C_Value* value_ptr, unsigned index );

/**
 * This method will set the values type to object with the given member count. The
 * memory allocation will be performed on the builder.
 *
 * @Note: objects and arrays cannot be resized. Thus in case invoking this function twice
 * on the same value will cause the memory allocated for the old "members" to be wasted!
 */
void QAJ4C_set_object( QAJ4C_Value* value_ptr, unsigned count, QAJ4C_Builder* builder );

/**
 * This method creates a member within the object using the reference of the handed over string.
 * This way the string does not have to be copied over to the buffer, but the string has to stay
 * valid until the DOM is not required anymore.
 */
QAJ4C_Value* QAJ4C_object_create_member_by_ref2( QAJ4C_Value* value_ptr, const char* str, size_t len );

/**
 * This method creates a member within the object using the reference of the handed over string.
 * This way the string does not have to be copied over to the buffer, but the string has to stay
 * valid until the DOM is not required anymore.
 *
 * @Note: This is a shortcut version of the QAJ4C_object_create_member_by_ref2 method, using
 * strlen to calculate the string length.
 */
static inline QAJ4C_Value* QAJ4C_object_create_member_by_ref( QAJ4C_Value* value_ptr, const char* str ) {
    return QAJ4C_object_create_member_by_ref2(value_ptr, str, strlen(str));
}

/**
 * This method creates a member within the object doing a copy of the handed over string.
 * The allocation will be performed on the handed over builder.
 */
QAJ4C_Value* QAJ4C_object_create_member_by_copy2( QAJ4C_Value* value_ptr, QAJ4C_Builder* builder, const char* str, size_t len );

/**
 * This method creates a member within the object doing a copy of the handed over string.
 * The allocation will be performed on the handed over builder.
 *
 * @Note: This is a shortcut version of the QAJ4C_object_create_member_by_copy2 method, using
 * strlen to calculate the string length.
 */
static inline QAJ4C_Value* QAJ4C_object_create_member_by_copy( QAJ4C_Value* value_ptr, QAJ4C_Builder* builder, const char* str ) {
    return QAJ4C_object_create_member_by_copy2(value_ptr, builder, str, strlen(str));
}

#ifdef __cplusplus
}
#endif

#endif /* QAJ4C_H_ */
