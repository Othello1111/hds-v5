/*
*+
*  Name:
*     datSlice

*  Purpose:
*     Locate slice

*  Language:
*     Starlink ANSI C

*  Type of Module:
*     Library routine

*  Invocation:
*     datSlice(const HDSLoc *locator1, int ndim, const hdsdim lower[],
*              const hdsdim upper[], HDSLoc  **locator2, int *status );

*  Arguments:
*     locator1 = const HDSLoc * (Given)
*        Array locator. Currently must be primitive type.
*     ndim = int (Given)
*        Number of dimensions.
*     lower = const hdsdim [] (Given)
*        Lower dimension bounds. 1-based.
*     upper = const hdsdim [] (Given)
*        Upper dimension bounds. 1-based. If any of the upper bounds
*        are zero or negative the full upper dimension is used instead.
*     locator2 = HDSLoc ** (Returned)
*        Slice locator.
*     status = int* (Given and Returned)
*        Pointer to global status.

*  Description:
*     Return a locator to a "slice" of a vector or an array.

*  Authors:
*     TIMJ: Tim Jenness (Cornell)
*     {enter_new_authors_here}

*  History:
*     2014-09-08 (TIMJ):
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
#include "hdf5_hl.h"

#include "ems.h"
#include "sae_par.h"

#include "hds1.h"
#include "dat1.h"
#include "hds.h"

#include "dat_err.h"

int
datSlice(const HDSLoc *locator1, int ndim, const hdsdim lower[],
         const hdsdim upper[], HDSLoc  **locator2, int *status ) {
  hdsdim loc1dims[DAT__MXDIM];
  int loc1ndims = 0;
  hsize_t h5lower[DAT__MXDIM];
  hsize_t h5upper[DAT__MXDIM];
  hsize_t h5dims[DAT__MXDIM];
  HDSLoc * sliceloc = NULL;
  int i = 0;

  if (*status != SAI__OK) return *status;

  /* We only work with primitives at the moment */
  if (dat1IsStructure( locator1, status ) ) {
    *status = DAT__OBJIN;
    emsRep("datSlice_1", "datSlice only works with primitive datasets",
           status);
    return *status;
  }

  /* Get the shape of the input locator and validate dimensionality */
  datShape( locator1, DAT__MXDIM, loc1dims, &loc1ndims, status );

  if (loc1ndims == 0) {
    if (*status == SAI__OK) {
      *status = DAT__DIMIN;
      emsRep("datSlice_2", "Can not use datSlice for scalar primitive "
             "(possible programming error)", status);
    }
  }

  if (loc1ndims != ndim) {
    if (*status != SAI__OK) {
      *status = DAT__DIMIN;
      emsRepf("datSlice_3", "datSlice: Arguments have %d axes but locator refers to %d axes",
              status, ndim, loc1ndims);
    }
  }

  if (*status != SAI__OK) return *status;

  /* import the bounds */
  dat1ImportDims( ndim, lower, h5lower, status );
  dat1ImportDims( ndim, upper, h5upper, status );
  dat1ImportDims( ndim, loc1dims, h5dims, status );

  /* Check that the upper bounds are greater than the lower
     bounds and within h5dims. Cap at h5dims if zero is given. */
  for (i=0; i<ndim; i++) {
    if (*status != SAI__OK) break;

    if ( h5lower[i] < 1 || h5lower[i] > h5dims[i] ) {
      *status = DAT__DIMIN;
      emsRepf("datSlice_4", "datSlice: lower bound %d is out of bounds 1 <= %llu <= %llu",
              status, i, (unsigned long long)h5lower[i], (unsigned long long)h5dims[i] );
      break;
    }

    if (h5upper[i] <= 0) h5upper[i] = h5dims[i];

    if ( h5upper[i] < h5lower[i] || h5upper[i] > h5dims[i] ) {
      *status = DAT__DIMIN;
      emsRepf("datSlice_4", "datSlice: upper bound %d is out of bounds %llu <= %llu <= %llu",
              status, i, (unsigned long long)h5lower[i],
              (unsigned long long)h5upper[i], (unsigned long long)h5dims[i] );
      break;
    }
  }

  /* Clone the locator and modify its dataspace */
  datClone( locator1, &sliceloc, status );

  if (*status == SAI__OK) {
    hsize_t h5count[DAT__MXDIM];

    /* Calculate the number of elements but also remember that
       HDF5 will be using 0-based counting */
    for (i=0; i<ndim; i++) {
      h5count[i] = h5upper[i] - h5lower[i] + 1;
      h5lower[i]--;
    }

    CALLHDFQ( H5Sselect_hyperslab( sliceloc->dataspace_id, H5S_SELECT_SET, h5lower,
                                   NULL, h5count, NULL) );

  }

 CLEANUP:
  if (*status != SAI__OK) {
    if (sliceloc) datAnnul( &sliceloc, status );
  } else {
    *locator2 = sliceloc;
  }

  return *status;
}