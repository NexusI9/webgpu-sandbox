#include "keyrecord.h"

#include <stddef.h>
#include <emscripten/html5.h>
#include <stdlib.h>

#include "utils/dyli.h"
#include "backend/logger.h"
#include "emscripten/em_types.h"
#include "string.h"
#include "backend/registry.h"
#include "runtime/html_event/add.h"
#include "runtime/html_event/core.h"

static void keyrec_update(KeyRecord *, const keyrec_t, bool);
static bool keyrec_match(KeyRecord *, const keyrec_t *, const size_t);

/**
   Create a new html input callback and update the key record according to the
   input keys.
 */
KeyRecordStatus keyrec_sequence_listener_create(KeyRecordSequenceList *listener,
                                                size_t capacity) {

  // init listener dynamic list
  DynamicListStatus list = dyli_create(
      (void *)&listener->entries, &listener->capacity, &listener->count,
      sizeof(KeyRecordSequence), capacity, "Key Record Sequence List");

  if (list != DynamicListStatus_Success)
    return KeyRecordStatus_CreateFail;

  // init events listeners
  html_event_add_key_down(&(HTMLEventKey){
      .callback = keyrec_html_keydown_callback,
      .data = listener,
  });

  return KeyRecordStatus_Success;
}

KeyRecordStatus keyrec_add_sequence(KeyRecordSequenceList *listener,
                                    KeyRecordSequence *seq) {

  // duplicate sequence keys
  keyrec_t *temp = (keyrec_t *)malloc(sizeof(keyrec_t) * seq->count);

  if (temp == NULL) {
    logger_add(LoggerFlag_Warning, "Could not allocate memory for Key Record sequence.");
    return KeyRecordStatus_AllocFail;
  }

  memcpy(temp, seq->sequence, sizeof(keyrec_t) * seq->count);
  seq->sequence = temp;

  // insert in sequence list
  DynamicListStatus insert = dyli_insert(
      (void *)&listener->entries, &listener->capacity, &listener->count,
      sizeof(KeyRecordSequence), (void *)seq, 1, "Key Record Sequence List");

  if (insert != DynamicListStatus_Success) {
    logger_add(LoggerFlag_Print, "Could not add new sequence to the key record");
    return KeyRecordStatus_InsertFail;
  }

  return KeyRecordStatus_Success;
}

KeyRecordStatus keyrec_destroy_sequence(KeyRecordSequenceList *list,
                                        KeyRecordSequence *seq) {

  // free sequence
  free(seq->sequence);
  seq->callback = NULL;
  seq->data = NULL;
  seq->count = 0;
  seq->owner = REG_OWNER_UNDEFINED;

  // shift list entries
  DynamicListStatus remove =
      dyli_remove((void *)list->entries, &list->count,
                  sizeof(KeyRecordSequence), seq, "Key Record Sequence List");

  if (remove != DynamicListStatus_Success) {
    logger_add(LoggerFlag_Warning, "Could not properly destroy and remove the sequence from "
                    "the key record list.");
    return KeyRecordStatus_RemoveFail;
  }

  return KeyRecordStatus_Success;
}

/**
   Set key record as pressed
 */
bool keyrec_html_keydown_callback(int enventType,
                                  const EmscriptenKeyboardEvent *keyEvent,
                                  void *userData) {

  KeyRecordSequenceList *listener = (KeyRecordSequenceList *)userData;
  unsigned int keyCode = keyEvent->keyCode;

  // update listener record
  keyrec_update(&listener->record, keyCode, true);

  // traverse callbacks and check if matches
  for (size_t i = 0; i < listener->count; i++)
    // if record history match sequence, call entry callback
    if (keyrec_match(&listener->record, listener->entries[i].sequence,
                     listener->entries[i].count))
      listener->entries[i].callback(&listener->entries[i],
                                    listener->entries[i].data);

  return EM_FALSE;
}

void keyrec_update(KeyRecord *record, const keyrec_t key, bool pressed) {

  for (size_t i = 0; i < INPUT_KEY_RECORD_MAX_KEYS; i++)
    // shift the timeline by 1 bit
    record->key[i] <<= 1;

  // if pressed, add a new 1 at the start of the sequence
  record->key[key] |= 0x01;
}

/**
   Traverse the sequence in reverse and check record entries based on the
   sequence key.
   The goes is to basically put the cursor to the right as to compate it with a
   bit mask and check the "&" operation output.
   To do so for each iteration we shift the targeted key value "i time" to the
   right.

   Let's say for the sequence S S X.

                    >>          >>
           i = 0    |   i = 1    |   i = 2
       x  0x000001  |  0x000000  | 0x000000
       s  0x000110  |  0x000011  | 0x000001
       s  0x000110  |  0x000011  | 0x000001
    ----------------+------------+-----------
    mask  0x000001  |  0x000001  | 0x000001

 */

bool keyrec_match(KeyRecord *record, const keyrec_t *seq, const size_t count) {

  const keyrec_t mask = 0x01;
  keyrec_t cursor = 0;

  for (size_t i = count; i-- > 0;) {

    keyrec_t current_char = seq[i];

    // shift by i the record key to the right
    keyrec_t shift_val = record->key[(size_t)current_char] >> cursor;

    // compare with the mask
    // if shifted value is 0 (0x01) means not match returns false
    if ((shift_val & mask) == 0)
      return false;

    // reset cursor if change sequence character
    cursor++;
  }

  return true;
}

KeyRecordSequenceListResult
keyrec_find_sequence_by_id(KeyRecordSequenceList *list, reg_id_t id) {

  KeyRecordSequenceListResult result = {0};

  for (size_t i = 0; i < list->count; i++)
    if (list->entries[i].owner == id &&
        result.count < INPUT_KEY_RECORD_MAX_RESULT)
      result.entries[result.count++] = list->entries[i];

  return result;
}

void keyrec_flush(KeyRecord *record) {
  memset(record->key, 0, INPUT_KEY_RECORD_MAX_KEYS * sizeof(keyrec_t));
}

bool keyrec_sequence_equal(keyrec_t *a, keyrec_t *b, size_t count) {
  return memcmp(a, b, count * sizeof(keyrec_t)) == 0;
}
