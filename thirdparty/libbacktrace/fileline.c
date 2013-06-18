/* fileline.c -- Get file and line number information in a backtrace.
   Copyright (C) 2012-2013 Free Software Foundation, Inc.
   Written by Ian Lance Taylor, Google.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    (1) Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer. 

    (2) Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the
    distribution.  
    
    (3) The name of the author may not be used to
    endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.  */

#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

#include "backtrace.h"
#include "internal.h"

#ifndef HAVE_GETEXECNAME
#define getexecname() NULL
#endif

/* Initialize the fileline information from the executable.  Returns 1
   on success, 0 on failure.  */

static int
fileline_initialize (struct backtrace_state *state,
		     backtrace_error_callback error_callback, void *data)
{
  int failed;
  fileline fileline_fn;
  int pass;
  int called_error_callback;
  int descriptor;

  failed = state->fileline_initialization_failed;

  if (state->threaded)
    {
      /* Use __sync_bool_compare_and_swap to do an atomic load.  */
      while (!__sync_bool_compare_and_swap
	     (&state->fileline_initialization_failed, failed, failed))
	failed = state->fileline_initialization_failed;
    }

  if (failed)
    {
      error_callback (data, "failed to read executable information", -1);
      return 0;
    }

  fileline_fn = state->fileline_fn;
  if (state->threaded)
    {
      while (!__sync_bool_compare_and_swap (&state->fileline_fn, fileline_fn,
					    fileline_fn))
	fileline_fn = state->fileline_fn;
    }
  if (fileline_fn != NULL)
    return 1;

  /* We have not initialized the information.  Do it now.  */

  descriptor = -1;
  called_error_callback = 0;
  for (pass = 0; pass < 4; ++pass)
    {
      const char *filename;
      int does_not_exist;

      switch (pass)
	{
	case 0:
	  filename = state->filename;
	  break;
	case 1:
	  filename = getexecname ();
	  break;
	case 2:
	  filename = "/proc/self/exe";
	  break;
	case 3:
	  filename = "/proc/curproc/file";
	  break;
	default:
	  abort ();
	}

      if (filename == NULL)
	continue;

      descriptor = backtrace_open (filename, error_callback, data,
				   &does_not_exist);
      if (descriptor < 0 && !does_not_exist)
	{
	  called_error_callback = 1;
	  break;
	}
      if (descriptor >= 0)
	break;
    }

  if (descriptor < 0)
    {
      if (!called_error_callback)
	{
	  if (state->filename != NULL)
	    error_callback (data, state->filename, ENOENT);
	  else
	    error_callback (data,
			    "libbacktrace could not find executable to open",
			    0);
	}
      failed = 1;
    }

  if (!failed)
    {
      if (!backtrace_initialize (state, descriptor, error_callback, data,
				 &fileline_fn))
	failed = 1;
    }

  if (failed)
    {
      if (!state->threaded)
	state->fileline_initialization_failed = 1;
      else
	__sync_bool_compare_and_swap (&state->fileline_initialization_failed,
				      0, failed);
      return 0;
    }

  if (!state->threaded)
    state->fileline_fn = fileline_fn;
  else
    {
      __sync_bool_compare_and_swap (&state->fileline_fn, NULL, fileline_fn);

      /* At this point we know that state->fileline_fn is not NULL.
	 Either we stored our value, or some other thread stored its
	 value.  If some other thread stored its value, we leak the
	 one we just initialized.  Either way, state->fileline_fn is
	 initialized.  The compare_and_swap is a full memory barrier,
	 so we should have full access to that value even if it was
	 created by another thread.  */
    }

  return 1;
}

/* Given a PC, find the file name, line number, and function name.  */

int
backtrace_pcinfo (struct backtrace_state *state, uintptr_t pc,
		  backtrace_full_callback callback,
		  backtrace_error_callback error_callback, void *data)
{
  if (!fileline_initialize (state, error_callback, data))
    return 0;

  if (state->fileline_initialization_failed)
    return 0;

  return state->fileline_fn (state, pc, callback, error_callback, data);
}

/* Given a PC, find the symbol for it, and its value.  */

int
backtrace_syminfo (struct backtrace_state *state, uintptr_t pc,
		   backtrace_syminfo_callback callback,
		   backtrace_error_callback error_callback, void *data)
{
  if (!fileline_initialize (state, error_callback, data))
    return 0;

  if (state->fileline_initialization_failed)
    return 0;

  state->syminfo_fn (state, pc, callback, error_callback, data);
  return 1;
}
