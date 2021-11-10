#include "command_responder.h"

// Called every time the results of an audio recognition run are available. The
// human-readable name of any recognized command is in the `found_command`
// argument, `score` has the numerical confidence, and `is_new_command` is set
// if the previous command was different to this one.

int RespondToCommand(tflite::ErrorReporter* error_reporter,
                      int32_t current_time, const char* found_command,
                      uint8_t score, bool is_new_command, char* out) {
  char scoreCh[10];
  char str1[] = "Command recognized: ";
  char str2[] = " at";
  char str3[] = " % confidence";
  strcpy(out, str1);
  strcpy(out, found_command);
  strcpy(out, str2);
  strcpy(out, itoa(score, scoreCh, 10));
  strcpy(out, str3);             
  if (strcmp(found_command,"yes")==0)
  {//do stuff here because it was a match
    // Allow for gesture inputs
    return 1;
  }
  else if (strcmp(found_command,"no")==0)
  {//do stuff here because it was a match
    // Set the light output to 0
    return 0;
  }
  else if (strcmp(found_command,"go")==0)
  {//do stuff here because it was a match
    return 2;
  }     
  return -1;
}
