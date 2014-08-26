/*
*+
*  Name:
*     dat1.h

*  Purpose:
*     Internal support routines for data access

*  Language:
*     Starlink ANSI C

*  Type of Module:
*     Include file

*  Invocation:
*     #include "dat1.h"

*  Description:
*     Defines internal interfaces for data access layer.

*  Authors:
*     TIMJ: Tim Jenness (Cornell)
*     {enter_new_authors_here}

*  History:
*     2014-08-15 (TIMJ):
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
#include "hds1.h"
#include "hds_types.h"

/* Private definition of the HDS locator struct */
typedef struct LOC {
  hid_t file_id;    /* Set if this locator is associated with a root file */
  hid_t dataset_id; /* Set if this is a dataset "primitive type" */
  hid_t dataspace_id; /* Set if this is a primitive with dimensions */
  hid_t group_id;   /* Set if this locator is associated with a group */
  hid_t dtype;      /* Set if a special data type was created for this locator */
} HDSLoc;


/* Global Constants:                                                        */
/* ================                                                         */
/* The following are "public" values. Copies are made available externally  */
/* in the dat_par.h and dat_par(.f) files.                                  */
#define DAT__MXDIM 7             /* Maximum number of object dimensions     */
#define DAT__NOLOC "<NOT A LOCATOR> "/* Null (invalid) locator value        */
#define DAT__NOWLD 0             /* Null wild-card search context           */
#define DAT__ROOT  "<ROOT LOCATOR>  "/* Root locator value                  */
#define DAT__SZGRP 15            /* Size of group name                      */
#define DAT__SZLOC ( ( 15 > (int) sizeof( struct LOC ) ) ? \
                     15 : (int) sizeof( struct LOC ) )
                                 /* Size of locator string                  */
#define DAT__SZMOD 15            /* Size of access mode string              */
#define DAT__SZNAM 15            /* Size of object name                     */
#define DAT__SZTYP 15            /* Size of type string                     */
#define DAT__FLEXT ".h5sdf"      /* Default HDS file extension              */
#define DAT__SZFLX 6             /* Length of DAT__FLEXT                    */

/* Internal Constants */
#define DAT__MXCHR 0xffff        /* Max characters in a character data type */


/* Wrapper to check HDF5 status and map to EMS.
   Also does not call the routine unless status is good.
   Assumes inherited status is available in "status" and
   that there is a CLEANUP label.
 */
#define CALLHDF( retval, hfunc, errcode, errfunc )      \
  if (*status == SAI__OK) {                             \
    herr_t h5err;                                       \
    h5err = hfunc;                                      \
    if (h5err >= 0) {                                   \
      retval = h5err;                                   \
    } else {                                            \
      *status = errcode;                                \
      dat1H5EtoEMS( status );                           \
      errfunc;                                          \
      goto CLEANUP;                                     \
    }                                                   \
  }

/* Simpler quick wrapper when we do not care about the return value
   or including an explicit error message */
#define CALLHDFQ( hfunc )                                               \
  if (*status == SAI__OK) {                                             \
    herr_t h5err;                                                       \
    h5err = hfunc;                                                      \
    if (h5err < 0) {                                                    \
      *status = DAT__HDF5E;                                             \
      dat1H5EtoEMS( status );                                           \
      emsRepf("HDF5ERR","Error calling HDF5 with '%s'", status, "" #hfunc ""); \
      goto CLEANUP;                                                     \
    }                                                                   \
  }

/* Internal API prototypes */

void dau1CheckName( const char * name, int isname, char * buf,
                    size_t buflen, int * status );

int dau1CheckType ( const char * type_str, hid_t * h5type,
                    char * norm_str, size_t normlen, int *typcreat, int * status );

void dat1InitHDF5(void);

void dat1H5EtoEMS( int *status );

HDSLoc *
dat1New( const HDSLoc *locator, const char *name_str, const char *type_str,
        int ndim, const hdsdim dims[], int *status);

hid_t dat1RetrieveContainer( const HDSLoc *locator, int * status );

HDSLoc * dat1FreeLoc( HDSLoc * locator, int * status );

HDSLoc * dat1AllocLoc( int * status );
