#ifndef _INPUT_KEY_RECORD_H_
#define _INPUT_KEY_RECORD_H_

#include "../backend/registry.h"
#include "emscripten/html5.h"
#include <cglm/cglm.h>
#include <stddef.h>
#include <stdint.h>

#define INPUT_KEY_RECORD_MAX_KEYS 256
#define INPUT_KEY_RECORD_SEQUENCE_MAX_KEY 4
#define INPUT_KEY_RECORD_MAX_RESULT 64
#define INPUT_KEY_RECORD_CAPACITY 64

typedef enum {
  KeyRecordStatus_Success,
  KeyRecordStatus_AllocFail,
  KeyRecordStatus_CreateFail,
  KeyRecordStatus_InsertFail,
  KeyRecordStatus_RemoveFail,
  KeyRecordStatus_UndefError,
} KeyRecordStatus;

/**
   The Key Recorder use Bitwise operations to record input keys.
   It uses bits as counters and shift each key each keydown ( << 1)

                        's' => 0x000000

   * press 's' 1 *      's' => 0x000001

   * press 's' 2 *      's' => 0x000011

   * press 'd' 1 *      's' => 0x000110
                        'd' => 0x000001
                              '--------'
                               timeline

   Here the bits act as a timeline and offer an history that spans over 5
   states, which is enough since usually we either want to check a combination
   of 2 or maybe 3 keys.

   Using bitfield benefit from being lightweight and compact compared to a
   string based history.
 */

typedef uint8_t key_t;

typedef struct KeyRecordSequence KeyRecordSequence;

typedef struct {
  key_t key[INPUT_KEY_RECORD_MAX_KEYS];
} KeyRecord;

/**
   Key Record Sequence Listener
   Offers an interface to link a sequence of keys with a certain callback.
   We will mostly used the global input (g_input.sequence_listener) to subcribe
   new sequences with their respective callback.

   { "S", "S", "Y" }  == call ==> function();

 */
typedef void (*input_keyrec_callback)(KeyRecordSequence *, void *);

struct KeyRecordSequence {
  key_t *sequence;
  size_t length;
  input_keyrec_callback callback;
  void *data;
  id_t owner;
};

typedef struct {
  KeyRecord record;
  KeyRecordSequence *entries;
  size_t capacity;
  size_t length;
} KeyRecordSequenceList;

typedef struct {
  KeyRecordSequence entries[INPUT_KEY_RECORD_MAX_RESULT];
  size_t length;
} KeyRecordSequenceListResult;

// Core
KeyRecordStatus input_keyrec_sequence_listener_create(KeyRecordSequenceList *,
                                                      size_t);

KeyRecordStatus input_keyrec_add_sequence(KeyRecordSequenceList *,
                                          KeyRecordSequence *);

KeyRecordStatus input_keyrec_destroy_sequence(KeyRecordSequenceList *,
                                              KeyRecordSequence *);

KeyRecordSequenceListResult
input_keyrec_find_sequence_by_id(KeyRecordSequenceList *, id_t);

// HTML Callbacks
bool input_keyrec_html_keydown_callback(int, const EmscriptenKeyboardEvent *,
                                        void *);

void input_keyrec_flush(KeyRecord*);

#endif
