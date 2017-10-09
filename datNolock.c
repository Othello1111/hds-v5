/*
*+
*  Name:
*     datNolock

*  Purpose:
*     Indicate that an object should be used without checking its lock.

*  Language:
*     Starlink ANSI C

*  Type of Module:
*     Library routine

*  Invocation:
*     datNolock( HDSLoc *locator, int *status );

*  Arguments:
*     locator = HDSLoc * (Given)
*        Locator to the object that is to be modified.
*     status = int* (Given and Returned)
*        Pointer to global status.

*  Description:
*     By default, every HDS function will check that the supplied locator
*     is locked for use by the current thread before using it, and issue
*     an error report if it is not suitably locked. In some cases however,
*     this check is not required and can cause problems. This function can
*     be used to supres the check when required. It stores a flag with the
*     given locator indicating that no such checks should be performed
*     before using the locator.
*
*     For instance, objects created within Fortran code may require this
*     flag to be set.

*  Authors:
*     DSB: David S Berry (DSB)
*     {enter_new_authors_here}

*  History:
*     9-OCT-2017 (DSB):
*        Initial version
*     {enter_further_changes_here}

*  Copyright:
*     Copyright (C) 2017 East Asian Observatory.
*     All Rights Reserved.

*  Licence:
*     Redistribution and use in source and binary forms, with or
*     without modification, are permitted provided that the following
*     conditions are met:
*
*     - Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*
*     - Redistributions in binary form must reproduce the above
*       copyright notice, this list of conditions and the following
*       disclaimer in the documentation and/or other materials
*       provided with the distribution.
*
*     - Neither the name of the {organization} nor the names of its
*       contributors may be used to endorse or promote products
*       derived from this software without specific prior written
*       permission.
*
*     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
*     CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*     INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
*     MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
*     CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*     SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*     LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*     USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
*     AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*     LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
*     IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
*     THE POSSIBILITY OF SUCH DAMAGE.

*  Bugs:
*     {note_any_bugs_here}
*-
*/

#include "sae_par.h"
#include "dat1.h"
#include "hds.h"

int datNolock( HDSLoc *locator, int *status ) {

/* Check inherited status. */
   if (*status != SAI__OK) return *status;

/* Validate input locator. */
   dat1ValidateLocator( "datNolock", 0, locator, 0, status );

/* Check we can de-reference "locator" safely. */
   if( *status == SAI__OK ) {

/* Set the flag. */
      locator->handle->docheck = 0;
   }

   return *status;
}

