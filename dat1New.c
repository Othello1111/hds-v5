/*
*+
*  Name:
*     dat1New

*  Purpose:
*     Create a new component in a structure and return a locator

*  Language:
*     Starlink ANSI C

*  Type of Module:
*     Library routine

*  Invocation:
*     HDSLoc * dat1New( const HDSLoc *locator, const char *name_str, const char *type_str,
*                      int ndim, const hdsdim dims[], int * status );

*  Arguments:
*     locator = const HDSLoc * (Given)
*        Locator to structure that will receive the new component.
*     name = const char * (Given)
*        Name of the object in the container.
*     type = const char * (Given)
*        Type of object.  If type matches one of the HDS primitive type names a primitive
*        of that type is created, otherwise the object is assumed to be a structure.
*     ndim = int (Given)
*        Number of dimensions. Use 0 for a scalar. See the Notes for a discussion of
*        arrays of structures.
*     dims = const hdsdim [] (Given)
*        Dimensionality of the object. Should be dimensioned with ndim. The array
*        is not accessed if ndim == 0.
*     status = int* (Given and Returned)
*        Pointer to global status.

*  Description:
*     Creates a new component (primitive type or structure) in an existing structure
*     and return the corresponding locator.

*  Returned Value:
*     HDSLoc * = locator associated with newly created structure. NULL on error.

*  Authors:
*     TIMJ: Tim Jenness (Cornell)
*     {enter_new_authors_here}

*  History:
*     2014-08-20 (TIMJ):
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

#include <stdlib.h>
#include <strings.h>

#include "hdf5.h"
#include "hdf5_hl.h"
#include "ems.h"

#include "hds1.h"
#include "dat1.h"
#include "hds.h"
#include "dat_err.h"
#include "sae_par.h"

HDSLoc *
dat1New( const HDSLoc    *locator,
        const char      *name_str,
        const char      *type_str,
        int       ndim,
        const hdsdim    dims[],
        int       *status) {

  char cleanname[DAT__SZNAM+1];
  char groupstr[DAT__SZTYP+1];

  hid_t group_id = 0;
  hid_t dataset_id = 0;
  hid_t dataspace_id = 0;
  hid_t h5type = 0;
  hid_t place = 0;
  int isprim;
  int typcreat = 0;
  HDSLoc * thisloc = NULL;
  hsize_t h5dims[DAT__MXDIM];

  if (*status != SAI__OK) return NULL;

  /* The name can not have "." in it as this will confuse things
     even though HDF5 will be using a "/" */
  dau1CheckName( name_str, 1, cleanname, sizeof(cleanname), status );
  if (*status != SAI__OK) return NULL;

  /* Copy dimensions if appropriate */
  dat1ImportDims( ndim, dims, h5dims, status );

  /* Work out where to place the component */
  place = dat1RetrieveContainer( locator, status );

  /* Convert the HDS data type to HDF5 data type */
  isprim = dau1CheckType( type_str, &h5type, groupstr,
                          sizeof(groupstr), &typcreat, status );

  /* The above routine has allocated resources so from here we can not
     simply return on error but have to ensure we clean up */

  /* Now create the group or dataset at the top level */
  if (isprim) {
    /* Create a primitive */

    /* Create the data space for the dataset */
    CALLHDF( dataspace_id,
             H5Screate_simple( ndim, h5dims, NULL ),
             DAT__HDF5E,
             emsRepf("dat1New_1", "Error allocating data space", status )
             );

    /* now place the dataset */
    CALLHDF( dataset_id,
             H5Dcreate2(place, cleanname, h5type, dataspace_id,
                        H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT),
             DAT__HDF5E,
             emsRepf("dat1New_2", "Error placing the data space in the file", status )
             );

  } else {
    /* Create a group */
    if (ndim != 0) {
      if (*status == SAI__OK) {
        *status = DAT__DIMIN;
        emsRep("dat1New_3", "Can not support arrays of structures at this time",
               status );
      }
      goto CLEANUP;
    } else {

      CALLHDF( group_id,
               H5Gcreate2(place, cleanname, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT),
               DAT__HDF5E,
               emsRepf("dat1New_4", "Error creating structure/group '%s'", status, cleanname)
               );

      /* Actual data type of the structure/group must be stored in an attribute */
      CALLHDFQ( H5LTset_attribute_string( group_id, ".", "HDSTYPE", groupstr ) );
    }
  }

  /* We now have to store this in a new locator */
  if (*status == SAI__OK) {
    HDSLoc * thisloc = dat1AllocLoc( status );
    thisloc->dataset_id = dataset_id;
    thisloc->group_id = group_id;
    thisloc->dataspace_id = dataspace_id;
    if (typcreat) thisloc->dtype = h5type;
    return thisloc;
  }

 CLEANUP:
  /* Everything should be freed */
  if (typcreat) H5Tclose( h5type );
  if (dataset_id) H5Dclose(dataset_id);
  if (dataspace_id) H5Sclose(dataspace_id);
  if (group_id) H5Gclose(group_id);
  if (thisloc) {
    thisloc = dat1FreeLoc(thisloc, status);
  }
  return NULL;
}
