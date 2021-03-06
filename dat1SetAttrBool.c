/*
*+
*  Name:
*     dat1SetAttrBool

*  Purpose:
*     Store scalar boolean value in an HDF5 attribute

*  Language:
*     Starlink ANSI C

*  Type of Module:
*     Library routine

*  Invocation:
*     dat1SetAttrBool( hid_t obj_id, const char * attrname,
*                      hdsbool_t value, int * status );

*  Arguments:
*     obj_id = hid_t (Given)
*        HDF5 object to associate with attribute.
*     attrname = const char * (Given)
*        Name of attribute.
*     value = hdsbool_t (Given)
*        Value to store in attribute.
*     status = int* (Given and Returned)
*        Pointer to global status.

*  Description:
*     Store a single boolean value in an attribute associated
*     with the specified HDF5 object.

*  Authors:
*     TIMJ: Tim Jenness (Cornell)
*     {enter_new_authors_here}

*  History:
*     2014-11-17 (TIMJ):
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

void
dat1SetAttrBool( hid_t obj_id, const char * attrname,
                 hdsbool_t value, int * status ) {

  hid_t attrtype = 0;
  unsigned char bvalue;

  if (*status != SAI__OK) return;

  CALLHDFE( hid_t, attrtype,
           H5Tcopy(H5T_NATIVE_B8),
           DAT__HDF5E,
           emsRepf("dat1SetAttrBool_1", "Error copying data type during writing of attribute '%s'", status, attrname );
           );

  bvalue = ( value ? 1 : 0 );
  dat1SetAttr( obj_id, attrname, attrtype, 0, &bvalue, status );

 CLEANUP:
  if (attrtype > 0) H5Tclose(attrtype);
  return;
}
