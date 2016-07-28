/*
 * taskcall.h
 *
 *  Created on: 17.07.2015
 *      Author: Michael Heidinger
 */

#ifndef INCLUDES_TASKCALL_H_
#define INCLUDES_TASKCALL_H_

#include <stdint.h>

typedef void (*prolog_ptr_f) (void);
typedef void (*task_ptr_f) (int64_t delay);
typedef void (*epilog_ptr_f) (void);

typedef struct taskcall
{
  char taskname[10];                    /* Name of the Task (human readable) */
  int64_t time;
  prolog_ptr_f prolog_ptr;
  task_ptr_f task_ptr;
  epilog_ptr_f epilog_ptr;
  struct taskcall *next;
} taskcall_t;



void _taskcall_task_register(taskcall_t* task_ptr);
void _taskcall_task_register_time(taskcall_t* task_ptr, int64_t time);
void _taskcall_exec(void);
void _taskcall_start(void);

#endif
