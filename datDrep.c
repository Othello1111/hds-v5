/*
*+
*  Name:
*     datDrep

*  Purpose:
*     Obtain primitive data representation information.

*  Language:
*     Starlink ANSI C

*  Type of Module:
*     Library routine

*  Invocation:
*     datDrep(const HDSLoc *locator, const char **format_str, const char **order_str,
*             int *status);

*  Arguments:
*     locator = const HDSLoc * (Given)
*        Primitive object locator.
*     format_str = const char ** (Returned)
*        Description of the format used to encode each data element.
*     order_str = const char ** (Returned)
*        Description of the (byte) storage order used for each data element.
*     status = int* (Given and Returned)
*        Pointer to global status.

*  Description:
*     The routine returns information describing how the data stored
*     in a primitive object are actually represented. An object's data
*     representation will match that used by the computer system on
*     which it was created, and this forms a permanent attribute of
*     the object. If necessary, HDS will automatically perform
*     conversion to the representation used by the host computer when
*     the data are accessed (except when using datBasic, which
*     provides direct access to the data without conversion).

*  Authors:
*     TIMJ: Tim Jenness (Cornell)
*     {enter_new_authors_here}

*  Notes:
*     - Not yet implemented

*  History:
*     2014-10-15 (TIMJ):
*        Initial version
*     {enter_further_changes_here}

*  Copyright:
*     Copyright (C) 2014 Cornell University
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

#include "hdf5.h"

#include "ems.h"
#include "sae_par.h"

#include "hds1.h"
#include "dat1.h"
#include "hds.h"

#include "dat_err.h"

int
datDrep(const HDSLoc *locator, char **format_str, char **order_str,
        int *status) {

  if (*status != SAI__OK) return *status;

  *status = DAT__FATAL;
  emsRep("datDrep", "datDrep: Not yet implemented for HDF5",
         status);

  return *status;
}
