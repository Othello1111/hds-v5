/*
*+
*  Name:
*    hdsDimC2F

*  Purpose:
*    Convert an array of hdsdim[] to F77_INTEGER_TYPE[]

*  Invocation:
*    outdims = hdsDimC2F( int ndim, const hdsdim dims[],
*                         F77_INTEGER_TYPE fdims[DAT__MXDIM], int * status );

*  Description:
*    This function should be used to convert an array of dimensions
*    of type hdsdim to an array of dimensions suitable for Fortran
*    usage. Returns a pointer to an array of F77_INTEGER_TYPE suitable
*    for the Fortran routine and always fills the Fortran array.

*  Arguments
*    ndim = int (Given)
*       Number of relevant dimensions. Should not exceed DAT__MXDIM.
*    dims[] = const hdsdim (Given)
*       Input dimensions to copy, of size ndim.
*    fdims[DAT__MXDIM] = F77_INTEGER_TYPE (Given and Returned)
*       Buffer space that can be used to store the copied dimensions.
*       Will contain the dimensions even if the types for dims and
*       fdims match.
*    int *status = Given and Returned
*       Inherited status. If set, this routine will return NULL.

*  Return Value:
*    outdims = F77_INTEGER_TYPE*
*       Pointer to an array of Fortran integers containing the dimensions.

*  Authors:
*    Tim Jenness (JAC, Hawaii)
*    Tim Jenness (Cornell University)

*  History:
*    11-JUL-2005 (TIMJ):
*      Initial version
*    2014-09-15 (TIMJ):
*      For HDS-H5. To ensure that this routine can be used in the fortran
*      interface we now always guarantee to fill fdims. This is a change of
*      behavior but allows HDSDIM2INT to be removed from interfaces.

*  Notes:
*    - This routine is commonly used to copy the output of a C routine
*      to a pre-existing Fortran buffer. This requires that fdims[] is
*      filled regardless of type matching. If the types match then a
*      fast memmove will be used to do the filling rather than a for loop.
*    - Status will be set to bad if the C dimension can not fit into
*      the corresponding Fortran integer.
*    - The expectation is that this routine is used solely for C
*      interfaces to Fortran library routines.
*    - fdims is always filled, so the return value is now always fdims.
*      It is retained for API compatibility.

*  Copyright:
*    Copyright (C) 2014 Cornell University.
*    Copyright (C) 2006 Particle Physics and Astronomy Research Council.
*    All Rights Reserved.

*  Licence:
*     This program is free software; you can redistribute it and/or
*     modify it under the terms of the GNU General Public License as
*     published by the Free Software Foundation; either version 2 of
*     the License, or (at your option) any later version.
*
*     This program is distributed in the hope that it will be
*     useful, but WITHOUT ANY WARRANTY; without even the implied
*     warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
*     PURPOSE. See the GNU General Public License for more details.
*
*     You should have received a copy of the GNU General Public
*     License along with this program; if not, write to the Free
*     Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
*     MA 02110-1301, USA

*  Bugs:
*     {note_any_bugs_here}

*-
*/

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "ems.h"
#include "star/mem.h"
#include "sae_par.h"

#include "hds1.h"
#include "dat1.h"
#include "hds_types.h"
#include "dat_err.h"

#include "hds1_types.h"
#include "hds_fortran.h"


F77_INTEGER_TYPE *
hdsDimC2F( int ndim, const hdsdim dims[],
	   F77_INTEGER_TYPE fdims[DAT__MXDIM], int * status ) {

#if HDS_COPY_FORTRAN_DIMS
  int i;   /* loop counter */
#endif
  F77_INTEGER_TYPE * retval = NULL;

  if ( *status != SAI__OK ) return NULL;

#if HDS_COPY_FORTRAN_DIMS
  /* sizes or signs differ so we need to copy one at a time
     and cast to the new type */

  for (i = 0; i < ndim; i++ ) {
    /* need to test for overflow - compare hdsdim to fortran max. Assume
       Fortran is a signed 32-bit int and negative dims will not
       be allowed (unsigned int would fit but fortran would treat that
       as negative dim). Do not test INT_MIN.
     */
    HDSDIM2INT( "hdsDimC2F", dims[i], fdims[i], status );
  }

  /* check status is good before deciding to use this array */
  if (*status == SAI__OK) retval = fdims;

#else
  /* hdsdim is the same size and sign so fast copy can
     be used. */
  memmove( fdims, dims, ndim*sizeof(*fdims) );
  retval = fdims;
#endif

  return retval;

}

